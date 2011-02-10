// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "commandbase.h"

namespace Konstruktor
{

CommandSelectionFilter::CommandSelectionFilter(const CommandBase *cmd)
	: model_(cmd->model()), selection_(cmd->selection())
{
}

bool CommandSelectionFilter::query(const ldraw::model *m, int index, int depth) const
{
	if (depth != 0 || model_ != m)
		return true;

	return selection_.contains(index);
}

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

CommandBase::AffectedRowInfo CommandBase::affectedRows() const
{
	return AffectedRowInfo(Inserted, QSet<int>());
}

}
