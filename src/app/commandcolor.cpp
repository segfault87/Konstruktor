// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <libldr/model.h>

#include "commandcolor.h"

namespace Konstruktor
{

CommandColor::CommandColor(const ldraw::color &color, const QSet<int> &selection, ldraw::model *model)
    : CommandBase(selection, model)
{
  setText(QObject::tr("Change Color to %1").arg(color.get_entity()->name.c_str()));
  
  color_ = color;
  
  for (QSet<int>::ConstIterator it = selection.constBegin(); it != selection.constEnd(); ++it) {
    if (model->elements()[*it]->capabilities() & ldraw::capability_color)
      oldcolors_[*it] = dynamic_cast<const ldraw::element_colored_base *>(model->elements()[*it])->get_color();
  }
}

CommandColor::~CommandColor()
{
  
}

void CommandColor::redo()
{
  for (QSet<int>::ConstIterator it = selection_.constBegin(); it != selection_.constEnd(); ++it) {
    if (model_->elements()[*it]->capabilities() & ldraw::capability_color)
      dynamic_cast<ldraw::element_colored_base *>(model_->elements()[*it])->set_color(color_);
  }
}

void CommandColor::undo()
{
  for (QSet<int>::ConstIterator it = selection_.constBegin(); it != selection_.constEnd(); ++it) {
    if (model_->elements()[*it]->capabilities() & ldraw::capability_color)
      dynamic_cast<ldraw::element_colored_base *>(model_->elements()[*it])->set_color(oldcolors_[*it]);
  }
}

}
