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
	  "0.5", "Japanese Reference Tool", KAboutData::License_GPL,
	  "(c) 2001, Jason Katz-Brown", 0, "http://www.katzbrown.com/kiten", "jason@katzbrown.com");
	aboutData.addAuthor("Jason Katz-Brown",0, "jason@katzbrown.com");
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

	KApplication a;
	TopLevel *t = new TopLevel();
	t->show();
	return a.exec();
}
