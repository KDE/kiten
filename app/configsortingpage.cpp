/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configsortingpage.h"

#include <QCheckBox>
#include <QListWidget>
#include <QStringList>

#include "dictionarymanager.h"

#include "kitenconfig.h"

ConfigSortingPage::ConfigSortingPage(  QWidget *parent
                                     , KitenConfigSkeleton *config
                                     ,Qt::WindowFlags f )
: QWidget( parent, f )
, _config( config )
{
  setupUi( this );
  _dictNames = DictionaryManager::listDictFileTypes();

  //Setup the relationship between the checkbox and the dictionary sortlist
  connect(kcfg_dictionary_enable, &QCheckBox::clicked, dictionary_order, &KActionSelector::setEnabled);
  dictionary_order->setEnabled( _config->dictionary_enable() == QLatin1String("true") );

  _fields.append( QStringLiteral("Word/Kanji") );
  _fields.append( QStringLiteral("Meaning") );
  _fields.append( QStringLiteral("Reading") );
  QList<QString> fieldListMap = DictionaryManager::generateExtendedFieldsList().keys();
  _fields += fieldListMap;

  //Make the connections to alert the main dialog when things change
  connect(dictionary_order, &KActionSelector::added, this, &ConfigSortingPage::widgetChanged);
  connect(dictionary_order, &KActionSelector::removed, this, &ConfigSortingPage::widgetChanged);
  connect(dictionary_order, &KActionSelector::movedUp, this, &ConfigSortingPage::widgetChanged);
  connect(dictionary_order, &KActionSelector::movedDown, this, &ConfigSortingPage::widgetChanged);

  connect(field_order, &KActionSelector::added, this, &ConfigSortingPage::widgetChanged);
  connect(field_order, &KActionSelector::removed, this, &ConfigSortingPage::widgetChanged);
  connect(field_order, &KActionSelector::movedUp, this, &ConfigSortingPage::widgetChanged);
  connect(field_order, &KActionSelector::movedDown, this, &ConfigSortingPage::widgetChanged);
}

//Read from preferences and set widgets
void ConfigSortingPage::updateWidgets()
{
  QStringList selectedDicts  = _config->dictionary_sortlist();
  QStringList selectedFields = _config->field_sortlist();

  QStringList availDicts  = _dictNames;
  QStringList availFields = _fields;

  foreach( const QString &dict, selectedDicts )
  {
    availDicts.removeAll( dict );
  }

  foreach( const QString &field, selectedFields )
  {
    availDicts.removeAll( field );
  }

  dictionary_order->availableListWidget()->clear();
  dictionary_order->availableListWidget()->addItems(availDicts);
  dictionary_order->selectedListWidget()->clear();
  dictionary_order->selectedListWidget()->addItems(selectedDicts);

  field_order->availableListWidget()->clear();
  field_order->availableListWidget()->addItems(availFields);
  field_order->selectedListWidget()->clear();
  field_order->selectedListWidget()->addItems(selectedFields);
}

void ConfigSortingPage::updateSettings()
{
  QStringList list;
  for( int i = 0; i < dictionary_order->selectedListWidget()->count(); i++ )
  {
    list.append( dictionary_order->selectedListWidget()->item( i )->text() );
  }
  _config->setDictionary_sortlist( list );

  list.clear();
  for( int i = 0; i < field_order->selectedListWidget()->count(); i++ )
  {
    list.append( field_order->selectedListWidget()->item( i )->text() );
  }
  _config->setField_sortlist( list );
}

void ConfigSortingPage::updateWidgetsDefault()
{
  //No Default?
}

bool ConfigSortingPage::isDefault()
{
  return true;
}

bool ConfigSortingPage::hasChanged()
{
  return false;
}


