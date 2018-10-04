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

/**
 * Future Plans:
 *
 * Design a custom QGridLayout to rearrange buttons dynamically to resize
 * Design multiple radical file handling
 * Icon set for displaying radicals outside of this bizarre unicode section
 * Radical decomposition implementation
 */

#include "buttongrid.h"

#include "radicalbutton.h"

#include <KLocalizedString>

#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QWidget>

ButtonGrid::ButtonGrid( QWidget *parent, RadicalFile *radicalInfo )
: QWidget( parent )
, CurrentMode( Selection )
, m_radicalInfo( radicalInfo )
{
  if ( m_radicalInfo )
  {
    buildRadicalButtons();
  }
}

ButtonGrid::~ButtonGrid()
{
}

void ButtonGrid::buildRadicalButtons()
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
    RadicalButton *button = new RadicalButton( *it, this );
    grid->addWidget( button, row_index++, column_index );
    //Bind slots/signals for this button
    connect( button, SIGNAL(userClicked(QString,RadicalButton::ButtonStatus)),
               this,   SLOT(radicalClicked(QString,RadicalButton::ButtonStatus)) );
    connect(   this, SIGNAL(clearButtonSelections()),
             button,   SLOT(resetButton()) );

    //Add this button to our list
    m_buttons.insert( *it, button );

    last_column = column_index;
    ++it;
  }
  setLayout( grid );
}

void ButtonGrid::clearSelections()
{
  m_selectedRadicals.clear();
  emit clearButtonSelections();
}

void ButtonGrid::radicalClicked(  const QString &newrad
                                , RadicalButton::ButtonStatus newStatus )
{
  if( newStatus == RadicalButton::Related )
  {
    ; //Do something fancy
  }
  else if( newStatus == RadicalButton::Normal
    || newStatus == RadicalButton::Selected )
  {
    CurrentMode = Selection;

    if( newStatus == RadicalButton::Normal )
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

void ButtonGrid::setFont( const QFont &font )
{
  foreach( QPushButton *button, m_buttons )
  {
    button->setFont( font );
  }
}

void ButtonGrid::updateButtons()
{
  if ( ! m_radicalInfo )
  {
    return;
  }
  //Special Case/Early exit: no radicals selected
  if( m_selectedRadicals.isEmpty() )
  {
    QList<Kanji> blankList;
    foreach( RadicalButton *button, m_buttons )
    {
      button->setStatus( RadicalButton::Normal );
    }

    emit possibleKanji( blankList );
    return;
  }

  //Figure out what our kanji possibilities are
  QSet<Kanji> kanjiSet = m_radicalInfo->kanjiContainingRadicals( m_selectedRadicals );

  //Convert to a list, sort, and tell the world!
  QList<Kanji> kanjiList = kanjiSet.toList();
  qSort( kanjiList );
  emit possibleKanji( kanjiList );

  //Do the announcement of the selected radical list
  QStringList radicalList( m_selectedRadicals.toList() );
  emit signalChangeStatusbar( i18n( "Selected Radicals: " )
                              + radicalList.join( ", " ) );

  //Now figure out what our remaining radical possibilities are
  QSet<QString> remainingRadicals = m_radicalInfo->radicalsInKanji( kanjiSet );
  //Remove the already selected ones
  remainingRadicals -= m_selectedRadicals;

  //Now go through and set status appropriately
  QHash<QString, RadicalButton*>::iterator i = m_buttons.begin();
  while( i != m_buttons.end() )
  {
    if( m_selectedRadicals.contains( i.key() ) )
    {
      i.value()->setStatus( RadicalButton::Selected );
    }
    else if( remainingRadicals.contains( i.key() ) )
    {
      i.value()->setStatus( RadicalButton::Normal );
    }
    else
    {
      i.value()->setStatus( RadicalButton::NotAppropriate );
    }

    ++i;
  }
}


