/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "searchStringInput.h"
#include "kiten.h"
#include "kitenconfig.h"

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <khistorycombobox.h>

#include <ktoolbar.h>

#include <kconfig.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <qclipboard.h>
#include <qlineedit.h>

searchStringInput::searchStringInput( kiten *iParent )
: QObject( iParent )
{
  parent = iParent;
  actionDeinflect = parent->actionCollection()->add<KToggleAction>( "search_deinflect" );
  actionDeinflect->setText( i18n( "Deinflect Verbs/Adjectives" ) );

  actionFilterRare = parent->actionCollection()->add<KToggleAction>( "search_filterRare" );
  actionFilterRare->setText (i18n( "&Filter Out Rare" ) );

  actionSearchSection = parent->actionCollection()->add<KSelectAction>( "search_searchType" );
  actionSearchSection->setText( i18n( "Match Type" ) );
  actionSearchSection->addAction( i18n( "Exact Match" ) );
  actionSearchSection->addAction( i18n( "Match Beginning" ) );
  actionSearchSection->addAction( i18n( "Match Anywhere" ) );

  actionSelectWordType = parent->actionCollection()->add<KSelectAction>( "search_wordType" );
/* Not quite working yet
  actionSelectWordType->setText(i18n("Word Type"));
  actionSelectWordType->addAction(i18n("Any"));
  actionSelectWordType->addAction(i18n("Verb"));
  actionSelectWordType->addAction(i18n("Noun"));
  actionSelectWordType->addAction(i18n("Adjective"));
  actionSelectWordType->addAction(i18n("Adverb"));
*/
  actionTextInput = new KHistoryComboBox( parent );
  actionTextInput->setDuplicatesEnabled( false );
  actionTextInput->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  updateFontFromConfig();

  actionFocusInput = parent->actionCollection()->addAction( "focusinputfield", this, SLOT( focusInput() ) );
  actionFocusInput->setShortcut( QString( "Ctrl+L" ) );
  actionFocusInput->setText( i18n( "Focus input field" ) );

  KAction *actionsearchbox = parent->actionCollection()->addAction( "searchbox" );
  actionsearchbox->setText( i18n( "Search Bar" ) );
  actionsearchbox->setDefaultWidget( actionTextInput );

  if( ! actionDeinflect || ! actionFilterRare
    || ! actionSearchSection || ! actionSelectWordType || ! actionsearchbox )
  {
    kError() << "Error creating user interface elements:"
             << !actionDeinflect << !actionFilterRare << !actionSearchSection
             << !actionSelectWordType << !actionsearchbox;
  }

  //connect(actionTextInput, SIGNAL(returnPressed()), this, SIGNAL(search()));
  connect( actionTextInput, SIGNAL( activated( const QString& ) ),
                      this,   SLOT( test() ) );
}

void searchStringInput::setDefaultsFromConfig()
{
  KitenConfigSkeleton* config = KitenConfigSkeleton::self();
  actionFilterRare->setChecked( config->common_only() );
  actionSearchSection->setCurrentItem( config->search_precision() );
  actionSelectWordType->setCurrentItem( config->search_limit_to_wordtype() );
}

void searchStringInput::updateFontFromConfig()
{
  actionTextInput->setFont( KitenConfigSkeleton::self()->font() );
}

DictQuery searchStringInput::getSearchQuery() const
{
  DictQuery result( actionTextInput->currentText() );

  if( actionSelectWordType->currentItem() )
  {
    result.setProperty( "type", actionSelectWordType->currentText() );
  }

  if( actionFilterRare->isChecked() )
  {
    result.setProperty( "common", "1" );
  }

  DictQuery::MatchType options[3] = {   DictQuery::matchExact
                                      , DictQuery::matchBeginning
                                      , DictQuery::matchAnywhere };
  result.setMatchType( options[ actionSearchSection->currentItem() ] );

  return result;
}

void searchStringInput::setSearchQuery( const DictQuery &query )
{
  kDebug() << "------------------------------Set Triggered";
  //First we set the various actions according to the query
  actionFilterRare->setChecked( query.getProperty( "common" ) == "1" );
  switch( query.getMatchType() )
  {
    case DictQuery::matchExact:
      actionSearchSection->setCurrentItem( 0 );
      break;
    case DictQuery::matchBeginning:
      actionSearchSection->setCurrentItem( 1 );
      break;
    case DictQuery::matchAnywhere:
      actionSearchSection->setCurrentItem( 2 );
      break;
  }

  //Secondly we remove aspects that are visible in the gui from the search string
  DictQuery copy( query );
  foreach( KToolBar *bar, parent->toolBars() )
  {
    if( bar->widgetForAction(actionFilterRare) != NULL )
        copy.removeProperty("common");

    if( bar->widgetForAction(actionSelectWordType) != NULL )
        copy.removeProperty("type");
  }

  actionTextInput->setCurrentItem( copy.toString(), true );
}

void searchStringInput::test()
{
  kDebug() << "test1";
  emit search();
  kDebug() << "test2";
}

void searchStringInput::focusInput()
{
  actionTextInput->setFocus();
  actionTextInput->lineEdit()->selectAll();
}

#include "searchStringInput.moc"
