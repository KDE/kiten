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

#include "kanjibrowser.h"

#include "ui_preferences.h"

#include "DictKanjidic/dictfilekanjidic.h"
#include "kanjibrowserconfig.h"
#include "kanjibrowserview.h"
#include "kitenmacros.h"

#include <KActionCollection>
#include <KApplication>
#include <KConfigDialog>
#include <KStandardAction>
#include <KStandardDirs>
#include <KStatusBar>

KanjiBrowser::KanjiBrowser()
: KXmlGuiWindow()
, _dictFileKanjidic( 0 )
{
  // Read the configuration file.
  _config = KanjiBrowserConfigSkeleton::self();
  _config->readConfig();

  statusBar()->show();

  // Add some actions.
  KStandardAction::quit( kapp, SLOT( quit() ), actionCollection() );
  KStandardAction::preferences( this, SLOT( showPreferences() ), actionCollection() );

  _view = new KanjiBrowserView( this->parentWidget() );
  connect( _view, SIGNAL( statusBarChanged( const QString&) ),
            this,   SLOT( changeStatusBar( const QString& ) ) );
  // Load the necessary information and setup the view.
  loadKanji();

  setCentralWidget( _view );
  setObjectName( QLatin1String( "kanjibrowser" ) );

  setupGUI( Default, "kanjibrowserui.rc" );
}

KanjiBrowser::~KanjiBrowser()
{

}

void KanjiBrowser::changeStatusBar( const QString &text )
{
  statusBar()->showMessage( text );
}

void KanjiBrowser::loadKanji()
{
  if( _dictFileKanjidic )
  {
    return;
  }

  kDebug() << "Loading kanji..." << endl;

  QString dictionary = KGlobal::dirs()->findResource(   "data"
                                                      , "kiten/kanjidic" );
  _dictFileKanjidic = new DictFileKanjidic();
  _dictFileKanjidic->loadSettings();
  _dictFileKanjidic->loadDictionary( dictionary, KANJIDIC );

  // Parse the contents of KANJIDIC that we need to create the view.
  // We need:
  //  - Kanji
  //  - Grade
  //  - Number of strokes
  QRegExp gradeMatch( "^G\\d+" );
  QRegExp strokeMatch( "^S\\d+" );
  QList<int> gradeList;
  QList<int> strokeList;
  QHash< QString, QPair<int, int> > kanjiList;
  foreach( const QString &line, _dictFileKanjidic->dumpDictionary() )
  {
    // All the kanji without grade will have Grade 0, making easy to
    // manage that information in KanjiBrowserView.
    int grade   = 0;
    int strokes = 0;
    QStringList gradeSection = line.split( " ", QString::SkipEmptyParts )
                                   .filter( gradeMatch );
    QStringList strokesSection = line.split( " ", QString::SkipEmptyParts )
                                     .filter( strokeMatch );

    // There are some kanji without grade (example: those not in Jouyou list).
    if( ! gradeSection.isEmpty() )
    {
      // In KANJIDIC the grade/stroke section is: G# (for grade)
      //                                          S# (for strokes)
      // where # can be one or more digits.
      // We remove the first character and convert the remaining ones to int.
      grade = gradeSection.first().remove( 0, 1 ).toInt();
      gradeList << grade;
    }

    strokes = strokesSection.first().remove( 0, 1 ).toInt();
    strokeList << strokes;

    // Insert the extracted information into our QHash.
    kanjiList.insert( line[ 0 ], qMakePair( grade, strokes ) );
  }

  // This convertion from QList to QSet to QList, is to remove duplicated items.
  gradeList  = gradeList.toSet().toList();
  strokeList = strokeList.toSet().toList();
  // Sort them.
  qSort( gradeList );
  qSort( strokeList );
  kDebug() << "Max. grade:" << gradeList.last() << endl;
  kDebug() << "Max. stroke count:" << strokeList.last() << endl;

  // Finaly setup the view.
  _view->setupView( this, kanjiList, gradeList, strokeList );
}

void KanjiBrowser::showPreferences()
{
  if( KConfigDialog::showDialog( "settings" ) )
  {
    return;
  }

  QWidget *preferences = new QWidget();
  Ui::preferences layout;
  layout.setupUi( preferences );

  KConfigDialog *dialog = new KConfigDialog( this, "settings", KanjiBrowserConfigSkeleton::self() );
  dialog->addPage( preferences, i18n( "Settings" ), "help-contents" );
  connect( dialog, SIGNAL( settingsChanged( const QString& ) ),
            _view,   SLOT( loadSettings() ) );
  dialog->show();
}

#include "kanjibrowser.moc"
