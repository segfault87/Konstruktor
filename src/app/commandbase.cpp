// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "commandbase.h"

KonstruktorCommandBase::KonstruktorCommandBase(const QSet<int> &selection, ldraw::model *model)
	: QUndoCommand(0L)
{
	selection_ = selection;
	model_ = model;
}

KonstruktorCommandBase::~KonstruktorCommandBase()
{
	
}

bool KonstruktorCommandBase::needUpdateDimension() const
{
	return false;
}

bool KonstruktorCommandBase::needRepaint() const
{
	return true;
}

QPair<KonstruktorCommandBase::AffectedRow, QSet<int> > KonstruktorCommandBase::affectedRows() const
{
	return QPair<KonstruktorCommandBase::AffectedRow, QSet<int> >(Inserted, QSet<int>());
}
