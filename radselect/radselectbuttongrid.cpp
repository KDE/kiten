/*
 This file is part of kiten, a KDE Japanese Reference Tool
 Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*/


/*
*    Future Plans:
*	Design a custom QGridLayout to rearrange buttons dynamically to resize
*	Design multiple radical file handling
*	Icon set for displaying radicals outside of this bizarre unicode section
*	Radical decomposition implementation
*/

#include "radselectbuttongrid.h"
#include "radicalbutton.h"

#include <QtCore/QString>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

#include <QtGui/QFontMetrics>
#include <QtCore/QSize>

#include <klocale.h>

#include <kdebug.h>

radselectButtonGrid::radselectButtonGrid( QWidget *parent, radicalFile *iradicalInfo )
: QWidget( parent )
, m_currentMode( kSelection )
, m_radicalInfo( iradicalInfo )
{
  if ( m_radicalInfo )
  {
    buildRadicalButtons();
  }
}

radselectButtonGrid::~radselectButtonGrid()
{
}

void radselectButtonGrid::buildRadicalButtons()
{
  //Setup the grid
  QGridLayout *grid = new QGridLayout( this );

  //Now make labels
  for( unsigned int i = 0; i < number_of_radical_columns; i++ )
  {
    QLabel *header = new QLabel( QString::number( i + 1 ), this );
    header->setAlignment( Qt::AlignHCenter );
    grid->addWidget( header, 0, i );
  }

  //Get a list of radicals (organized by strokes)
  QMultiMap<int, Radical> *radicalMap = m_radicalInfo->mapRadicalsByStrokes();
  QMultiMap<int, Radical>::const_iterator it = radicalMap->constBegin();
  //Now create all the buttons
  int last_column = 0;
  int row_index = 1;
  while( it != radicalMap->constEnd() )
  {
    //For each radical, figure out which slot it goes in (0-based column index)
    unsigned int column_index = it.key() - 1;
    if( column_index >= number_of_radical_columns )
    {
      column_index = number_of_radical_columns - 1;
    }

    //If we're starting a new column, reset the row
    if( last_column != column_index )
    {
      row_index = 1;
    }

    //Make the button
    radicalButton *button = new radicalButton( *it, this );
    grid->addWidget( button, row_index++, column_index );
    //Bind slots/signals for this button
    connect( button, SIGNAL( userClicked( const QString&, radicalButton::ButtonStatus ) ),
               this,   SLOT( radicalClicked( const QString&, radicalButton::ButtonStatus ) ) );
    connect(   this, SIGNAL( clearButtonSelections() ),
             button,   SLOT( resetButton() ) );

    //Add this button to our list
    m_buttons.insert( *it, button );

    last_column = column_index;
    ++it;
  }
  setLayout( grid );
}

void radselectButtonGrid::setFont( const QFont &font )
{
  foreach( QPushButton *button, m_buttons )
  {
    button->setFont( font );
  }
}

void radselectButtonGrid::radicalClicked(  const QString &newrad
                                         , radicalButton::ButtonStatus newStatus )
{
  if( newStatus == radicalButton::kRelated )
  {
    ; //Do something fancy
  }
  else if( newStatus == radicalButton::kNormal
    || newStatus == radicalButton::kSelected )
  {
    m_currentMode = kSelection;

    if( newStatus == radicalButton::kNormal )
    {
      m_selectedRadicals.remove( newrad );
      if( m_selectedRadicals.isEmpty() )
      {
        emit signalChangeStatusbar( i18n( "No Radicals Selected" ) );
      }
    }
    else
    {
      m_selectedRadicals.insert( newrad );
    }

    updateButtons();
  }
}

void radselectButtonGrid::updateButtons()
{
  if ( ! m_radicalInfo )
  {
    return;
  }
  //Special Case/Early exit: no radicals selected
  if( m_selectedRadicals.isEmpty() )
  {
    QList<Kanji> blankList;
    foreach( radicalButton *button, m_buttons )
    {
      button->setStatus( radicalButton::kNormal );
    }

    emit possibleKanji( blankList );
    return;
  }

  //Figure out what our kanji possibilites are
  QSet<Kanji> kanjiSet = m_radicalInfo->kanjiContainingRadicals( m_selectedRadicals );

  //Convert to a list, sort, and tell the world!
  QList<Kanji> kanjiList = kanjiSet.toList();
  qSort( kanjiList );
  emit possibleKanji( kanjiList );

  //Do the announcement of the selected radical list
  QStringList radicalList( m_selectedRadicals.toList() );
  emit signalChangeStatusbar( i18n( "Selected Radicals: " ) + radicalList.join( ", " ) );

  //Now figure out what our remaining radical possibilities are
  QSet<QString> remainingRadicals = m_radicalInfo->radicalsInKanji(kanjiSet);
  //Remove the already selected ones
  remainingRadicals -= m_selectedRadicals;

  //Now go through and set status appropriately
  QHash<QString, radicalButton*>::iterator i = m_buttons.begin();
  while( i != m_buttons.end() )
  {
    if( m_selectedRadicals.contains( i.key() ) )
    {
      i.value()->setStatus( radicalButton::kSelected );
    }
    else if( remainingRadicals.contains( i.key() ) )
    {
      i.value()->setStatus( radicalButton::kNormal );
    }
    else
    {
      i.value()->setStatus( radicalButton::kNotAppropriate );
    }

    ++i;
  }
}

void radselectButtonGrid::clearSelections()
{
  m_selectedRadicals.clear();
  emit clearButtonSelections();
}

#include "radselectbuttongrid.moc"
