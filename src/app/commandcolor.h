// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COMMANDCOLOR_H_
#define _COMMANDCOLOR_H_

#include <QMap>

#include <libldr/color.h>

#include "commandbase.h"

namespace Konstruktor
{

class CommandColor : public CommandBase
{
  public:
	CommandColor(const ldraw::color &color, const QSet<int> &selection, ldraw::model *model);
	~CommandColor();

	void redo();
	void undo();

  private:
	ldraw::color color_;
	std::map<int, ldraw::color> oldcolors_;
};

}

#endif
