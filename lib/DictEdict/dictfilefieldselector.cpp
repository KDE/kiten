/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dictfilefieldselector.h"

#include <KActionSelector>
#include <KConfigSkeleton>
#include <KLocalizedString>

#include <QListWidget>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>

DictFileFieldSelector::DictFileFieldSelector( KConfigSkeleton *config,
                       const QString &dictionaryTypeName, QWidget *parent )
: DictionaryPreferenceDialog( parent, dictionaryTypeName )
, m_dictName( dictionaryTypeName )
{
  QVBoxLayout *newTabLayout = new QVBoxLayout();

  //Make selection box
  m_listView = new KActionSelector();
  m_listView->setAvailableLabel( i18n( "&Available Fields:" ) );
  newTabLayout->addWidget( m_listView );

  //Add layout to our widget
  this->setLayout( newTabLayout );

  //Create Default List
  m_completeList.append( QStringLiteral("--NewLine--") );
  m_completeList.append( QStringLiteral("--NewLine--") );
  m_completeList.append( QStringLiteral("--NewLine--") );
  m_completeList.append( QStringLiteral("Word/Kanji") );
  m_completeList.append( QStringLiteral("Reading") );
  m_completeList.append( QStringLiteral("Meaning") );

  //Make connections
  connect(m_listView, &KActionSelector::added, this, &DictFileFieldSelector::settingChanged);
  connect(m_listView, &KActionSelector::removed, this, &DictFileFieldSelector::settingChanged);
  connect(m_listView, &KActionSelector::movedUp, this, &DictFileFieldSelector::settingChanged);
  connect(m_listView, &KActionSelector::movedDown, this, &DictFileFieldSelector::settingChanged);

  m_config = config;
  updateWidgets();
}

DictFileFieldSelector::~DictFileFieldSelector()
{
}

void DictFileFieldSelector::addAvailable( const QStringList &list )
{
  m_completeList += list;
  updateWidgets();
}

void DictFileFieldSelector::readFromPrefs()
{
  QStringList selectedList;

  m_config->setCurrentGroup( "dicts_" + m_dictName );

  QStringList actionList = m_completeList;
  QString itemName = m_dictName + "__displayFields";
  KConfigSkeletonItem *item = m_config->findItem( itemName );
  if( item != nullptr )
  {
    selectedList = item->property().toStringList();
  }
  else
  {
    //it's not currently in the preferences list
    m_config->addItem(  new KConfigSkeleton::ItemStringList(  "dicts_" + m_dictName
                                                          , itemName
                                                          , *new QStringList() )
                    , itemName );
    m_config->load();
    selectedList = m_config->findItem( itemName )->property().toStringList();
  }

  foreach( const QString &it, selectedList)
  {
    actionList.removeAt( actionList.indexOf( it ) ); //don't just use remove()... will remove all
  }

  m_listView->availableListWidget()->clear();
  m_listView->selectedListWidget()->clear();
  m_listView->availableListWidget()->addItems( actionList );
  m_listView->selectedListWidget()->addItems( selectedList );
}

void DictFileFieldSelector::setAvailable( const QStringList &list )
{
  m_completeList = list;
  updateWidgets();
}

void DictFileFieldSelector::setDefaultList( const QStringList &list )
{
  m_defaultList = list;
}

void DictFileFieldSelector::settingChanged()
{
  emit widgetChanged();
}

void DictFileFieldSelector::updateSettings()
{
  writeToPrefs();
}

void DictFileFieldSelector::updateWidgets()
{
  readFromPrefs();
}

void DictFileFieldSelector::updateWidgetsDefault()
{
}

void DictFileFieldSelector::writeToPrefs()
{
  m_config->setCurrentGroup( "dicts_" + m_dictName );
  QStringList theList;
  KConfigSkeletonItem *item;
  QString itemName;

  for( int i = 0; i < m_listView->selectedListWidget()->count(); i++ )
  {
    theList.append( m_listView->selectedListWidget()->item( i )->text() );
  }

  itemName = m_dictName + "__displayFields";
  item = m_config->findItem( itemName );
  if( ! item )
  {
    item = new KConfigSkeleton::ItemStringList( "dicts_" + m_dictName, itemName, *new QStringList() );
    m_config->addItem( item, itemName );
  }
  item->setProperty( theList );

  m_config->save();
}


