// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include <libldr/metrics.h>
#include <libldr/model.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QMenu>
#include <QMimeData>

#include "application.h"
#include "colordialog.h"
#include "colormanager.h"
#include "commandcolor.h"
#include "commandinsert.h"
#include "commandpaste.h"
#include "commandremove.h"
#include "commandtransform.h"
#include "commandtransformlinear.h"
#include "editor.h"
#include "objectlist.h"
#include "utils.h"

namespace Konstruktor
{

UndoAction::UndoAction(const QString &prefix, QObject *parent)
    : QAction(parent)
{
  prefix_ = prefix;
}

void UndoAction::setPrefixedText(const QString &text)
{
  QString s = prefix_;
  
  if (!prefix_.isEmpty() && !text.isEmpty())
    s.append(QLatin1Char(' '));
  
  s.append(text);
  
  setText(s);
}

Editor::Editor(QObject *parent)
    : QUndoGroup(parent)
{
  gridMode_ = Grid10;
  pivot_ = PivotEach;
  
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

QAction* Editor::createRedoAction()
{
  UndoAction *action = new UndoAction(tr("Redo"));
  
  action->setEnabled(canRedo());
  action->setPrefixedText(redoText());
  
  connect(this, SIGNAL(canRedoChanged(bool)), action, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(redoTextChanged(QString)), action, SLOT(setPrefixedText(QString)));
  connect(action, SIGNAL(triggered()), this, SLOT(redo()));
  
  action->setIcon(Utils::icon("edit-redo"));
  action->setIconText(tr("&Redo"));
  action->setShortcut(QKeySequence::Redo);
  
  return action;
}

QAction* Editor::createUndoAction()
{
  UndoAction *action = new UndoAction(tr("Undo"));
  
  action->setEnabled(canUndo());
  action->setPrefixedText(undoText());
  
  connect(this, SIGNAL(canUndoChanged(bool)), action, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(undoTextChanged(QString)), action, SLOT(setPrefixedText(QString)));
  connect(action, SIGNAL(triggered()), this, SLOT(undo()));
  
  action->setIcon(Utils::icon("edit-undo"));
  action->setIconText(tr("&Undo"));
  action->setShortcut(QKeySequence::Undo);
  
  return action;
}

float Editor::snap(float v) const
{
  float mod = std::fmod(std::fabs(v), gridDensity());
  
  if (v >= 0.0f)
    return v - mod;
  else
    return v + mod;
}

float Editor::snapYAxis(float v) const
{
  float mod = std::fmod(std::fabs(v), gridDensityYAxis());
  
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

QAction* Editor::getColor(const ldraw::color &color) const
{
  QAction *action = new QAction(QIcon(ColorManager::colorPixmap(color)), QString(color.get_entity()->name.c_str()), 0L);
  action->setData(color.get_id());

  return action;
}

QList<QAction *> Editor::getFavoriteColors() const
{
  ColorManager *cm = Application::self()->colorManager();

  QList<QAction *> colorList;
  foreach (const ldraw::color &it, cm->colorList()) {
    colorList.append(getColor(it));
  }

  return colorList;
}

QList<QAction *> Editor::getRecentlyUsedColors() const
{
  ColorManager *cm = Application::self()->colorManager();

  QList<QAction *> colorList;
  
  typedef QPair<ldraw::color, int> RecentlyUsedItem;
  foreach (const RecentlyUsedItem &it, cm->recentlyUsed()) {
    colorList.append(getColor(it.first));
  }

  return colorList;
}

void Editor::setRotationPivotMode(RotationPivot pivot)
{
  pivot_ = pivot;
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

void Editor::cut()
{
  if (!activeStack() || selection_->empty())
    return;
  
  copy();
  deleteSelected();
}

void Editor::copy()
{
  if (!activeStack() || selection_->empty())
    return;
  
  ObjectList ol(*selection_, model_);
  
  QClipboard *clipboard = qApp->clipboard();
  clipboard->setMimeData(ol.mimeData(), QClipboard::Clipboard);
}

void Editor::paste()
{
  if (!activeStack())
    return;
  
  QClipboard *clipboard = qApp->clipboard();
  const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
  
  if (!mimeData)
    return;
  else if (!mimeData->hasFormat(ObjectList::mimeType))
    return;
  
  ObjectList list = ObjectList::deserialize(mimeData->data(ObjectList::mimeType));
  
  activeStack()->push(new CommandPaste(list, *selection_, model_));
  
  emit modified();
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
  
  QMenu menu(tr("Select Color"));
  ColorManager *cm = Application::self()->colorManager();
  
  foreach (QAction *a, getFavoriteColors())
    menu.addAction(a);
  menu.addSeparator();
  foreach (QAction *a, getRecentlyUsedColors())
    menu.addAction(a);
  menu.addSeparator();
  QAction *customize = menu.addAction(tr("&More..."));
  
  QAction *result = menu.exec(QCursor::pos());
  if (result) {
    if (result == customize) {
      ColorDialog *colordialog = new ColorDialog(Application::self()->rootWindow());
      
      if (colordialog->exec() == QDialog::Accepted) {
        activeStack()->push(new CommandColor(colordialog->getSelected(), *selection_, model_));
        cm->hit(colordialog->getSelected());
	
        emit modified();

        if (colordialog->isChanged())
          emit colorListChanged();
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

void Editor::setColor(const ldraw::color &c)
{
  if (!activeStack() || selection_->empty())
    return;

  ColorManager *cm = Application::self()->colorManager();

  activeStack()->push(new CommandColor(c, *selection_, model_));
  cm->hit(c);

  emit modified();
}

#if 0
void Editor::rotationPivot()
{
  QMenu menu(tr("Rotation Pivot"));
  
  QAction *a1 = menu.addAction(tr("Each parts"));
  QAction *a2 = menu.addAction(tr("Center"));
  QAction *a3 = menu.addAction(tr("Manual (if exists)"));
  
  a1->setCheckable(true);
  a2->setCheckable(true);
  a3->setCheckable(true);
  
  QActionGroup ag(&menu);
  ag.addAction(a1);
  ag.addAction(a2);
  ag.addAction(a3);
  ag.setExclusive(true);
  
  if (pivot_ == PivotEach)
    a1->setChecked(true);
  else if (pivot_ == PivotCenter)
    a2->setChecked(true);
  else if (pivot_ == PivotManual)
    a3->setChecked(true);
  
  a3->setEnabled(false);
  
  QAction *result = menu.exec(QCursor::pos());
  if (result == a1)
    pivot_ = PivotEach;
  else if (result == a2)
    pivot_ = PivotCenter;
  else if (result == a3)
    pivot_ = PivotManual;
}
#endif

void Editor::move(const ldraw::vector &vector)
{
  if (!activeStack() || selection_->empty())
    return;
  
  ldraw::matrix m;
  m.set_translation_vector(vector);
  
  activeStack()->push(new CommandTransform(m, ldraw::matrix(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByXPositive()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisX, pivot_, gridDensity(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByXNegative()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisX, pivot_, -gridDensity(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByYPositive()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisY, pivot_, gridDensityYAxis(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByYNegative()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisY, pivot_, -gridDensityYAxis(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByZPositive()
{	
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisZ, pivot_, gridDensity(), *selection_, model_));
  
  emit modified();
}

void Editor::moveByZNegative()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Position, AxisZ, pivot_, -gridDensity(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByXClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisX, pivot_, gridDensityAngle(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByXCounterClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisX, pivot_, -gridDensityAngle(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByYClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisY, pivot_, gridDensityAngle(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByYCounterClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisY, pivot_, -gridDensityAngle(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByZClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisZ, pivot_, gridDensityAngle(), *selection_, model_));
  
  emit modified();
}

void Editor::rotateByZCounterClockwise()
{
  if (!activeStack() || selection_->empty())
    return;
  
  activeStack()->push(new CommandTransformLinear(CommandTransformLinear::Rotation, AxisZ, pivot_, -gridDensityAngle(), *selection_, model_));
  
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
  if (activeStack_ != activeStack() || index > activeStack()->count()) {
    return;
  }
  
  int s, e;
  bool redo;
  if (lastIndex_ < index) {
    // Redo
    redo = true;
    s = lastIndex_ + 1;
    e = index;
  } else if (lastIndex_ > index) {
    // Undo
    redo = false;
    s = index + 1;
    e = lastIndex_;
  } else {
    // Merge
    redo = true;
    s = index;
    e = index;
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
