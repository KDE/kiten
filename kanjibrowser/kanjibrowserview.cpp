/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kanjibrowserview.h"

#include "DictKanjidic/dictfilekanjidic.h"
#include "DictKanjidic/entrykanjidic.h"
#include "dictquery.h"
#include "entrylist.h"
#include "kanjibrowser.h"
#include "kanjibrowserconfig.h"

#include <QAction>
#include <QClipboard>
#include <QDebug>
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>

KanjiBrowserView::KanjiBrowserView( QWidget *parent )
: QWidget( parent )
, _currentKanji( 0 )
{
  setupUi( this );
  loadSettings();
}

KanjiBrowserView::~KanjiBrowserView()
{

}

void KanjiBrowserView::changeGrade( const int grade )
{
  _currentGradeList.clear();

  // Indexes of items in the ComboBox:
  //   All Jouyou Kanji Grades: 0
  //   Grade 1: 1
  //   Grade 2: 2
  //   .
  //   .
  //   .
  //   Not in Jouyou list: ComboBox->count() - 1

  if( grade == AllJouyouGrades )
  {
    // Add the all the grades found in our list.
    foreach( const int grd, _gradeList )
    {
      _currentGradeList << grd;
    }
  }
  // Here the user selected "Not in Jouyou list".
  else if( grade == ( _grades->count() - 1 ) )
  {
    // Only show the kanji with grade 0 (not in Jouyou).
    _currentGradeList << 0;
  }
  // It seems KANJIDIC doesn't have a G7 (grade 7) kanji.
  // If the user selects G8 or above, we need to add 1 to the grade
  // because the index (from the ComboBox) and the grade will be different.
  else if( grade >= Grade7 )
  {
    _currentGradeList << ( grade + 1 );
  }
  // Show the kanji with the selected grade.
  else
  {
    _currentGradeList << grade;
  }

  // Reload our QListWidget widget.
  reloadKanjiList();
}

