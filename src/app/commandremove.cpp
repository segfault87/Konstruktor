// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <sstream>

#include <libldr/model.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <libldr/writer.h>

#include <QMap>

#include <klocalizedstring.h>

#include "application.h"

#include "commandremove.h"

namespace Konstruktor
{

CommandRemove::CommandRemove(const QSet<int> &selection, ldraw::model *model)
	: CommandBase(selection, model)
{
	if (selection.size() == 1)
		setText(i18n("Delete an Object"));
	else
		setText(i18n("Delete Objects"));

	// QSet is unordered; convert QSet to QMap
	QMap<int, int> temporary;
	for (QSet<int>::ConstIterator it = selection.begin(); it != selection.end(); ++it)
		temporary[*it] = *it;
	
	int i = 0;
	for (QMap<int, int>::ConstIterator it = temporary.begin(); it != temporary.end(); ++it) {
		itemsToRemove_.append(it.key() - i++);
		std::ostringstream stream;
		ldraw::writer writer(stream);
		writer.write(model->at(*it));
		objects_[*it] = stream.str();
	}
}

CommandRemove::~CommandRemove()
{

}

bool CommandRemove::needUpdateDimension() const
{
	return true;
}

QPair<CommandBase::AffectedRow, QSet<int> > CommandRemove::affectedRows() const
{
	return QPair<CommandBase::AffectedRow, QSet<int> >(Removed, selection_);
}

void CommandRemove::redo()
{
	for (QList<int>::Iterator it = itemsToRemove_.begin(); it != itemsToRemove_.end(); ++it)
		model_->delete_element(*it);
}

void CommandRemove::undo()
{
	for (QMap<int, std::string>::Iterator it = objects_.begin(); it != objects_.end(); ++it) {
		ldraw::element_base *elem = ldraw::reader::parse_line(*it, model_);
		model_->insert_element(elem, it.key());
		if (elem->get_type() == ldraw::type_ref) {
			ldraw::element_ref *ref = CAST_AS_REF(elem);
			if (!ref->get_model())
				Application::self()->library()->link_element(ref);
		}
	}
}

}
