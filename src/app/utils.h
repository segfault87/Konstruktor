// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _UTILS_H_
#define _UTILS_H_

#include <QSet>
#include <QString>
#include <QUrl>

namespace Konstruktor
{

class Utils
{
  private:
	Utils() {}
	~Utils() {}
	
  public:
	static int maximum(const QSet<int> &set);
	static QString urlFileName(const QUrl &url);
};

}

#endif
