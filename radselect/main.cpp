/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#include <QApplication>
#include <QIcon>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>

#include "radselect.h"

static const char description[] = I18N_NOOP( "A KDE Application" );
static const char version[] = "0.1";

int main( int argc, char **argv )
{
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon::fromTheme(QLatin1String("kiten"), app.windowIcon()));
  KLocalizedString::setApplicationDomain("kiten");
  
  KAboutData about( QStringLiteral("kitenradselect"), i18n("kitenradselect"), QStringLiteral("0.1"),
                    i18n(description), KAboutLicense::GPL,
                    i18n("(C) 2005 Joseph Kerian"), QString(), QString(), "jkerian@gmail.com" );
  about.addAuthor( i18n("Joseph Kerian"), QString(), "jkerian@gmail.com" );
  about.setOrganizationDomain("kde.org"); //For DBus domain

  QCommandLineParser parser;
  KAboutData::setApplicationData(about);
  
  parser.addVersionOption();
  parser.addHelpOption();
  about.setupCommandLine(&parser);
  parser.addPositionalArgument("Search_String", i18n( "Initial Search String from Kiten" ));
  parser.process(app);
  about.processCommandLine(&parser);

  // see if we are starting with session management
  if ( app.isSessionRestored() )
  {
    RESTORE( RadSelect );
  }
  else
  {
    // no session.. just start up normally

    RadSelect *widget = new RadSelect();
    widget->show();

    if ( parser.positionalArguments().count() >=  1 )
    {
      const QStringList args = parser.positionalArguments();
      widget->loadSearchString( args.first() );
    }

    
  }

  return app.exec();
}
