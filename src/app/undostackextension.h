// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _UNDOSTACKEXTENSION_H_
#define _UNDOSTACKEXTENSION_H_

#include <QUndoStack>

#include <libldr/extension.h>

namespace Konstruktor
{

class UndoStackExtension : public ldraw::extension, public QUndoStack
{
  public:
	UndoStackExtension(ldraw::model *m, void *arg = 0L);

	static const std::string identifier() { return "undostack"; }
};	

}

#endif

