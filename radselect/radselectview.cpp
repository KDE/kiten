/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

/*
 *    Future Plans:
 *	Build a proper exception handling framework
 */

#include "radselectview.h"

#include "buttongrid.h"
#include "radselectconfig.h"

#include <KMessageBox>
#include <KLocalizedString>

#include <QApplication>
#include <QClipboard>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>

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
    QString kanjidicname = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kiten/kanjidic");
    if ( kanjidicname.isNull() )
    {
      KMessageBox::error( nullptr, i18n( "Kanji dictionary does not seem to "
                                   "be installed (file kiten/kanjidic), stroke "
                                   "count information will be unavailable." ) );
      strokes_low->setEnabled( false );
      strokes_high->setEnabled( false );
    }

    m_radicalInfo = new RadicalFile( radkfilename, kanjidicname );
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
  m_possibleKanji.clear();
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
  unsigned int low  = strokes_low->value();
  unsigned int high = strokes_high->value();

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
  Q_UNUSED( kanji );
  //TODO: loadKanji method
}

void RadSelectView::loadRadicals(  const QString &radicals
                                 , int strokeMin
                                 , int strokeMax )
{
  Q_UNUSED( radicals );
  Q_UNUSED( strokeMin );
  Q_UNUSED( strokeMax );
  //TODO: loadRadicals method
  emit searchModified();
}

void RadSelectView::loadSettings()
{
  //TODO: Add preferences for what to do on single/double click
  //Suggested options: Lookup in Kiten, Add to Search Bar, Copy to Clipboard
  selected_radicals->setFont( RadSelectConfigSkeleton::self()->resultListFont() );
  m_buttongrid->setFont( RadSelectConfigSkeleton::self()->font() );

  m_buttongrid->setSortByFrequency( RadSelectConfigSkeleton::self()->sortByFrequency() );
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


