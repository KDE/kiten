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

#include <klocale.h>
#include <kconfigdialog.h>

#include <qlayout.h>
#include <qwidget.h>
#include <qtabwidget.h>
#include <qstringlist.h>
#include <qstring.h>

#include "optiondialog.h"

//Generated from UI files
#include "ui_configfont.h"
#include "ui_configlearn.h"
#include "ui_configsearching.h"

//Our template for managing individual dict type's settings
#include "dictFilePreferenceDialog.h"

//The dictionary file selector widget
#include "configdictionaryselector.h"
//To get the list of dictionary Types and to interface with dictType objects
#include "dictionary.h"


ConfigureDialog::ConfigureDialog(QWidget *parent, const char *name) : KConfigDialog(parent, name, KitenConfigSkeleton::self())
{
	//TODO: Figure out why these pages are unmanaged... is this needed?
	addPage(makeDictionaryFileSelectionPage(0,"dictionaries_page"),i18n("Dictionaries"),"contents");

	QWidget *widget;
	widget = new QWidget();
	Ui::ConfigSearching cs;
	cs.setupUi(widget);
	addPage(widget, i18n("Searching"), "find");

	widget = new QWidget();
	Ui::ConfigLearn cl;
	cl.setupUi(widget);
	addPage(widget, i18n("Learn"), "pencil");
	
	widget = new QWidget();
	Ui::ConfigFont cf;
	cf.setupUi(widget);
	addPage(widget, i18n("Font"), "fonts");
	
	addPage(makeDictionaryPreferencesPage(0,"display_page"),i18n("Display"),"indent");
	hasChangedMarker = false;
}

ConfigureDialog::~ConfigureDialog()
{
}

QWidget *ConfigureDialog::makeDictionaryFileSelectionPage(QWidget *parent, const char *name) {
	
	QTabWidget *tabWidget = new QTabWidget(parent);
	tabWidget->setObjectName(QLatin1String(name));

	QStringList dictTypes = KitenConfigSkeleton::self()->dictionary_list();
	QStringList::Iterator dict, dictEnd = dictTypes.end();
	for(dict = dictTypes.begin(); dict != dictEnd; ++dict) {
		QWidget *newTab = new ConfigDictionarySelector(*dict,tabWidget,KitenConfigSkeleton::self());
		connect(newTab, SIGNAL(widgetChanged()), this, SLOT(updateButtons()));
		connect(this, SIGNAL(SIG_updateWidgets()), newTab, SLOT(updateWidgets()));
		connect(this, SIGNAL(SIG_updateWidgetsDefault()), newTab, SLOT(updateWidgetsDefault()));
		connect(this, SIGNAL(SIG_updateSettings()), newTab, SLOT(updateSettings()));
		tabWidget->addTab(newTab, "&"+*dict);
	}
	return tabWidget;
}

QWidget *ConfigureDialog::makeDictionaryPreferencesPage(QWidget *parent, const char *name) {
	QStringList dictTypes = dictionary::listDictFileTypes();
	
	QTabWidget *tabWidget = new QTabWidget(parent);

	QStringList::Iterator dict, dictEnd = dictTypes.end();
	for(dict = dictTypes.begin(); dict != dictEnd; ++dict) {
		dictFile *tempDict = dictionary::makeDictFile(*dict);
		
		DictionaryPreferenceDialog *newTab = tempDict->preferencesWidget(KitenConfigSkeleton::self(),parent, (*dict).toAscii());
		if(newTab == NULL) continue;

		connect(newTab, SIGNAL(widgetChanged()), this, SLOT(updateButtons()));
		connect(this, SIGNAL(SIG_updateWidgets()), newTab, SLOT(updateWidgets()));
		connect(this, SIGNAL(SIG_updateWidgetsDefault()), newTab, SLOT(updateWidgetsDefault()));
		connect(this, SIGNAL(SIG_updateSettings()), newTab, SLOT(updateSettings()));
		
		delete tempDict;
		tabWidget->addTab(newTab,*dict);
	}
	return tabWidget;
}

void ConfigureDialog::updateButtons() {
	hasChangedMarker = true;
	KConfigDialog::updateButtons();
}

void ConfigureDialog::updateWidgets()
{
	hasChangedMarker = false;
	emit SIG_updateWidgets();
}

void ConfigureDialog::updateWidgetsDefault()
{
	hasChangedMarker = false;
	emit SIG_updateWidgetsDefault();
}

void ConfigureDialog::updateSettings()
{
	emit SIG_updateSettings();
	if(hasChanged())
		KConfigDialog::settingsChangedSlot();

	hasChangedMarker = false;
}

bool ConfigureDialog::hasChanged()
{
	return hasChangedMarker;
}

bool ConfigureDialog::isDefault()
{
	 return false; 
	 //Always show the defaults button.... perhaps make a workaround later
}

#include "optiondialog.moc"
