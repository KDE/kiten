/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kmainwindow.h>
#include <qsqlpropertymap.h>

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
	  "1.2", I18N_NOOP("Japanese Reference Tool"), KAboutData::License_GPL,
	  "(c) 2001-2004, Jason Katz-Brown", 0, "http://www.katzbrown.com/kiten");

	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("Original author"), "jason@katzbrown.com");
	aboutData.addCredit("Jim Breen", I18N_NOOP("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\nAlso is main author of edict and kanjidic, which Kiten essentially require."), "jwb@csse.monash.edu.au");
	aboutData.addAuthor("Neil Stevens", I18N_NOOP("Code simplification, UI suggestions."), "neil@qualityassistant.com");
	aboutData.addCredit("David Vignoni", I18N_NOOP("svg icon"), "david80v@tin.it");
	aboutData.addCredit("Paul Temple", I18N_NOOP("Porting to KConfig XT, bug fixing"), "paul.temple@gmx.net");
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

	KApplication a;
//	a.installKDEPropertyMap();
//	QSqlPropertyMap *map = QSqlPropertyMap::defaultMap();
//	map->insert("MyStringList", "value");

	TopLevel *t = new TopLevel();
	t->show();
	return a.exec();
}
