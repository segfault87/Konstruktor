// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "application.h"

static const char description[] = "An interactive LDraw model designer for KDE 4";
static const char version[] = "0.5.0-beta1";

int main(int argc, char *argv[])
{
	KAboutData about(
		"konstruktor", 0, ki18nc("@title", "Konstruktor"), version, ki18nc("@title", description),
		KAboutData::License_GPL_V3, ki18nc("@info:credit", "(c) 2008, Park \"segfault\" Joon-Kyu")
	);
	about.addAuthor(ki18nc("@info:credit", "Park \"segfault\" Joon-Kyu"), ki18nc("@info:credit", "Main developer"),
					"mastermind@planetmono.org", "http://planetmono.org");
	about.addCredit(ki18nc("@info:credit", "Lee Olson"), ki18nc("@info:credit", "Application icon"),
					"leetolson@gmail.com");
	
	KCmdLineArgs::init(argc, argv, &about);

	KCmdLineOptions options;
	options.add("+[Url]", ki18nc("@info:shell", "Document to open"));
	KCmdLineArgs::addCmdLineOptions(options);
	
	KApplication app;
	
	KonstruktorApplication mainapp;
	
	return app.exec();
}
