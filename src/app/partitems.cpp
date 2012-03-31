// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "partitems.h"

namespace Konstruktor
{

PartCategory::PartCategory(const QString &name, int id, int visibility, int idx)
{
  name_ = name;
  id_ = id;
  visibility_ = visibility;
  index_ = idx;
}


PartItem::PartItem(PartCategory *parent, const QString &desc, const QString &filename, const ldraw::metrics &metrics)
    : RefObject(filename, metrics)
{
  parent_ = parent;
  desc_ = desc;
}

}
