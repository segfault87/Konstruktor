// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "commandbase.h"

namespace Konstruktor
{

CommandBase::CommandBase(const QSet<int> &selection, ldraw::model *model)
	: QUndoCommand(0L)
{
	selection_ = selection;
	model_ = model;
}

CommandBase::~CommandBase()
{
	
}

bool CommandBase::needUpdateDimension() const
{
	return false;
}

bool CommandBase::needRepaint() const
{
	return true;
}

QPair<CommandBase::AffectedRow, QSet<int> > CommandBase::affectedRows() const
{
	return QPair<CommandBase::AffectedRow, QSet<int> >(Inserted, QSet<int>());
}

}
