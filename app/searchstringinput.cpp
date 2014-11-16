/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

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
  _actionFilterRare = _parent->actionCollection()->add<KSelectAction>( "filter_rare" );
  _actionFilterRare->setText( i18n( "Filter Type" ) );
  _actionFilterRare->addAction( i18n( "No filter" ) );
  _actionFilterRare->addAction( i18n( "Filter out rare" ) );
  _actionFilterRare->addAction( i18n( "Common/Uncommon" ) );

  _actionSearchSection = _parent->actionCollection()->add<KSelectAction>( "search_searchType" );
  _actionSearchSection->setText( i18n( "Match Type" ) );
  _actionSearchSection->addAction( i18n( "Exact Match" ) );
  _actionSearchSection->addAction( i18n( "Match Beginning" ) );
  _actionSearchSection->addAction( i18n( "Match Ending" ) );
  _actionSearchSection->addAction( i18n( "Match Anywhere" ) );

  _actionSelectWordType = _parent->actionCollection()->add<KSelectAction>( "search_wordType" );
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

  _actionFocusInput = _parent->actionCollection()->addAction( "focusinputfield", this, SLOT(focusInput()) );
  _parent->actionCollection()->setDefaultShortcut(_actionFocusInput, Qt::CTRL+Qt::Key_L );
  _actionFocusInput->setText( i18n( "Focus input field" ) );

  QWidgetAction *actionsearchbox = new QWidgetAction(this);
  actionsearchbox->setText( i18n( "Search Bar" ) );
  actionsearchbox->setDefaultWidget(_actionTextInput);
  _parent->actionCollection()->addAction( "searchbox", actionsearchbox );
  
  if( ! _actionFilterRare || ! _actionSearchSection
      || ! _actionSelectWordType || ! actionsearchbox )
  {
    qCritical() << "Error creating user interface elements: "
             << ! _actionFilterRare << ! _actionSearchSection
             << ! _actionSelectWordType << ! actionsearchbox;
  }

  //connect(actionTextInput, SIGNAL(returnPressed()), this, SIGNAL(search()));
  connect(_actionTextInput, static_cast<void (KHistoryComboBox::*)(const QString &)>(&KHistoryComboBox::activated), this, &SearchStringInput::test);
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
    result.setProperty( "common", "1" );
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
    if( bar->widgetForAction( _actionFilterRare ) != NULL )
      copy.removeProperty( "common" );

    if( bar->widgetForAction( _actionSelectWordType ) != NULL )
      copy.removeProperty("type");
  }

  _actionTextInput->setCurrentItem( copy.toString(), true );
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


