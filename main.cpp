//jason@katzbrown.com

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>
#include <klocale.h>
#include <qnamespace.h>
#include <kmainwindow.h>

#include "kiten.h"

static const char *description =
	I18N_NOOP("Japanese Reference Tool");
	
static KCmdLineOptions options[] =
{
	{ 0, 0, 0 }
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	KAboutData aboutData( "Kiten", I18N_NOOP("Kiten"),
	  "0.1", I18N_NOOP("Japanese Reference Tool"), KAboutData::License_GPL,
	  "(c) 2001, Jason Katz-Brown", 0, "http://www.katzbrown.com/kiten", "jason@katzbrown.com");

	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("Original author"), "jason@katzbrown.com");
	aboutData.addCredit("Jim Breen", I18N_NOOP("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\nAlso is main author of edict and kanjidic, which Kiten essentially require."), "jwb@csse.monash.edu.au");
	aboutData.addCredit("Neil Stevens", I18N_NOOP("Encoding help, and miscellaneous fixes."), "neil@qualityassistant.com");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

	KApplication a;
	TopLevel *t = new TopLevel();
	t->show();
	return a.exec();
}
