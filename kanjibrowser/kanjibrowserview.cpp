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

void KanjiBrowserView::setupView(   const QHash< QString, QPair<int, int> > &kanji
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

  _kanji = kanji;
  _gradeList = kanjiGrades;
  _strokesList = strokeCount;

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

  _grades->setCurrentIndex( 1 );
  _strokes->setCurrentIndex( 1 );
  _strokes->setCurrentIndex( 0 );
  kDebug() << "Initial setup succeeded!" << endl;
}

#include "kanjibrowserview.moc"
