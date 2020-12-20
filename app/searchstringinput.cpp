/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchstringinput.h"

#include "kiten.h"
#include "kitenconfig.h"

#include <KActionCollection>
#include <KHistoryComboBox>
#include <KLocalizedString>
#include <KSelectAction>
#include <KToolBar>

#include <QAction>
#include <QDebug>
#include <QLineEdit>

SearchStringInput::SearchStringInput( Kiten *parent )
: QObject( parent )
{
  _parent = parent;
  _actionFilterRare = _parent->actionCollection()->add<KSelectAction>( QStringLiteral("filter_rare") );
  _actionFilterRare->setText( i18n( "Filter Type" ) );
  _actionFilterRare->addAction( i18n( "No filter" ) );
  _actionFilterRare->addAction( i18n( "Filter out rare" ) );
  _actionFilterRare->addAction( i18n( "Common/Uncommon" ) );

  _actionSearchSection = _parent->actionCollection()->add<KSelectAction>( QStringLiteral("search_searchType") );
  _actionSearchSection->setText( i18n( "Match Type" ) );
  _actionSearchSection->addAction( i18n( "Exact Match" ) );
  _actionSearchSection->addAction( i18n( "Match Beginning" ) );
  _actionSearchSection->addAction( i18n( "Match Ending" ) );
  _actionSearchSection->addAction( i18n( "Match Anywhere" ) );

  _actionSelectWordType = _parent->actionCollection()->add<KSelectAction>( QStringLiteral("search_wordType") );
  _actionSelectWordType->setText( i18n( "Word Type" ) );
  _actionSelectWordType->addAction( i18n( "Any" ) );
  _actionSelectWordType->addAction( i18n( "Verb" ) );
  _actionSelectWordType->addAction( i18n( "Noun" ) );
  _actionSelectWordType->addAction( i18n( "Adjective" ) );
  _actionSelectWordType->addAction( i18n( "Adverb" ) );
  _actionSelectWordType->addAction( i18n( "Prefix" ) );
  _actionSelectWordType->addAction( i18n( "Suffix" ) );
  _actionSelectWordType->addAction( i18n( "Expression" ) );

  _actionTextInput = new KHistoryComboBox( _parent );
  _actionTextInput->setDuplicatesEnabled( false );
  _actionTextInput->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  updateFontFromConfig();

  _actionFocusInput = _parent->actionCollection()->addAction( QStringLiteral("focusinputfield"), this, SLOT(focusInput()) );
  _parent->actionCollection()->setDefaultShortcut(_actionFocusInput, Qt::CTRL+Qt::Key_L );
  _actionFocusInput->setText( i18n( "Focus input field" ) );

  QWidgetAction *actionsearchbox = new QWidgetAction(this);
  actionsearchbox->setText( i18n( "Search Bar" ) );
  actionsearchbox->setDefaultWidget(_actionTextInput);
  _parent->actionCollection()->addAction( QStringLiteral("searchbox"), actionsearchbox );
  
  if( ! _actionFilterRare || ! _actionSearchSection
      || ! _actionSelectWordType || ! actionsearchbox )
  {
    qCritical() << "Error creating user interface elements: "
             << ! _actionFilterRare << ! _actionSearchSection
             << ! _actionSelectWordType << ! actionsearchbox;
  }

  //connect(actionTextInput, SIGNAL(returnPressed()), this, SIGNAL(search()));
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  connect(_actionTextInput, static_cast<void (KHistoryComboBox::*)(const QString &)>(&KHistoryComboBox::activated), this, &SearchStringInput::test);
#else
  connect(_actionTextInput, static_cast<void (KHistoryComboBox::*)(const QString &)>(&KHistoryComboBox::textActivated), this, &SearchStringInput::test);
#endif
}

void SearchStringInput::focusInput()
{
  _actionTextInput->setFocus();
  _actionTextInput->lineEdit()->selectAll();
}

DictQuery SearchStringInput::getSearchQuery() const
{
  DictQuery result( _actionTextInput->currentText() );

  if( _actionFilterRare->currentItem() == DictQuery::Rare )
  {
    result.setProperty( QStringLiteral("common"), QStringLiteral("1") );
  }

  result.setFilterType( (DictQuery::FilterType)_actionFilterRare->currentItem() );
  result.setMatchType( (DictQuery::MatchType)_actionSearchSection->currentItem() );
  result.setMatchWordType( (DictQuery::MatchWordType)_actionSelectWordType->currentItem() );

  return result;
}

void SearchStringInput::setDefaultsFromConfig()
{
  KitenConfigSkeleton* config = KitenConfigSkeleton::self();
  _actionFilterRare->setCurrentItem( config->filter_rare() );
  _actionSearchSection->setCurrentItem( config->search_precision() );
  _actionSelectWordType->setCurrentItem( config->search_limit_to_wordtype() );
}

void SearchStringInput::setSearchQuery( const DictQuery &query )
{
  qDebug() << "------------------------------Set Triggered";
  //First we set the various actions according to the query
  _actionFilterRare->setCurrentItem( query.getFilterType() );
  _actionSearchSection->setCurrentItem( query.getMatchType() );
  _actionSelectWordType->setCurrentItem( query.getMatchWordType() );

  //Secondly we remove aspects that are visible in the gui from the search string
  DictQuery copy( query );
  foreach( KToolBar *bar, _parent->toolBars() )
  {
    if( bar->widgetForAction( _actionFilterRare ) != nullptr )
      copy.removeProperty( QStringLiteral("common") );

    if( bar->widgetForAction( _actionSelectWordType ) != nullptr )
      copy.removeProperty(QStringLiteral("type"));
  }

  _actionTextInput->addToHistory(copy.toString()); // Update history and completion list
  _actionTextInput->setCurrentItem( copy.toString(), true );
  _actionTextInput->reset(); // Call this manually when you call setCurrentItem
}

void SearchStringInput::test()
{
  qDebug() << "test1";
  emit search();
  qDebug() << "test2";
}

void SearchStringInput::updateFontFromConfig()
{
  _actionTextInput->setFont( KitenConfigSkeleton::self()->font() );
}