void KanjiBrowserView::changeStrokeCount( const int strokes )
{
  _currentStrokesList.clear();

  // Indexes of items in the ComboBox:
  //   No stroke limit: 0
  //   1 stroke: 1
  //   2 strokes: 2
  //   .
  //   .
  //   .

  // We don't need to filter any kanji by stroke number.
  if( strokes == NoStrokeLimit )
  {
    // Add all the strokes found to our the list.
    foreach( const int stroke, _strokesList )
    {
      _currentStrokesList << stroke;
    }
  }
  // Show the kanji with the selected number of strokes.
  else
  {
    _currentStrokesList << strokes;
  }

  // Reload our QListWidget widget.
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

QString KanjiBrowserView::convertToCSS( const QFont &font )
{
  QString weight;
  switch( font.weight() )
  {
    case QFont::Light:
      weight = QStringLiteral("lighter");
      break;
    case QFont::Normal:
      weight = QStringLiteral("normal");
      break;
    case QFont::Bold:
      weight = QStringLiteral("bold");
      break;
  }

  QString style;
  switch( font.style() )
  {
    case QFont::StyleNormal:
      style = QStringLiteral("normal");
      break;
    case QFont::StyleItalic:
      style = QStringLiteral("italic");
      break;
    case QFont::StyleOblique:
      style = QStringLiteral("oblique");
      break;
  }

  return QString( "font-family:\"%1\";"
                  "font-size:%2px;"
                  "font-weight:%3;"
                  "font-style:%4;" ).arg( font.family() )
                                    .arg( font.pointSizeF() )
                                    .arg( weight )
                                    .arg( style );
}

void KanjiBrowserView::loadSettings()
{
  _kanjiList->setFont( KanjiBrowserConfigSkeleton::self()->kanjiListFont() );
  _kanjiSize = KanjiBrowserConfigSkeleton::self()->kanjiSize();
  _kanaFont  = KanjiBrowserConfigSkeleton::self()->kanaFont();
  _labelFont = KanjiBrowserConfigSkeleton::self()->labelFont();

  // Reload the Kanji Information page with the new font changes.
  if( _currentKanji != nullptr )
  {
    showKanjiInformation( _currentKanji );
  }
}

void KanjiBrowserView::reloadKanjiList()
{
  // Grade and strokes lists have the information of
  // which kanji we are going to filter.
  // We just iterate on them to actually do the filtering.
  QStringList list;
  foreach( const int strokes, _currentStrokesList )
  {
    foreach( const int grade, _currentGradeList )
    {
      list.append( _kanji.keys( qMakePair( grade, strokes ) ) );
    }
  }

  _kanjiList->clear();
  _kanjiList->addItems( list );

  // Update our status bar with the number of kanji filtered.
  statusBarChanged( i18np( "%1 kanji found", "%1 kanji found", _kanjiList->count() ) );
}

void KanjiBrowserView::searchKanji( QListWidgetItem *item )
{
  if(   _currentKanji != nullptr
      && item->text() == _currentKanji->getWord() )
  {
    return;
  }

  _goToKanjiInfo->setText( i18n( "About %1", item->text() ) );
  _copyToClipboard->setText( i18n( "Copy %1 to clipboard", item->text() ) );
  _copyToClipboard->setVisible( true );

  Entry *result = _parent->_dictFileKanjidic->doSearch( DictQuery( item->text() ) )->first();
  EntryKanjidic *kanji = static_cast<EntryKanjidic*>( result );
  _currentKanji = kanji;

  showKanjiInformation( kanji );
}

void KanjiBrowserView::setupView(   KanjiBrowser *parent
                                  , const QHash< QString, QPair<int, int> > &kanji
                                  , QList<int> &kanjiGrades
                                  , QList<int> &strokeCount )
{
  if( kanji.isEmpty() || kanjiGrades.isEmpty() || strokeCount.isEmpty() )
  {
    qDebug() << "One or more of our lists are empty (kanji, grades, strokes).";
    qDebug() << "Could not load the view properly.";
    KMessageBox::error( this, i18n( "Could not load the necessary kanji information." ) );
    return;
  }

  _parent = parent;
  _kanji = kanji;
  _gradeList = kanjiGrades;
  _strokesList = strokeCount;

  QAction *goToKanjiList = _parent->actionCollection()->addAction( QStringLiteral("kanji_list") );
  goToKanjiList->setText( i18n( "Kanji &List" ) );

  _goToKanjiInfo = _parent->actionCollection()->addAction( QStringLiteral("kanji_info") );
  _goToKanjiInfo->setText( i18n( "Kanji &Information" ) );

  _copyToClipboard = _parent->actionCollection()->addAction( QStringLiteral("copy_kanji_to_clipboard") );
  _copyToClipboard->setVisible(false);

  _grades->addItem( i18n( "All Jouyou Kanji grades" ) );
  foreach( const int &grade, kanjiGrades )
  {
    // Grades 9 and above are considered Jinmeiyou.
    if( grade >= Jinmeiyou )
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
    _strokes->addItem( i18np( "%1 stroke", "%1 strokes", stroke ) );
  }

  connect(_grades, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &KanjiBrowserView::changeGrade);
  connect(_strokes, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &KanjiBrowserView::changeStrokeCount);
  connect(_kanjiList, &QListWidget::itemClicked, this, &KanjiBrowserView::searchKanji);
  connect(_kanjiList, &QListWidget::itemClicked, _goToKanjiInfo, &QAction::triggered);
  connect(goToKanjiList, &QAction::triggered, this, &KanjiBrowserView::changeToListPage);
  connect(_goToKanjiInfo, &QAction::triggered, this, &KanjiBrowserView::changeToInfoPage);
  connect(_copyToClipboard, &QAction::triggered, this, &KanjiBrowserView::toClipboard);

  // Set the current grade (Grade 1).
  _grades->setCurrentIndex( 1 );
  // Set the current number of strokes (No stroke limit).
  // NOTE: we change from '1 stroke' to 'No stroke limit'
  // to let the ComboBox notice the change and do the filter.
  _strokes->setCurrentIndex( 1 );
  _strokes->setCurrentIndex( NoStrokeLimit );

  qDebug() << "Initial setup succeeded!";
}

void KanjiBrowserView::showKanjiInformation( const EntryKanjidic *kanji )
{
  // This font is shipped with Kiten and should not be changed as it shows
  // the stroke order of a kanji.
  QFont kanjiFont( QStringLiteral("KanjiStrokeOrders") );
  kanjiFont.setPointSizeF( _kanjiSize.toReal() );

  QString text;
  text.append( "<html><body><style>" );
  text.append( QStringLiteral( ".kanji { %1 }" ).arg( convertToCSS( kanjiFont ) ) );
  text.append( QStringLiteral( ".label { %1 }" ).arg( convertToCSS( _labelFont ) ) );
  text.append( QStringLiteral( ".kana  { %1 }" ).arg( convertToCSS( _kanaFont ) ) );
  text.append( "</style>" );

  // Put the kanji.
  text.append( QStringLiteral( "<table><tr><td><p class=\"kanji\">%1</p></td>" )
                        .arg( kanji->getWord() ) );

  // Now the kanji grades and number of strokes.
  text.append( "<td>" );
  if( ! kanji->getKanjiGrade().isEmpty() )
  {
    text.append( QStringLiteral( "<p class=\"label\">%1 %2</p></br>" )
                          .arg( i18n( "Grade:" ) )
                          .arg( kanji->getKanjiGrade() ) );
  }
  text.append( QStringLiteral( "<p class=\"label\">%1 %2</p></td></tr></table>" )
                        .arg( i18n( "Strokes:" ) )
                        .arg( kanji->getStrokesCount() ) );

  // Onyomi readings.
  if( ! kanji->getOnyomiReadingsList().isEmpty() )
  {
    text.append( QString( "<p class=\"label\">%1"
                          "<span class=\"kana\">%2</span></p></br>" )
                          .arg( i18n( "Onyomi: " ) )
                          .arg( kanji->getOnyomiReadings() ) );
  }

  // Kunyomi readings.
  if( ! kanji->getKunyomiReadingsList().isEmpty() )
  {
    text.append( QString( "<p class=\"label\">%1"
                          "<span class=\"kana\">%2</span></p></br>" )
                          .arg( i18n( "Kunyomi: " ) )
                          .arg( kanji->getKunyomiReadings() ) );
  }

  // Special readings used in names.
  if( ! kanji->getInNamesReadingsList().isEmpty() )
  {
    text.append( QString( "<p class=\"label\">%1"
                          "<span class=\"kana\">%2</span></p></br>" )
                          .arg( i18n( "In names: " ) )
                          .arg( kanji->getInNamesReadings() ) );
  }

  // Reading used as radical.
  if( ! kanji->getAsRadicalReadingsList().isEmpty() )
  {
    text.append( QString( "<p class=\"label\">%1"
                          "<span class=\"kana\">%2</span></p></br>" )
                          .arg( i18n( "As radical: " ) )
                          .arg( kanji->getAsRadicalReadings() ) );
  }

  // Meanings
  text.append( "<p class=\"label\">" );
  if( kanji->getMeaningsList().count() == 1 )
  {
    text.append( i18n( "Meaning: ") );
  }
  else
  {
    text.append( i18n( "Meanings: " ) );
  }
  text.append( QStringLiteral( "<span class=\"kana\">%1</span></p>" )
                        .arg( kanji->getMeanings() ) );

  // Close remaining tags and set the HTML text.
  text.append( "</body></html>" );
  _kanjiInformation->setHtml( text );
}

void KanjiBrowserView::toClipboard()
{
  QClipboard *cb = QApplication::clipboard();
  cb->setText( _currentKanji->getWord(), QClipboard::Clipboard );
  cb->setText( _currentKanji->getWord(), QClipboard::Selection );
}

