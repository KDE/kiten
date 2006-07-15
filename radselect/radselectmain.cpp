/***************************************************************************
 *   Copyright (C) 2005 by Joseph Kerian   *
 *   jkerian@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "radselect.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <kmessagebox.h>

static const char description[] =
    I18N_NOOP("A KDE Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
    { "+[Search_String]", I18N_NOOP( "Initial Search String from Kiten" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("radselect", I18N_NOOP("radselect"), version, description,
    	KAboutData::License_GPL, "(C) 2005 Joseph Kerian", 0, 0, "jkerian@gmail.com");
	about.addAuthor( "Joseph Kerian", 0, "jkerian@gmail.com" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication app;

   // see if we are starting with session management
   if (app.isSessionRestored())
   {
       RESTORE(radselect);
   }
   else
   {
       // no session.. just start up normally
       KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

       radselect *widget = new radselect;
       widget->show();

       if (args->count() != 0)
       { //We're being passed a search string on the command line
           int i = 0;
           for (; i < args->count(); i++)
           { //Really... there should be only one argument here...
               widget->loadSearchString(args->arg(i));
           }
       }
       args->clear();
   }

   return app.exec();
}
