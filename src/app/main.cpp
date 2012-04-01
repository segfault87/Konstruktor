// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QApplication>
#include <QIcon>

#include "application.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName(QObject::tr("Konstruktor"));
  QCoreApplication::setApplicationVersion(VERSION);
  app.setWindowIcon(QIcon(":/images/logo.png"));
  
  Konstruktor::Application mainapp;
  
  return app.exec();
}
