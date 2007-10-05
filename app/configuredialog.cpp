/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	        (C) 2005 Paul Temple <paul.temple@gmx.net>
			  (C) 2006 Joseph Kerian <jkerian@gmail.com>

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

#include "configuredialog.h"

#include <klocale.h>
#include <kconfigdialog.h>

#include <QtGui/QLayout>
#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtCore/QStringList>
#include <QtCore/QString>

#include "kitenconfig.h"

//Generated from UI files
#include "ui_configfont.h"
#include "ui_configlearn.h"
#include "ui_configsearching.h"

//Our template for managing individual dict type's settings
#include "DictionaryPreferenceDialog.h"

//The dictionary file selector widget
#include "configdictionaryselector.h"
//The sorting selection widget
#include "configsortingpage.h"
//To get the list of dictionary Types and to interface with dictType objects
#include "DictionaryManager.h"


ConfigureDialog::ConfigureDialog(QWidget *parent, KitenConfigSkeleton *config )
	: KConfigDialog(parent, "Settings", config)
{
	//TODO: Figure out why these pages are unmanaged... is this needed?
	addPage(makeDictionaryFileSelectionPage(NULL,config),i18n("Dictionaries"),"help-contents");

	QWidget *widget;
	widget = new QWidget();
	Ui::ConfigSearching cs;
	cs.setupUi(widget);
	addPage(widget, i18n("Searching"), "edit-find");

	widget = new QWidget();
	Ui::ConfigLearn cl; cl.setupUi(widget);
	addPage(widget, i18n("Learn"), "pencil");

	widget = new QWidget();
	Ui::ConfigFont cf;
	cf.setupUi(widget);
	addPage(widget, i18n("Font"), "fonts");

	addPage(makeDictionaryPreferencesPage(NULL,config),i18nc("@title:group the settings for the dictionary display", "Display"),"format-indent-more");

	addPage(makeSortingPage(NULL,config),i18n("Results Sorting"), "format-indent-more");

	connect(this,SIGNAL(settingsChanged(const QString&)),this,SIGNAL(settingsChanged()));
}

ConfigureDialog::~ConfigureDialog()
{
}

QWidget *ConfigureDialog::makeDictionaryFileSelectionPage(QWidget *parent,
		KitenConfigSkeleton *config) {

	QTabWidget *tabWidget = new QTabWidget(parent);

	foreach( const QString &dict, config->dictionary_list() ) {
		QWidget *newTab = new ConfigDictionarySelector(dict,tabWidget,config);
		connect(newTab, SIGNAL(widgetChanged()), this, SLOT(updateButtons()));
		connect(this, SIGNAL(SIG_updateWidgets()), newTab, SLOT(updateWidgets()));
		connect(this, SIGNAL(SIG_updateWidgetsDefault()), newTab, SLOT(updateWidgetsDefault()));
		connect(this, SIGNAL(SIG_updateSettings()), newTab, SLOT(updateSettings()));
		tabWidget->addTab(newTab, '&'+dict);
	}

	return tabWidget;
}

QWidget *ConfigureDialog::makeDictionaryPreferencesPage
	(QWidget *parent, KitenConfigSkeleton *config) {

	QStringList dictTypes = DictionaryManager::listDictFileTypes();

	QTabWidget *tabWidget = new QTabWidget(parent);

	QMap<QString,DictionaryPreferenceDialog*> dialogList =
			DictionaryManager::generatePreferenceDialogs(config,parent);
	foreach( DictionaryPreferenceDialog *dialog, dialogList ) {
		connect(this, SIGNAL(SIG_updateWidgets()), dialog, SLOT(updateWidgets()));
		connect(this, SIGNAL(SIG_updateWidgetsDefault()), dialog, SLOT(updateWidgetsDefault()));
		connect(this, SIGNAL(SIG_updateSettings()), dialog, SLOT(updateSettings()));

		tabWidget->addTab(dialog,dialog->name());
	}
	return tabWidget;
}

QWidget *ConfigureDialog::makeSortingPage
	(QWidget *parent, KitenConfigSkeleton *config) {
	ConfigSortingPage *newPage = new ConfigSortingPage(parent,config);

	connect(newPage, SIGNAL(widgetChanged()), this, SLOT(updateButtons()));
	connect(this, SIGNAL(SIG_updateWidgets()), newPage, SLOT(updateWidgets()));
	connect(this, SIGNAL(SIG_updateWidgetsDefault()), newPage, SLOT(updateWidgetsDefault()));
	connect(this, SIGNAL(SIG_updateSettings()), newPage, SLOT(updateSettings()));

	return newPage;
}

void ConfigureDialog::updateWidgets()
{
	emit SIG_updateWidgets();
}

void ConfigureDialog::updateWidgetsDefault()
{
	emit SIG_updateWidgetsDefault();
}

void ConfigureDialog::updateSettings()
{
	emit SIG_updateSettings();
}

bool ConfigureDialog::isDefault()
{
	 return false;
	 //Always show the defaults button.... perhaps make a workaround later
}

#include "configuredialog.moc"
