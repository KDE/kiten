/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
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

#include "configdictionaryselector.h"

#include <KConfigSkeleton>

#include <QFileDialog>
#include <QStringList>

ConfigDictionarySelector::ConfigDictionarySelector( const QString &dictionaryName,
  QWidget *parent, KConfigSkeleton *config,Qt::WindowFlags f )
{
  setupUi( this );
  _dictName = dictionaryName;
  _config = config;

  connect(addButton, &QPushButton::clicked, this, &ConfigDictionarySelector::addDictSlot);
  connect(delButton, &QPushButton::clicked, this, &ConfigDictionarySelector::deleteDictSlot);
  __useGlobal->setObjectName( QString( "kcfg_" + _dictName + "__useGlobal" ) );
}

//Read from preferences to the active list
void ConfigDictionarySelector::updateWidgets()
{
  QStringList names;

  _config->setCurrentGroup( "dicts_" + _dictName );
  KConfigSkeletonItem *item = _config->findItem( _dictName + "__NAMES" );
  if( item != nullptr )
  {
    names = item->property().toStringList();
  }

  foreach( const QString &it, names )
  {
    QString name = _dictName + '_' + it;
    if ( ! _config->findItem( name ) )
    {
      _config->addItem( new KConfigSkeleton::ItemString( _dictName, it, *new QString() ), name );
      //Don't touch the *new QString()... that's a reference for a reason... stupid KDE
    }
  }

  _config->load();
  fileList->clear();

  foreach( const QString &it, names )
  {
    QStringList newRow( it );
    newRow << _config->findItem( _dictName + '_' + it )->property().toString();
    (void) new QTreeWidgetItem( fileList, newRow );
  }
}

void ConfigDictionarySelector::updateSettings()
{
  QStringList names;

  KConfigGroup group = _config->config()->group( "dicts_" + _dictName.toLower() );

  for( int i = 0; i < fileList->topLevelItemCount(); i++ )
  {
    QTreeWidgetItem *it = fileList->topLevelItem( i );
    QString dictionaryName = it->text( 0 );
    QString dictionaryPath = it->text( 1 );
    names.append( dictionaryName );

    if ( ! group.hasKey( dictionaryName ) )
    {
      KConfigSkeletonItem *item = new KConfigSkeleton::ItemPath( group.name()
                                                , dictionaryName, *new QString() );
      _config->addItem( item, dictionaryName );
    }
    group.writeEntry( dictionaryName, dictionaryPath );
  }

  //This feels distinctly hackish to me... :(
  _config->findItem( _dictName + "__NAMES" )->setProperty( names );
  _config->save();
}

void ConfigDictionarySelector::updateWidgetsDefault()
{
  // no default for custom edict list or
  // should we really delete all items in the list?
}

bool ConfigDictionarySelector::isDefault()
{
  // no default for custom edict list or
  // should we really delete all items in the list?
  return true;
}

bool ConfigDictionarySelector::hasChanged()
{
  return false;
}

void ConfigDictionarySelector::addDictSlot()
{
  QTreeWidgetItem *item = fileList->topLevelItem( 0 );

  QString filename = QFileDialog::getOpenFileName(nullptr, QString(),
                  item ? QFileInfo( item->text( 1 ) ).absolutePath().append( "/" )
                  : QString() );
  QString name = QFileInfo( filename ).fileName();
  if( filename.isNull() )
    return;

  QStringList newRow( name );
  newRow << filename;
  (void) new QTreeWidgetItem( fileList, newRow );

  updateSettings();
  emit widgetChanged();
}

void ConfigDictionarySelector::deleteDictSlot()
{
  foreach( QTreeWidgetItem *file, fileList->selectedItems() )
  {
    if ( ! file )
    {
      return;
    }

    delete file;

    updateSettings();
    emit widgetChanged();
  }
}


