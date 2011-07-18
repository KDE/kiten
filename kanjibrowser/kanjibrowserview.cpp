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

#include "kanjibrowserview.h"

#include "kanjibrowser.h"
#include "DictKanjidic/dictfilekanjidic.h"
#include "DictKanjidic/entrykanjidic.h"
#include "dictquery.h"
#include "entrylist.h"

#include <KAction>
#include <KActionCollection>
#include <KConfigSkeleton>
#include <KDebug>
#include <KMessageBox>

KanjiBrowserView::KanjiBrowserView( QWidget *parent )
: QWidget( parent )
{
  setupUi( this );
}

KanjiBrowserView::~KanjiBrowserView()
{

}

void KanjiBrowserView::changeGrade( const int grade )
{
  _currentGradeList.clear();

  if( grade == 0 )
  {
    foreach( const int grd, _gradeList )
    {
      _currentGradeList << grd;
    }
  }
  else if( grade == ( _grades->count() - 1 ) )
  {
    _currentGradeList << 0;
  }
  else if( grade >= 7 )
  {
    _currentGradeList << ( grade + 1 );
  }
  else
  {
    _currentGradeList << grade;
  }

  reloadKanjiList();
}

void KanjiBrowserView::changeStrokeCount( const int strokes )
{
  _currentStrokesList.clear();

  if( strokes == 0 )
  {
    foreach( const int stroke, _strokesList )
    {
      _currentStrokesList << stroke;
    }
  }
  else
  {
    _currentStrokesList << strokes;
  }

  reloadKanjiList();
}

void KanjiBrowserView::changeToInfoPage()
{
  _stackedWidget->setCurrentIndex( Info );
}

void KanjiBrowserView::changeToListPage()
{
  _stackedWidget->setCurrentIndex( List );
}

void KanjiBrowserView::reloadKanjiList()
{
  _kanjiList->clear();
  foreach( const int grade, _currentGradeList )
  {
    foreach( const int strokes, _currentStrokesList )
    {
      _kanjiList->addItems( _kanji.keys( qMakePair( grade, strokes ) ) );
    }
  }
}

void KanjiBrowserView::setupView(   KanjiBrowser *parent
                                  , const QHash< QString, QPair<int, int> > &kanji
                                  , QList<int> &kanjiGrades
                                  , QList<int> &strokeCount )
{
  if( kanji.isEmpty() || kanjiGrades.isEmpty() || strokeCount.isEmpty() )
  {
    kDebug() << "One or more of our lists are empty (kanji, grades, strokes)." << endl;
    kDebug() << "Could not load the view properly." << endl;
    KMessageBox::error( this, i18n( "Could not load the necessary kanji information." ) );
    return;
  }

  _parent = parent;
  _kanji = kanji;
  _gradeList = kanjiGrades;
  _strokesList = strokeCount;

  KAction *goToKanjiList = _parent->actionCollection()->addAction( "kanji_list" );
  goToKanjiList->setText( i18n( "Kanji &List" ) );

  KAction *goToKanjiInfo = _parent->actionCollection()->addAction( "kanji_info" );
  goToKanjiInfo->setText( i18n( "Kanji &Information" ) );

  _grades->addItem( i18n( "All Jouyou Kanji grades" ) );
  foreach( const int &grade, kanjiGrades )
  {
    if( grade >= 9 )
    {
      _grades->addItem( i18n( "Grade %1 (Jinmeiyou)", grade ) );
    }
    else
    {
      _grades->addItem( i18n( "Grade %1", grade ) );
    }
  }
  _grades->addItem( i18n( "Not in Jouyou list" ) );

  _strokes->addItem( i18n( "No stroke limit" ) );
  foreach( const int &stroke, strokeCount )
  {
    if( stroke == 1 )
    {
      _strokes->addItem( i18n( "%1 stroke", stroke ) );
    }
    else
    {
      _strokes->addItem( i18n( "%1 strokes", stroke ) );
    }
  }

  connect( _grades, SIGNAL( currentIndexChanged( int ) ),
              this,   SLOT( changeGrade( int ) ) );
  connect( _strokes, SIGNAL( currentIndexChanged( int ) ),
               this,   SLOT( changeStrokeCount( int ) ) );
  connect( _kanjiList, SIGNAL( executed( QListWidgetItem* ) ),
                 this,   SLOT( showKanjiInformation( QListWidgetItem* ) ) );
  connect( _kanjiList, SIGNAL( executed( QListWidgetItem* ) ),
           goToKanjiInfo, SIGNAL( triggered() ) );
  connect( goToKanjiList, SIGNAL( triggered() ),
                    this,   SLOT( changeToListPage() ) );
  connect( goToKanjiInfo, SIGNAL( triggered() ),
                    this,   SLOT( changeToInfoPage() ) );

  _grades->setCurrentIndex( 1 );
  _strokes->setCurrentIndex( 1 );
  _strokes->setCurrentIndex( 0 );

  kDebug() << "Initial setup succeeded!" << endl;
}

void KanjiBrowserView::showKanjiInformation( QListWidgetItem *item )
{
  Entry *result = _parent->_dictFileKanjidic->doSearch( DictQuery( item->text() ) )->first();
  EntryKanjidic *kanji = static_cast<EntryKanjidic*>( result );

  QString width = QString::number( _kanjiInformation->width() );

  QString text;
  text.append( "<html><style>" );
  text.append( "p { font-size:30px; }" );
  text.append( "</style><body><table width=\"" + width + "\">" );

  // Put the kanji.
  text.append( "<tr><td><p style=\"font-size:200px;\">" + kanji->getWord() + "</p></td>" );

  // Now the kanji grades and number of strokes.
  text.append( "<td><p><b>" + i18n( "Grade: " ) + "</b>"
               + kanji->getKanjiGrade() + "</p></br>" );
  text.append( "<p><b>" + i18n( "Strokes: " ) + "</b>"
               + kanji->getStrokesCount() + "</p></td></tr></table>" );

  // Onyomi readings.
  if( ! kanji->getOnyomiReadingsList().isEmpty() )
  {
    text.append( "<p><b>Onyomi: </b>" + kanji->getOnyomiReadings() + "</p></br>" );
  }

  // Kunyomi readings.
  if( ! kanji->getKunyomiReadingsList().isEmpty() )
  {
    text.append( "<p><b>Kunyomi: </b>" + kanji->getKunyomiReadings() + "</p></br>" );
  }

  // Special readings used in names.
  if( ! kanji->getInNamesReadingsList().isEmpty() )
  {
    text.append( "<p><b>" + i18n( "In names: " ) + "</b>"
                 + kanji->getInNamesReadings() + "</p></br>" );
  }

  // Reading used as radical.
  if( ! kanji->getAsRadicalReadingsList().isEmpty() )
  {
    text.append( "<p><b>" + i18n( "As radical: " ) + "</b>"
                 + kanji->getAsRadicalReadings() + "</p></br>" );
  }

  // Meanings
  text.append( "<p><b>" );
  if( kanji->getMeaningsList().count() == 1 )
  {
    text.append( i18n( "Meaning: ") );
  }
  else
  {
    text.append( i18n( "Meanings: " ) );
  }
  text.append( "</b>" + kanji->getMeanings() + "</p>" );

  // Close remaining tags and set the HTML text.
  text.append( "</body></html>" );
  _kanjiInformation->setHtml( text );
}

#include "kanjibrowserview.moc"
