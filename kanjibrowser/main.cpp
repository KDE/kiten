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

#include <QApplication>

#include <KAboutData>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include "kanjibrowser.h"

static const char description[] = I18N_NOOP( "Kiten's Kanji Browser, a KDE Japanese Reference Tool" );
static const char version[] = "1.0";

int main( int argc, char **argv )
{
  Kdelibs4ConfigMigrator migrate(QLatin1String("kitenkanjibrowser"));
  migrate.setConfigFiles(QStringList() << QLatin1String("kitenkanjibrowserrc"));
  migrate.setUiFiles(QStringList() << QLatin1String("kanjibrowserui.rc"));
  migrate.migrate();

  QApplication app(argc, argv);
  app.setWindowIcon(QIcon::fromTheme(QLatin1String("kiten"), app.windowIcon()));
  KLocalizedString::setApplicationDomain("kiten");

  KAboutData about( QStringLiteral("kitenkanjibrowser")
                    , i18n( "kitenkanjibrowser" )
                    , QStringLiteral("1.0")
                    , i18n( description )
                    , KAboutLicense::GPL_V2
                    , i18n( "(C) 2011 Daniel E. Moctezuma" )
                    , QString()
                    , QStringLiteral("http://edu.kde.org/kiten")
                    , "democtezuma@gmail.com" );
  about.addAuthor(   i18n( "Daniel E. Moctezuma" )
                   , QString()
                   , "democtezuma@gmail.com" );
  about.setOrganizationDomain( "kde.org" );

  KAboutData::setApplicationData(about);
  
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
