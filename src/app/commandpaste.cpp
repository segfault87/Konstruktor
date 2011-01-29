// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <klocalizedstring.h>

#include <libldr/model.h>
#include <libldr/part_library.h>

#include "application.h"
#include "utils.h"

#include "commandpaste.h"

namespace Konstruktor
{

CommandPaste::CommandPaste(const ObjectList &list, const QSet<int> &selection, ldraw::model *model)
	: CommandBase(selection, model)
{
	list_ = list;

	if (selection.size() == 0)
		offset_ = -1;
	else {
		offset_ = Utils::maximum(selection);
		if (offset_ == (int)model_->elements().size() - 1)
			offset_ = -1;
		else
			offset_ += 1;
	}

	if (offset_ >= (int)model_->elements().size())
		offset_ = -1;

	setText(i18n("Paste"));
}

CommandPaste::~CommandPaste()
{

}

bool CommandPaste::needUpdateDimension() const
{
	return true;
}

CommandPaste::AffectedRowInfo CommandPaste::affectedRows() const
{
	QSet<int> set;

	int start;

	if (offset_ != -1)
		start = offset_;
	else
		start = model_->elements().size() - list_.length();

	for (int i = 0; i < list_.length(); ++i)
		set.insert(start + i);

	return AffectedRowInfo(Inserted, set);
}

void CommandPaste::redo()
{
	int o = offset_;

	QList<ldraw::element_base *> list = list_.elements();

	foreach (ldraw::element_base *elem, list) {
		printf("offset %d\n", o);
		
		model_->insert_element(elem, o);

		if (elem->get_type() == ldraw::type_ref) {
			ldraw::element_ref *ref = CAST_AS_REF(elem);

			if (!ref->get_model())
				Application::self()->library()->link_element(CAST_AS_REF(elem));
		}

		if (offset_ != -1)
			++o;
	}
}

void CommandPaste::undo()
{
	int o = offset_;

	for (int i = 0; i < list_.length(); ++i)
		model_->delete_element(o);
}

}
