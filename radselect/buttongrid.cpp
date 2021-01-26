/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

/**
 * Future Plans:
 *
 * Design a custom QGridLayout to rearrange buttons dynamically to resize
 * Design multiple radical file handling
 * Icon set for displaying radicals outside of this bizarre unicode section
 * Radical decomposition implementation
 */

#include "buttongrid.h"


#include <KLocalizedString>

#include <QGridLayout>
#include <QLabel>
#include <QString>

ButtonGrid::ButtonGrid( QWidget *parent, RadicalFile *radicalInfo )
: QWidget( parent )
, CurrentMode( Selection )
, m_radicalInfo( radicalInfo )
, m_sortByFrequency( false )
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
  if( layout() )
  {
    // Instead of iterating over our children and deleting them one by one, we
    // simply reparent the previous layout to a temporary widget; when it goes
    // out of scope, everything will be automatically deleted.
    // Thanks to https://stackoverflow.com/a/10439207 for the tip!
    QWidget().setLayout( layout() );
  }

  //Setup the grid
  QGridLayout *grid = new QGridLayout( this );

  //Now make labels
  for( unsigned int i = 0; i < number_of_radical_columns; i++ )
  {
    QString headerString = QString::number( i + 1 );
    if( i == ( number_of_radical_columns - 1 ) )
    {
      headerString.append( QStringLiteral( "+" ) );
    }
    QLabel *header = new QLabel( headerString, this );
    header->setAlignment( Qt::AlignHCenter );
    grid->addWidget( header, 0, i );
  }

  //Get a list of radicals (organized by strokes)
  QMultiMap<int, Radical> *radicalMap = m_radicalInfo->mapRadicalsByStrokes( number_of_radical_columns );
  QList<int> radicalStrokeCounts = radicalMap->uniqueKeys();
  //Now create all the buttons
  foreach( int strokeCount, radicalStrokeCounts )
  {
    //(0-based column index)
    unsigned int column_index = strokeCount - 1;
    int row_index = 1;

    QList<Radical> radicals = radicalMap->values( strokeCount );
    std::sort( radicals.begin(), radicals.end(),
               m_sortByFrequency ? Radical::compareFrequencies : Radical::compareIndices );
    foreach( const Radical &radical, radicals )
    {
      //Make the button
      RadicalButton *button = new RadicalButton( radical.toString(), this );
      grid->addWidget( button, row_index++, column_index );
      //Bind slots/signals for this button
      connect( button, &RadicalButton::userClicked,
                 this,   &ButtonGrid::radicalClicked );
      connect(   this, &ButtonGrid::clearButtonSelections,
               button,   &RadicalButton::resetButton );

      //Add this button to our list
      m_buttons.insert( radical.toString(), button );
    }
  }
  delete radicalMap;
  setLayout( grid );

  updateButtons();
}

void ButtonGrid::setSortByFrequency( bool enable )
{
  if( m_sortByFrequency != enable )
  {
    m_sortByFrequency = enable;
    buildRadicalButtons();
  }
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
  QList<Kanji> kanjiList = kanjiSet.values();
  std::sort(kanjiList.begin(), kanjiList.end());
  emit possibleKanji( kanjiList );

  //Do the announcement of the selected radical list
  QStringList radicalList( m_selectedRadicals.values() );
  emit signalChangeStatusbar( i18n( "Selected Radicals: " )
                              + radicalList.join(QLatin1String(", ") ) );

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


