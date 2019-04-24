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

/*
 *    Future Plans:
 *	Build a proper exception handling framework
 */

#include "radselectview.h"

#include "radicalfile.h"
#include "buttongrid.h"
#include "radselectconfig.h"

#include <KMessageBox>
#include <KLocalizedString>

#include <QApplication>
#include <QClipboard>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <QWidget>

RadSelectView::RadSelectView( QWidget *parent )
: QWidget( parent )
{
  //Setup the ui from the .ui file
  setupUi( this );
  m_radicalInfo = nullptr;
  //Load the radical information
  QString radkfilename = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kiten/radkfile"));
  if ( radkfilename.isNull() )
  {
    KMessageBox::error( nullptr, i18n( "Kanji radical information does not seem to "
                                 "be installed (file kiten/radkfile), this "
                                 "file is required for this app to function." ) );
  }
  else
  {
    m_radicalInfo = new RadicalFile( radkfilename );
  }

  //Configure the scrolling area
  m_buttongrid = new ButtonGrid( radical_box, m_radicalInfo );
  radical_box->setWidget( m_buttongrid );
  radical_box->setWidgetResizable( true );

  //Configure the stroke selectors
  strokes_low->setSpecialValueText(  i18nc( "Minimum number of strokes for a kanji", "Min" ) );
  strokes_high->setSpecialValueText( i18nc( "Maximum number of strokes for a kanji", "Max") );

  //== Now we connect all our signals ==
  //Connect our radical grid to our adding method
  connect( m_buttongrid, &ButtonGrid::possibleKanji,
                   this,   &RadSelectView::listPossibleKanji );
  //Connect the results selection to our logic
  connect( selected_radicals, &QListWidget::itemClicked,
                        this,   &RadSelectView::kanjiClicked );
  connect( selected_radicals, &QListWidget::itemDoubleClicked,
                        this,   &RadSelectView::kanjiDoubleClicked );
  //Connect our stroke limit actions
  connect( strokes_low, SIGNAL(valueChanged(int)),
                  this,   SLOT(strokeLimitChanged(int)) );
  connect( strokes_high, SIGNAL(valueChanged(int)),
                   this,   SLOT(strokeLimitChanged(int)) );
  //Connect statusbar updates
  connect( m_buttongrid, &ButtonGrid::signalChangeStatusbar,
                   this, &RadSelectView::signalChangeStatusbar );

  //Connect our clear button
  connect( clear_button, &QAbstractButton::clicked,
                   this,   &RadSelectView::clearSearch );

  // copy text from copied_line (QLineEdit) to clipboard
  connect( copy_button, &QAbstractButton::clicked,
                  this,   &RadSelectView::toClipboard );

  loadSettings();
}

RadSelectView::~RadSelectView()
{
  delete m_radicalInfo;
}

void RadSelectView::changedSearch()
{
  emit searchModified();
}

void RadSelectView::clearSearch()
{
  m_buttongrid->clearSelections();
  selected_radicals->clear();
  strokes_low->setValue( 0 );
  strokes_high->setValue( 0 );
}

void RadSelectView::kanjiClicked( QListWidgetItem *item )
{
  QString allText = i18nc( "@item:inlist all matches should be found", "(ALL)" );
  QString finalText;
  if( item->text() == allText )
  {
    foreach(  QListWidgetItem *listItem
            , selected_radicals->findItems( "*", Qt::MatchWildcard ) )
    {
      if( listItem->text() != allText )
      {
        finalText += listItem->text();
      }
    }
  }
  else
  {
    finalText = item->text();
  }

  QApplication::clipboard()->setText( finalText, QClipboard::Selection );
}

void RadSelectView::kanjiDoubleClicked( QListWidgetItem *item )
{
  QString str = copied_line->text();
  int pos = copied_line->cursorPosition();
  str.insert( pos, item->text() );
  copied_line->setText( str );
  copied_line->setCursorPosition( pos + 1 );
}

void RadSelectView::listPossibleKanji( const QList<Kanji>& list )
{
  int low  = strokes_low->value();
  int high = strokes_high->value();

  //Modification of the stroke boxes
  //We want to move the max value to something reasonable...
  //for example, 5 above the current max value so that rollover
  //works nicely. We want to reset to all if the list is empty.
  //And we also don't limit if the current value is higher than
  //max value in the list
  int newMax = 20;
  if( list.count() < 1
    || list.last().strokes() < low
    || list.last().strokes() + 5 < high )
  {
    newMax = 99;
  }
  else
  {
    newMax = list.last().strokes() + 5;
  }

  strokes_low->setMaximum( newMax );
  strokes_high->setMaximum( newMax );
  if( high == 0 )
  {
    high = 99;
  }

  selected_radicals->clear();
  foreach( const Kanji &it, list )
  {
    if( low <= it.strokes() && it.strokes() <= high )
    {
      new QListWidgetItem( (QString)it, selected_radicals );
    }
  }

  m_possibleKanji = list;

  emit searchModified();
}

void RadSelectView::loadKanji( QString &kanji )
{
  //TODO: loadKanji method
}

void RadSelectView::loadRadicals(  const QString &radicals
                                 , int strokeMin
                                 , int strokeMax )
{
  //TODO: loadRadicals method
  emit searchModified();
}

void RadSelectView::loadSettings()
{
  //TODO: Add preferences for what to do on single/double click
  //Suggested options: Lookup in Kiten, Add to Search Bar, Copy to Clipboard
  m_buttongrid->setFont( RadSelectConfigSkeleton::self()->font() );
}

void RadSelectView::strokeLimitChanged( int newvalue )
{
  int low  = strokes_low->value();
  int high = strokes_high->value();
  if( low > high )
  {
    if( low == newvalue )
    {
      strokes_high->setValue( newvalue );
    }
    else
    {
      strokes_low->setValue( newvalue );
    }
  }

  //This will force reevaluation of the list if it's needed
  QList<Kanji> newList = m_possibleKanji;
  listPossibleKanji( newList );
}

void RadSelectView::toClipboard()
{
  QClipboard *cb = QApplication::clipboard();
  cb->setText( copied_line->text(), QClipboard::Clipboard );
  cb->setText( copied_line->text(), QClipboard::Selection );
}


