// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COMMANDREMOVE_H_
#define _COMMANDREMOVE_H_

#include <string>

#include <QList>
#include <QMap>

#include "commandbase.h"

namespace Konstruktor
{

class CommandRemove : public CommandBase
{
  public:
	CommandRemove(const QSet<int> &selection, ldraw::model *model);
	~CommandRemove();

	bool needUpdateDimension() const;
	QPair<CommandBase::AffectedRow, QSet<int> > affectedRows() const;

	void redo();
	void undo();

  private:
	QList<int> itemsToRemove_;
	QMap<int, std::string> objects_;
};

}

#endif
