// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COMMANDPASTE_H_
#define _COMMANDPASTE_H_

#include "commandbase.h"
#include "objectlist.h"

namespace Konstruktor
{

class CommandPaste : public CommandBase
{
  public:
	CommandPaste(const ObjectList &list, const QSet<int> &selection, ldraw::model *model);
	~CommandPaste();

	bool needUpdateDimension() const;
	AffectedRowInfo affectedRows() const;

	void redo();
	void undo();

  private:
	ObjectList list_;
	int offset_;
};

}

#endif
