/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#include "configuredialog.h"

#include <KConfigDialog>
#include <KLocale>
#include <KTabWidget>

#include <QLayout>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "kitenconfig.h"

//Generated from UI files
#include "ui_configfont.h"
#include "ui_configlearn.h"
#include "ui_configsearching.h"

//Our template for managing individual dict type's settings
#include "dictionarypreferencedialog.h"
//The dictionary file selector widget
#include "configdictionaryselector.h"
//The sorting selection widget
#include "configsortingpage.h"
//To get the list of dictionary Types and to interface with dictType objects
#include "dictionarymanager.h"

ConfigureDialog::ConfigureDialog( QWidget *parent, KitenConfigSkeleton *config )
: KConfigDialog( parent, "Settings", config )
{
  //TODO: Figure out why these pages are unmanaged... is this needed?
  addPage(  makeDictionaryFileSelectionPage( NULL, config )
          , i18n( "Dictionaries" )
          , "help-contents" );

  QWidget *widget;
  widget = new QWidget();
  Ui::ConfigSearching cs;
  cs.setupUi( widget );
  addPage( widget, i18n( "Searching" ), "edit-find" );

  widget = new QWidget();
  Ui::ConfigLearn cl; cl.setupUi( widget );
  addPage( widget, i18n( "Learn" ), "draw-freehand" );

  widget = new QWidget();
  Ui::ConfigFont cf;
  cf.setupUi( widget );
  addPage( widget, i18n( "Font" ), "preferences-desktop-font" );

  addPage(  makeDictionaryPreferencesPage( NULL, config )
          , i18nc( "@title:group the settings for the dictionary display", "Display" )
          , "format-indent-more" );
  addPage(  makeSortingPage( NULL, config )
          , i18n( "Results Sorting" )
          , "format-indent-more" );

  setHelp( QString(),"kiten" );

  connect( this, SIGNAL( settingsChanged( const QString& ) ),
           this, SIGNAL( settingsChanged() ) );
}

ConfigureDialog::~ConfigureDialog()
{
}

QWidget *ConfigureDialog::makeDictionaryFileSelectionPage(  QWidget *parent
                                                          , KitenConfigSkeleton *config )
{
  KTabWidget *tabWidget = new KTabWidget( parent );

  foreach( const QString &dict, config->dictionary_list() )
  {
    QWidget *newTab = new ConfigDictionarySelector( dict, tabWidget, config );
    if( newTab )
    {
      connect( newTab, SIGNAL( widgetChanged() ),
                 this, SIGNAL( widgetModified() ) );
      connect(   this, SIGNAL( updateWidgetsSignal() ),
               newTab,   SLOT( updateWidgets() ) );
      connect(   this, SIGNAL( updateWidgetsDefaultSignal() ),
               newTab,   SLOT( updateWidgetsDefault() ) );
      connect(   this, SIGNAL( updateSettingsSignal() ),
               newTab,   SLOT( updateSettings() ) );
      tabWidget->addTab( newTab, dict );
    }
  }

  return tabWidget;
}

QWidget *ConfigureDialog::makeDictionaryPreferencesPage(  QWidget *parent
                                                        , KitenConfigSkeleton *config )
{

  QStringList dictTypes = DictionaryManager::listDictFileTypes();

  KTabWidget *tabWidget = new KTabWidget( parent );

  QMap<QString,DictionaryPreferenceDialog*> dialogList =
                  DictionaryManager::generatePreferenceDialogs( config, parent );

  foreach( DictionaryPreferenceDialog *dialog, dialogList )
  {
    connect(   this, SIGNAL( updateWidgetsSignal() ),
             dialog,   SLOT( updateWidgets() ) );
    connect(   this, SIGNAL( updateWidgetsDefaultSignal() ),
             dialog,   SLOT( updateWidgetsDefault() ) );
    connect(   this, SIGNAL( updateSettingsSignal() ),
             dialog,   SLOT( updateSettings() ) );

    tabWidget->addTab( dialog,dialog->name() );
  }

  return tabWidget;
}

QWidget *ConfigureDialog::makeSortingPage( QWidget *parent, KitenConfigSkeleton *config )
{
  ConfigSortingPage *newPage = new ConfigSortingPage(parent,config);

  connect( newPage, SIGNAL( widgetChanged() ),
              this, SIGNAL( widgetModified() ) );

  connect(    this, SIGNAL( updateWidgetsSignal() ),
           newPage,   SLOT( updateWidgets() ) );
  connect(    this, SIGNAL( updateWidgetsDefaultSignal() ),
           newPage,   SLOT( updateWidgetsDefault() ) );
  connect(    this, SIGNAL( updateSettingsSignal() ),
           newPage,   SLOT( updateSettings() ) );

  return newPage;
}

void ConfigureDialog::updateWidgets()
{
  emit updateWidgetsSignal();
}

void ConfigureDialog::updateWidgetsDefault()
{
  emit updateWidgetsDefaultSignal();
}

void ConfigureDialog::updateSettings()
{
  emit updateSettingsSignal();
}

bool ConfigureDialog::isDefault()
{
  return false;
  //Always show the defaults button.... perhaps make a workaround later
}

#include "configuredialog.moc"
