//jason@katzbrown.com

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <klocale.h>
#include <qnamespace.h>
#include <kmainwindow.h>

#include "kiten.h"

void noMessageOutput(QtMsgType, const char *)
{
}

static KCmdLineOptions options[] =
{
	KCmdLineLastOption
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	qInstallMsgHandler(noMessageOutput);
	KAboutData aboutData( "kiten", I18N_NOOP("Kiten"),
	  "1.1", I18N_NOOP("Japanese Reference Tool"), KAboutData::License_GPL,
	  "(c) 2001, Jason Katz-Brown", 0, "http://www.katzbrown.com/kiten");

	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("Original author"), "jason@katzbrown.com");
	aboutData.addCredit("Jim Breen", I18N_NOOP("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\nAlso is main author of edict and kanjidic, which Kiten essentially require."), "jwb@csse.monash.edu.au");
	aboutData.addAuthor("Neil Stevens", I18N_NOOP("Code simplification, UI suggestions."), "neil@qualityassistant.com");
	aboutData.addCredit("David Vignoni", I18N_NOOP("svg icon"), "david80v@tin.it");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

	KApplication a;
	TopLevel *t = new TopLevel();
	t->show();
	return a.exec();
}
