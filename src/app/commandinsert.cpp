// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/elements.h>
#include <libldr/model.h>
#include <libldr/part_library.h>

#include <klocalizedstring.h>

#include "application.h"

#include "commandinsert.h"

KonstruktorCommandInsert::KonstruktorCommandInsert(const QString &filename, const ldraw::matrix &matrix, const ldraw::color &color, const QSet<int> &selection, ldraw::model *model)
	: KonstruktorCommandBase(selection, model)
{
	filename_ = filename;
	matrix_ = matrix;
	color_ = color;

	if (selection.size() != 1)
		offset_ = -1;
	else
		offset_ = *selection.begin() + 1;

	if (offset_ >= (int)model_->elements().size())
		offset_ = -1;

	setText(i18n("Insert %1", filename));
}

KonstruktorCommandInsert::~KonstruktorCommandInsert()
{

}

bool KonstruktorCommandInsert::needUpdateDimension() const
{
	return true;
}

QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > KonstruktorCommandInsert::affectedRows() const
{
	QSet<int> set;

	if (offset_ != -1)
		set.insert(offset_);
	else
		set.insert(model_->elements().size() - 1);

	return QPair<KonstruktorCommandBase::AffectedRow, QSet<int> >(Inserted, set);
}

void KonstruktorCommandInsert::redo()
{
	ldraw::element_ref *ref = new ldraw::element_ref(color_, matrix_, filename_.toLocal8Bit().data());
	model_->insert_element(ref, offset_);

	if (!ref->get_model())
		KonstruktorApplication::self()->library()->link_element(ref);
}

void KonstruktorCommandInsert::undo()
{
	model_->delete_element(offset_);
}

