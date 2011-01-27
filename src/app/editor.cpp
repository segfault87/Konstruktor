// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include <libldr/metrics.h>
#include <libldr/model.h>

#include <QAction>
#include <QCursor>

#include <kactioncollection.h>
#include <kicon.h>
#include <kmenu.h>
#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>

#include "application.h"
#include "colordialog.h"
#include "colormanager.h"
#include "commandcolor.h"
#include "commandinsert.h"
#include "commandremove.h"
#include "commandtransform.h"
#include "editor.h"
#include "utils.h"

namespace Konstruktor
{

Editor::Editor(QObject *parent)
	: QUndoGroup(parent)
{
	gridMode_ = Grid10;
	
	activeStack_ = 0L;
	lastIndex_ = 0;
	selection_ = 0L;
	model_ = 0L;
	
	connect(this, SIGNAL(activeStackChanged(QUndoStack *)), this, SLOT(activeChanged(QUndoStack *)));
	connect(this, SIGNAL(indexChanged(int)), this, SLOT(indexChanged(int)));
}

Editor::~Editor()
{

}

QAction* Editor::createRedoAction(KActionCollection *actionCollection, const QString &actionName)
{
	QAction *action = QUndoGroup::createRedoAction(actionCollection);

	if (actionName.isEmpty()) {
		action->setObjectName(KStandardAction::name(KStandardAction::Redo));
	} else {
		action->setObjectName(actionName);
	}

	action->setIcon(KIcon("edit-redo"));
	action->setIconText(i18n("Redo"));
	action->setShortcuts(KStandardShortcut::redo());

	actionCollection->addAction(action->objectName(), action);

	return action;
}

QAction* Editor::createUndoAction(KActionCollection *actionCollection, const QString &actionName)
{
	QAction *action = QUndoGroup::createUndoAction(actionCollection);

	if (actionName.isEmpty()) {
		action->setObjectName(KStandardAction::name(KStandardAction::Undo));
	} else {
		action->setObjectName(actionName);
	}

	action->setIcon(KIcon("edit-undo"));
	action->setIconText(i18n("Undo"));
	action->setShortcuts(KStandardShortcut::undo());

	actionCollection->addAction(action->objectName(), action);

	return action;
}

float Editor::snap(float v) const
{
	float mod = Utils::floatModulo(std::fabs(v), gridDensity());
	
	if (v >= 0.0f)
		return v - mod;
	else
		return v + mod;
}

float Editor::snapYAxis(float v) const
{
	float mod = Utils::floatModulo(std::fabs(v), gridDensityYAxis());
	
	if (v >= 0.0f)
		return v - mod;
	else
		return v + mod;
}

float Editor::gridDensity() const
{
	switch (gridMode_) {
		case Grid20:
			return 20.0f;
		case Grid10:
			return 10.0f;
		case Grid5:
			return 5.0f;
		default:
			return 1.0f;
	}
}

float Editor::gridDensityYAxis() const
{
	switch (gridMode_) {
		case Grid20:
			return 24.0f;
		case Grid10:
			return 8.0f;
		case Grid5:
			return 4.0f;
		default:
			return 1.0f;
	}
}

float Editor::gridDensityAngle() const
{
	switch (gridMode_) {
		case Grid20:
			return M_PI / 2.0f;
		case Grid10:
			return M_PI / 4.0f;
		case Grid5:
			return M_PI / 8.0f;
		default:
			return M_PI / 16.0f;
	}
}

void Editor::selectionChanged(const QSet<int> &selection)
{
	selection_ = &selection;
}

void Editor::modelChanged(ldraw::model *model)
{
	model_ = model;
}

void Editor::activeChanged(QUndoStack *stack)
{
	activeStack_ = stack;

	if (stack)
		lastIndex_ = stack->index();
	else
		lastIndex_ = 0;
}

void Editor::stackAdded(QUndoStack *stack)
{
	addStack(stack);
}

void Editor::setGridMode(GridMode mode)
{
	gridMode_ = mode;
}

void Editor::deleteSelected()
{
	if (!activeStack() || selection_->empty())
		return;

	activeStack()->push(new CommandRemove(*selection_, model_));

	emit modified();
}

void Editor::editColor()
{
	if (!activeStack() || selection_->empty())
		return;

	KMenu menu(i18n("Select Color"));
	menu.addTitle(i18n("Shortcuts"));

	ColorManager *cm = Application::self()->colorManager();

	foreach (const ldraw::color &it, cm->colorList()) {
		QAction *action = menu.addAction(QIcon(ColorManager::colorPixmap(it)), it.get_entity()->name.c_str());
		action->setData(it.get_id());
	}

	menu.addTitle(i18n("Recently Used"));

	foreach (const ColorManager::RecentColorPair &it, cm->recentlyUsed()) {
		QAction *action = menu.addAction(QIcon(ColorManager::colorPixmap(it.first)), it.first.get_entity()->name.c_str());
		action->setData(it.first.get_id());
	}
	
	menu.addSeparator();
	QAction *customize = menu.addAction(i18n("&More..."));

	QAction *result = menu.exec(QCursor::pos());
	if (result) {
		if (result == customize) {
			ColorDialog *colordialog = new ColorDialog(Application::self()->rootWindow());

			if (colordialog->exec() == QDialog::Accepted) {
				activeStack()->push(new CommandColor(colordialog->getSelected(), *selection_, model_));
				cm->hit(colordialog->getSelected());
				
				emit modified();
			}
			
			delete colordialog;
		} else {
			ldraw::color selected(result->data().toInt());
			
			activeStack()->push(new CommandColor(selected, *selection_, model_));
			cm->hit(selected);
			
			emit modified();
		}
	}
}

void Editor::move(const ldraw::vector &vector)
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(vector);

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByXPositive()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(gridDensity(), 0.0f, 0.0f));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByXNegative()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(-gridDensity(), 0.0f, 0.0f));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByYPositive()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(0.0f, gridDensityYAxis(), 0.0f));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByYNegative()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(0.0f, -gridDensityYAxis(), 0.0f));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByZPositive()
{	
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(0.0f, 0.0f, gridDensity()));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::moveByZNegative()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	m.set_translation_vector(ldraw::vector(0.0f, 0.0f, -gridDensity()));

	activeStack()->push(new CommandTransform(true, m, *selection_, model_));

	emit modified();
}

