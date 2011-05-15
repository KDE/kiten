/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#include <KAboutData>
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <KXmlGuiWindow>

#include <QtDBus>

#include "kiten.h"

void noMessageOutput( QtMsgType, const char* )
{
}

int main( int argc, char *argv[] )
{
//   qInstallMsgHandler(noMessageOutput);	//Disable Qt Errors from showing

  // Copyright and author information
  KAboutData aboutData( "kiten", 0, ki18n("Kiten"), "1.2", ki18n("Japanese Reference Tool"),
                  KAboutData::License_GPL_V2, ki18n("(c) 2001-2004, Jason Katz-Brown"),
                  KLocalizedString(), "http://edu.kde.org/kiten" );

  aboutData.addAuthor( ki18n("Jason Katz-Brown"), ki18n("Original author"), "jason@katzbrown.com" );
  aboutData.addCredit( ki18n("Jim Breen"),        ki18n("Wrote xjdic, of which Kiten borrows code, and the xjdic index file generator.\nAlso is main author of edict and kanjidic, which Kiten essentially require."), "jwb@csse.monash.edu.au" );
  aboutData.addAuthor( ki18n("Neil Stevens"),     ki18n("Code simplification, UI suggestions."), "neil@qualityassistant.com" );
  aboutData.addCredit( ki18n("David Vignoni"),    ki18n("svg icon"), "david80v@tin.it");
  aboutData.addCredit( ki18n("Paul Temple"),      ki18n("Porting to KConfig XT, bug fixing"), "paul.temple@gmx.net" );
  aboutData.addAuthor( ki18n("Joseph Kerian"),    ki18n("KDE4 rewrite"), "jkerian@gmail.com" );

  aboutData.setOrganizationDomain( "kde.org" ); //Set this for the DBUS ID
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KUniqueApplication app;

  Kiten *kiten = new Kiten();

  //Register the DBUS name or die
  if ( ! QDBusConnection::sessionBus().registerService( "org.kde.kiten" ) )
  {
    exit( 1 );
  }

  kiten->show();
  QDBusConnection::sessionBus().registerObject( "/", kiten, QDBusConnection::ExportAllSlots );
  return app.exec();
}
