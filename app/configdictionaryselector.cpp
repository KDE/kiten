/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configdictionaryselector.h"

#include <KConfigSkeleton>

#include <QFileDialog>
#include <QStringList>

ConfigDictionarySelector::ConfigDictionarySelector( const QString &dictionaryName,
  QWidget *parent, KConfigSkeleton *config,Qt::WindowFlags f )
: QWidget( parent, f )
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
  QString groupName = "dicts_" + _dictName;
  KConfigGroup group = _config->config()->group( groupName );
  QStringList names = group.readEntry( "__NAMES", QStringList() );

  fileList->clear();

  foreach( const QString &it, names )
  {
    QStringList newRow( it );
    newRow << group.readEntry( it, QString() );
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


