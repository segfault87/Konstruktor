// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QApplication>

#include "application.h"

static const char description[] = "An interactive LDraw model designer";
static const char version[] = "0.9.0-beta1";

int main(int argc, char *argv[])
{
  /*KAboutData about(
      "konstruktor", 0, ki18nc("@title", "Konstruktor"), version, ki18nc("@title", description),
      KAboutData::License_GPL_V3, ki18nc("@info:credit", "(c) 2011, Park \"segfault\" Joon-Kyu")
                   );
  about.setHomepage("http://konstruktor.influx.kr");
  about.addAuthor(ki18nc("@info:credit", "Park \"segfault\" Joon-Kyu"),
                  ki18nc("@info:credit", "Main developer"),
                  "mastermind@planetmono.org", "http://planetmono.org");
  about.addCredit(ki18nc("@info:credit", "Lee Olson"),
                  ki18nc("@info:credit", "Application icon"),
                  "leetolson@gmail.com");
  about.addCredit(ki18nc("@info:credit", "Nathaniel Titane"),
                  ki18nc("@info:credit", "Testing"),
                  "nathaniel.titane@gmail.com");
  about.addCredit(ki18nc("@info:credit", "Ralf Habacker"),
                  ki18nc("@info:credit", "Windows port"),
                  "ralf.habacker@freenet.de");*/
  
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName("Konstruktor");
  
  Konstruktor::Application mainapp;
  
  return app.exec();
}
