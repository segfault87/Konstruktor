// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _UTILS_H_
#define _UTILS_H_

#include <QString>

class KonstruktorUtils
{
  private:
	KonstruktorUtils() {}
	~KonstruktorUtils() {}
	
  public:
	static float floatModulo(float dividend, float divisor);
};

#endif
