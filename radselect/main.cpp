/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
  app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kiten"), app.windowIcon()));
  KLocalizedString::setApplicationDomain("kiten");
  
  KAboutData about( QStringLiteral("kitenradselect"), i18n("kitenradselect"), version,
                    i18n(description), KAboutLicense::GPL,
                    i18n("(C) 2005 Joseph Kerian"), QString(), QString(), QStringLiteral("jkerian@gmail.com") );
  about.addAuthor( i18n("Joseph Kerian"), QString(), QStringLiteral("jkerian@gmail.com") );
  about.addCredit( i18n("Electronic Dictionary Research and Development Group"), i18n("This program uses the KANJIDIC and RADKFILE dictionary files. These files are the property of the Electronic Dictionary Research and Development Group, and are used in conformance with the Group's licence."), QString(), QStringLiteral("https://www.edrdg.org/") );
  about.setOrganizationDomain("kde.org"); //For DBus domain

  QCommandLineParser parser;
  KAboutData::setApplicationData(about);
  
  about.setupCommandLine(&parser);
  parser.addPositionalArgument(QStringLiteral("Search_String"), i18n( "Initial Search String from Kiten" ));
  parser.process(app);
  about.processCommandLine(&parser);

  // see if we are starting with session management
  if ( app.isSessionRestored() )
  {
    kRestoreMainWindows<RadSelect>();
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
