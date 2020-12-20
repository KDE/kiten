/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configuredialog.h"

#include <KLocalizedString>

#include <QTabWidget>
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
: KConfigDialog( parent, QStringLiteral("Settings"), config )
{
  //TODO: Figure out why these pages are unmanaged... is this needed?
  addPage(  makeDictionaryFileSelectionPage( NULL, config )
          , i18n( "Dictionaries" )
          , QStringLiteral("help-contents") );

  QWidget *widget;
  widget = new QWidget();
  Ui::ConfigSearching cs;
  cs.setupUi( widget );
  addPage( widget, i18n( "Searching" ), QStringLiteral("edit-find") );

  widget = new QWidget();
  Ui::ConfigLearn cl; cl.setupUi( widget );
  addPage( widget, i18n( "Learn" ), QStringLiteral("draw-freehand") );

  widget = new QWidget();
  Ui::ConfigFont cf;
  cf.setupUi( widget );
  addPage( widget, i18n( "Font" ), QStringLiteral("preferences-desktop-font") );

  addPage(  makeDictionaryPreferencesPage( NULL, config )
          , i18nc( "@title:group the settings for the dictionary display", "Display" )
          , QStringLiteral("format-indent-more") );
  addPage(  makeSortingPage( NULL, config )
          , i18n( "Results Sorting" )
          , QStringLiteral("format-indent-more") );

  setHelp( QString(),QStringLiteral("kiten") );

  connect( this, &KConfigDialog::widgetModified,
           this,   &ConfigureDialog::updateConfiguration );
}

void ConfigureDialog::updateConfiguration()
{
  emit settingsChanged( QString() );
}

ConfigureDialog::~ConfigureDialog()
{
}

QWidget *ConfigureDialog::makeDictionaryFileSelectionPage(  QWidget *parent
                                                          , KitenConfigSkeleton *config )
{
  QTabWidget *tabWidget = new QTabWidget( parent );

  foreach( const QString &dict, config->dictionary_list() )
  {
    QWidget *newTab = new ConfigDictionarySelector( dict, tabWidget, config );
    if( newTab )
    {
      connect( newTab, SIGNAL(widgetChanged()),
                 this, SIGNAL(widgetModified()) );
      connect(   this, SIGNAL(updateWidgetsSignal()),
               newTab,   SLOT(updateWidgets()) );
      connect(   this, SIGNAL(updateWidgetsDefaultSignal()),
               newTab,   SLOT(updateWidgetsDefault()) );
      connect(   this, SIGNAL(updateSettingsSignal()),
               newTab,   SLOT(updateSettings()) );
      tabWidget->addTab( newTab, dict );
    }
  }

  return tabWidget;
}

QWidget *ConfigureDialog::makeDictionaryPreferencesPage(  QWidget *parent
                                                        , KitenConfigSkeleton *config )
{

  QStringList dictTypes = DictionaryManager::listDictFileTypes();

  QTabWidget *tabWidget = new QTabWidget( parent );

  QMap<QString,DictionaryPreferenceDialog*> dialogList =
                  DictionaryManager::generatePreferenceDialogs( config, parent );

  foreach( DictionaryPreferenceDialog *dialog, dialogList )
  {
    connect(   this, &ConfigureDialog::updateWidgetsSignal,
             dialog,   &DictionaryPreferenceDialog::updateWidgets );
    connect(   this, &ConfigureDialog::updateWidgetsDefaultSignal,
             dialog,   &DictionaryPreferenceDialog::updateWidgetsDefault );
    connect(   this, &ConfigureDialog::updateSettingsSignal,
             dialog,   &DictionaryPreferenceDialog::updateSettings );

    tabWidget->addTab( dialog,dialog->name() );
  }

  return tabWidget;
}

QWidget *ConfigureDialog::makeSortingPage( QWidget *parent, KitenConfigSkeleton *config )
{
  ConfigSortingPage *newPage = new ConfigSortingPage(parent,config);

  connect( newPage, &ConfigSortingPage::widgetChanged,
              this, &KConfigDialog::widgetModified );

  connect(    this, &ConfigureDialog::updateWidgetsSignal,
           newPage,   &ConfigSortingPage::updateWidgets );
  connect(    this, &ConfigureDialog::updateWidgetsDefaultSignal,
           newPage,   &ConfigSortingPage::updateWidgetsDefault );
  connect(    this, &ConfigureDialog::updateSettingsSignal,
           newPage,   &ConfigSortingPage::updateSettings );

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


