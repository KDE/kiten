/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KUniqueApplication>

#include "kanjibrowser.h"

static const char description[] = I18N_NOOP( "Kiten's Kanji Browser, a KDE Japanese Reference Tool" );
static const char version[] = "1.0";

int main( int argc, char **argv )
{
  KAboutData about(   "kitenkanjibrowser"
                    , "kiten"
                    , ki18n( "kitenkanjibrowser" )
                    , version
                    , ki18n( description )
                    , KAboutData::License_GPL_V2
                    , ki18n( "(C) 2011 Daniel E. Moctezuma" )
                    , KLocalizedString()
                    , "http://edu.kde.org/kiten"
                    , "democtezuma@gmail.com" );
  about.addAuthor(   ki18n( "Daniel E. Moctezuma" )
                   , KLocalizedString()
                   , "democtezuma@gmail.com" );
  about.setOrganizationDomain( "kde.org" );
  about.setProgramIconName( "kiten" );

  KCmdLineArgs::init( argc, argv, &about );

  KUniqueApplication app;
  if( app.isSessionRestored() )
  {
    RESTORE( KanjiBrowser );
  }
  else
  {
    KanjiBrowser *kanjiBrowser = new KanjiBrowser();
    kanjiBrowser->show();
  }

  return app.exec();
}
