// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <iostream>
#include <stdexcept>
#include <string>

#include <QApplication>
#include <QMessageBox>

#include "dbupdater.h"

void usage(const char *progname)
{
  std::cerr << "Usage: " << progname << " [-rescan] path-to-ldraw" << std::endl;
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QStringList args = app.arguments();
  std::string path;
  bool rescan;

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName("Konstruktor");

  for (int i = 1; i < args.count(); ++i) {
    const QString &arg = args[i];

    if (arg == "-rescan") {
      rescan = true;
    } else if (arg.startsWith("-")) {
      std::cerr << "Unrecognized option: " << arg.toLocal8Bit().data() << std::endl;
      usage(argv[0]);
      return 1;
    } else {
      path = arg.toLocal8Bit().data();
    }
  }

  if (path.empty()) {
    usage(argv[0]);
    return 1;
  }
  
  int status;
  try {
    Konstruktor::DBUpdater updater(path, rescan);
    
    status = updater.start();
  } catch (const std::runtime_error &e) {
    QMessageBox::critical(0L, QObject::tr("Error"), QString(e.what()));
    
    status = -1;
  }
  
  app.exit(status);
  
  return status;
}
