// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _COMMANDTRANSFORMLINEAR_H_
#define _COMMANDTRANSFORMLINEAR_H_

#include <libldr/math.h>

#include "editor.h"

#include "commandtransform.h"

#define COMMAND_ID_TRANSFORM_LINEAR 1

namespace Konstruktor
{

class CommandTransformLinear : public CommandTransform
{
  public:
	static ldraw::matrix getPositionMatrix(Editor::Axis axis, float delta);
	static ldraw::matrix getRotationMatrix(Editor::Axis axis, float delta);

	enum TransformType { Position, Rotation };

	CommandTransformLinear(TransformType type, Editor::Axis axis, Editor::RotationPivot pivot, float delta, const QSet<int> &selection, ldraw::model *model);
	~CommandTransformLinear();

	float delta() const;
	TransformType transformType() const;
	Editor::Axis axis() const;

	/* reimplement */
	int id() const;
	bool mergeWith(const QUndoCommand *command);

  private:
	Editor::Axis axis_;
	TransformType type_;
	float delta_;
};

}

#endif
