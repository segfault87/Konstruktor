// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include "commandtransformlinear.h"

namespace Konstruktor
{


ldraw::matrix CommandTransformLinear::getPositionMatrix(Editor::Axis axis, float delta)
{
	ldraw::matrix m;
	ldraw::vector v;

	if (axis == Editor::AxisX)
		v = ldraw::vector(delta, 0.0f, 0.0f);
	else if (axis == Editor::AxisY)
		v = ldraw::vector(0.0f, delta, 0.0f);
	else if (axis == Editor::AxisZ)
		v = ldraw::vector(0.0f, 0.0f, delta);

	m.set_translation_vector(v);

	return m;
}

ldraw::matrix CommandTransformLinear::getRotationMatrix(Editor::Axis axis, float delta)
{
	ldraw::matrix m;

	if (axis == Editor::AxisX) {
		m.value(1, 1) = std::cos(delta);
		m.value(1, 2) = std::sin(-delta);
		m.value(2, 1) = std::sin(delta);
		m.value(2, 2) = std::cos(delta);
	} else if (axis == Editor::AxisY) {
		m.value(0, 0) = std::cos(delta);
		m.value(2, 0) = std::sin(-delta);
		m.value(0, 2) = std::sin(delta);
		m.value(2, 2) = std::cos(delta);
	} else if (axis == Editor::AxisZ) {
		m.value(0, 0) = std::cos(delta);
		m.value(0, 1) = std::sin(-delta);
		m.value(1, 0) = std::sin(delta);
		m.value(1, 1) = std::cos(delta);
	}

	return m;
}

CommandTransformLinear::CommandTransformLinear(TransformType type, Editor::Axis axis, Editor::RotationPivot pivot, float delta, const QSet<int> &selection, ldraw::model *model)
	: CommandTransform(selection, model, pivot)
{
	axis_ = axis;
	type_ = type;
	delta_ = delta;

	/* no use for translating 'position' */
	if (type == Position)
		pivot_ = Editor::PivotEach;
	
	if (type == Rotation)
		postmult_ = getRotationMatrix(axis_, delta);
	else
		premult_ = getPositionMatrix(axis_, delta);
}

CommandTransformLinear::~CommandTransformLinear()
{

}

float CommandTransformLinear::delta() const
{
	return delta_;
}

CommandTransformLinear::TransformType CommandTransformLinear::transformType() const
{
	return type_;
}

Editor::Axis CommandTransformLinear::axis() const
{
	return axis_;
}

int CommandTransformLinear::id() const
{
	int axis;
	int type;
	int pivot;

	if (axis_ == Editor::AxisX)
		axis = 0;
	else if (axis_ == Editor::AxisY)
		axis = 1;
	else if (axis_ == Editor::AxisZ)
		axis = 2;
	else
		return -1;

	if (type_ == Position)
		type = 0;
	else if (type_ == Rotation)
		type = 1;
	else
		return -1;

	if (pivot_ == Editor::PivotEach)
		pivot = 0;
	else if (pivot_ == Editor::PivotCenter)
		pivot = 1;
	else if (pivot_ == Editor::PivotManual)
		pivot = 2;
	else
		return -1;

	return COMMAND_ID_TRANSFORM_LINEAR << 5 | pivot << 3 | axis << 1 | type;
}

bool CommandTransformLinear::mergeWith(const QUndoCommand *command)
{
	if (command->id() != id())
		return false;

	const CommandTransformLinear *ctl = dynamic_cast<const CommandTransformLinear *>(command);

	if (selection_ != ctl->selection())
		return false;

	delta_ += ctl->delta();
	
	if (type_ == Position)
		premult_ = getPositionMatrix(axis_, delta_);
	else
		postmult_ = getRotationMatrix(axis_, delta_);

	return true;
}

}



