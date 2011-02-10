// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COMMANDTRANSFORM_H_
#define _COMMANDTRANSFORM_H_

#include <QMap>

#include <libldr/math.h>

#include "commandbase.h"
#include "editor.h"

namespace Konstruktor
{

class CommandTransform : public CommandBase
{
  public:
	CommandTransform(const ldraw::matrix &premult, const ldraw::matrix &postmult, const QSet<int> &selection, ldraw::model *model, Editor::RotationPivot pivot = Editor::PivotEach);
	CommandTransform(const QSet<int> &selection, ldraw::model *model, Editor::RotationPivot pivot = Editor::PivotEach);
	virtual ~CommandTransform();

	bool needUpdateDimension() const;
	
	void redo();
	void undo();

  protected:
	Editor::RotationPivot pivot_;
	ldraw::matrix premult_;
	ldraw::matrix postmult_;
	ldraw::vector pivotpoint_;
	std::map<int, ldraw::matrix> oldmatrices_;
	bool inverse_;
};

}

#endif
