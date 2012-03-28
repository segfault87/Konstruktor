// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

namespace Konstruktor
{

struct Viewport
{
  float left;
  float top;
  float right;
  float bottom;
  float aspectRatio;
};

extern const Viewport defaultViewport;

}

#endif
