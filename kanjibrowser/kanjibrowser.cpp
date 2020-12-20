/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kanjibrowser.h"

#include "ui_preferences.h"

#include "DictKanjidic/dictfilekanjidic.h"
#include "kanjibrowserconfig.h"
#include "kanjibrowserview.h"
#include "kitenmacros.h"

#include <QStatusBar>

#include <KActionCollection>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KStandardAction>

KanjiBrowser::KanjiBrowser()
: KXmlGuiWindow()
, _dictFileKanjidic( 0 )
{
  // Read the configuration file.
  _config = KanjiBrowserConfigSkeleton::self();
  _config->load();

  statusBar()->show();

  // Add some actions.
  KStandardAction::quit( this, SLOT(close()), actionCollection() );
  KStandardAction::preferences( this, SLOT(showPreferences()), actionCollection() );

  _view = new KanjiBrowserView( this->parentWidget() );
  connect(_view, &KanjiBrowserView::statusBarChanged, this, &KanjiBrowser::changeStatusBar);
  // Load the necessary information and setup the view.
  loadKanji();

  setCentralWidget( _view );
  setObjectName( QStringLiteral( "kanjibrowser" ) );

  setupGUI( Default, QStringLiteral("kanjibrowserui.rc") );
}

KanjiBrowser::~KanjiBrowser()
{
  if( _dictFileKanjidic )
  {
    delete _dictFileKanjidic;
  }
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

  qDebug() << "Loading kanji...";

  QString dictionary = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kiten/kanjidic"));
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
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList gradeSection = line.split( ' ', QString::SkipEmptyParts )
#else
    QStringList gradeSection = line.split( ' ', Qt::SkipEmptyParts )
#endif
                                   .filter( gradeMatch );
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList strokesSection = line.split( ' ', QString::SkipEmptyParts )
#else
    QStringList strokesSection = line.split( ' ', Qt::SkipEmptyParts )
#endif
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

  // This conversion from QList to QSet to QList, is to remove duplicated items.
  std::sort(gradeList.begin(), gradeList.end());
  gradeList.erase(std::unique(gradeList.begin(), gradeList.end()), gradeList.end());

  std::sort(strokeList.begin(), strokeList.end());
  strokeList.erase(std::unique(strokeList.begin(), strokeList.end()), strokeList.end());
  qDebug() << "Max. grade:" << gradeList.last();
  qDebug() << "Max. stroke count:" << strokeList.last();

  // Finally setup the view.
  _view->setupView( this, kanjiList, gradeList, strokeList );
}

void KanjiBrowser::showPreferences()
{
  if( KConfigDialog::showDialog( QStringLiteral("settings") ) )
  {
    return;
  }

  QWidget *preferences = new QWidget();
  Ui::preferences layout;
  layout.setupUi( preferences );

  KConfigDialog *dialog = new KConfigDialog( this, QStringLiteral("settings"), KanjiBrowserConfigSkeleton::self() );
  dialog->addPage( preferences, i18n( "Settings" ), QStringLiteral("help-contents") );
  connect(dialog, &KConfigDialog::settingsChanged, _view, &KanjiBrowserView::loadSettings);
  dialog->show();
}


