// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "dbupdater.h"

int main(int argc, char *argv[])
{
	const KAboutData about(
		"konstruktor", "konstruktor",
		ki18n("Part database updater for Konstruktor"), "0.6.0",
		ki18n("Updates the part database. Must be called internally by Konstruktor."),
		KAboutData::License_GPL_V3,
		ki18n("(c)2006-2008, Park \"segfault\" Joon-Kyu"));
	
	KCmdLineArgs::init(argc, argv, &about);

	KCmdLineOptions options;
	options.add("+[location]", ki18n("Path to LDraw part library"));
	options.add("rescan", ki18n("Rescan the entire library"));
	KCmdLineArgs::addCmdLineOptions(options);
	
	KApplication app;

	KonstruktorDBUpdater updater;

	int status = updater.start();
	app.exit(status);

	return status;
}
