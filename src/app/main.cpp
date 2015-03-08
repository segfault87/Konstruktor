// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>

#include "application.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // register metatype for following standard types
  qRegisterMetaType<std::string>("std::string");

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName(QObject::tr("Konstruktor"));
  QCoreApplication::setApplicationVersion(VERSION);
  app.setWindowIcon(QIcon(":/images/logo.png"));

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  
  QCommandLineOption rescanOption(QStringList() << "r" << "rescan",
                                  QObject::tr("Force rescan part database"));
  parser.addOption(rescanOption);

  parser.process(app);

  Konstruktor::Application mainapp;
  if (parser.isSet(rescanOption))
    mainapp.setForceRescan(true);

  if (!mainapp.initialize()) {
    app.exit();
    return -1;
  }

  mainapp.startup();
  
  return app.exec();
}
