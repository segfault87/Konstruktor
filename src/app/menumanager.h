// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _MENUMANAGER_H_
#define _MENUMANAGER_H_

#include <QObject>

namespace Konstruktor
{

class MenuManager : public QObject
{
 public:
  MenuManager(QObject *parent = 0L);
  ~MenuManager();
};

}

#endif
