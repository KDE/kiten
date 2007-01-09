/* This file is part of kiten, a KDE Japanese Reference Tool
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "radselect.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <kmessagebox.h>

static const char description[] = I18N_NOOP("A KDE Application");
static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
	{ "+[Search_String]", I18N_NOOP( "Initial Search String from Kiten" ), 0 },
	KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("kitenradselect", I18N_NOOP("kitenradselect"), version,
			description, KAboutData::License_GPL,
			"(C) 2005 Joseph Kerian", 0, 0, "jkerian@gmail.com");
	about.addAuthor( "Joseph Kerian", 0, "jkerian@gmail.com" );
	about.setOrganizationDomain("kde.org"); //For DBus domain

	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

   // see if we are starting with session management
	if (app.isSessionRestored()) {
		RESTORE(radselect);
	} else {
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

		radselect *widget = new radselect;
		widget->show();

		if (args->count() >= 1)
			widget->loadSearchString(args->arg(0));
		args->clear();
	}

	return app.exec();
}