void Editor::rotateByXClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = gridDensityAngle();
	m.value(1, 1) = std::cos(angle);
	m.value(1, 2) = std::sin(-angle);
	m.value(2, 1) = std::sin(angle);
	m.value(2, 2) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}

void Editor::rotateByXCounterClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = -gridDensityAngle();
	m.value(1, 1) = std::cos(angle);
	m.value(1, 2) = std::sin(-angle);
	m.value(2, 1) = std::sin(angle);
	m.value(2, 2) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}

void Editor::rotateByYClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = gridDensityAngle();
	m.value(0, 0) = std::cos(angle);
	m.value(2, 0) = std::sin(-angle);
	m.value(0, 2) = std::sin(angle);
	m.value(2, 2) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}

void Editor::rotateByYCounterClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = -gridDensityAngle();
	m.value(0, 0) = std::cos(angle);
	m.value(2, 0) = std::sin(-angle);
	m.value(0, 2) = std::sin(angle);
	m.value(2, 2) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}

void Editor::rotateByZClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = gridDensityAngle();
	m.value(0, 0) = std::cos(angle);
	m.value(0, 1) = std::sin(-angle);
	m.value(1, 0) = std::sin(angle);
	m.value(1, 1) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}
	
void Editor::rotateByZCounterClockwise()
{
	if (!activeStack() || selection_->empty())
		return;

	ldraw::matrix m;
	float angle = -gridDensityAngle();
	m.value(0, 0) = std::cos(angle);
	m.value(0, 1) = std::sin(-angle);
	m.value(1, 0) = std::sin(angle);
	m.value(1, 1) = std::cos(angle);

	activeStack()->push(new CommandTransform(false, m, *selection_, model_));

	emit modified();
}

void Editor::insert(const QString &filename, const ldraw::matrix &matrix, const ldraw::color &color)
{
	if (selection_)
		activeStack()->push(new CommandInsert(filename, matrix, color, *selection_, model_));
	else
		activeStack()->push(new CommandInsert(filename, matrix, color, QSet<int>(), model_));

	emit modified();
}

// after changes are made
void Editor::indexChanged(int index)
{
	if (activeStack_ != activeStack() || index > activeStack()->count())
		return;

	int s, e;
	bool redo;
	if (lastIndex_ < index) {
		// Redo
		redo = true;
		s = lastIndex_ + 1;
		e = index;
	} else {
		// Undo
		redo = false;
		s = index + 1;
		e = lastIndex_;
	}

	for (int i = s; i <= e; ++i) {
		const CommandBase *cmd = dynamic_cast<const CommandBase *>(activeStack_->command(i - 1));
		if (cmd->needRepaint()) {
			emit needRepaint();
			break;
		}
	}
	
	for (int i = s; i <= e; ++i) {
		const CommandBase *cmd = dynamic_cast<const CommandBase *>(activeStack_->command(i - 1));
		if (cmd->needUpdateDimension()) {
			const_cast<CommandBase *>(cmd)->model()->update_custom_data<ldraw::metrics>();
			break;
		}
	}

	for (int i = s; i <= e; ++i) {
		const CommandBase *cmd = dynamic_cast<const CommandBase *>(activeStack_->command(i - 1));
		QPair<CommandBase::AffectedRow, QSet<int> > affected = cmd->affectedRows();
		if (affected.second.size()) {
			if (!redo) {
				// flip
				if (affected.first == CommandBase::Inserted)
					affected.first = CommandBase::Removed;
				else
					affected.first = CommandBase::Inserted;
			}
			
			emit rowsChanged(affected);
		}
	}

	lastIndex_ = index;
}

}
