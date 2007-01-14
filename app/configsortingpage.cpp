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

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtGui/QCheckBox>
#include <QtGui/QListWidget>

#include "dictionary.h"

#include "kitenconfig.h"
#include "configsortingpage.h"

ConfigSortingPage::ConfigSortingPage
	(QWidget *parent,KitenConfigSkeleton *iconfig,Qt::WFlags f) {

	setupUi(this);
	config = iconfig;

	//Setup the relationship between the checkbox and the dictionary sortlist
	connect(kcfg_dictionary_enable, SIGNAL(clicked(bool)),
			dictionary_order,SLOT(setEnabled(bool)));
	dictionary_order->setEnabled(iconfig->dictionary_enable()=="true");

	dictNames = dictionary::listDictFileTypes();

	fields.append("Word/Kanji");
	fields.append("Meaning");
	fields.append("Reading");

	foreach(const QString &dict, dictNames) {
		dictFile *tempDict = dictionary::makeDictFile(dict);
		QStringList newestFields = tempDict->getSearchableAttributes().keys();

		foreach(const QString &field, newestFields)
			if(!fields.contains(field)) //No duplicates
				fields.append(field);

		delete tempDict;
	}

	//Make the connections to alert the main dialog when things change
	connect(dictionary_order,SIGNAL(added(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(dictionary_order,SIGNAL(removed(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(dictionary_order,SIGNAL(movedUp(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(dictionary_order,SIGNAL(movedDown(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(field_order,SIGNAL(added(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(field_order,SIGNAL(removed(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(field_order,SIGNAL(movedUp(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
	connect(field_order,SIGNAL(movedDown(QListWidgetItem*)),
			this,SIGNAL(widgetChanged()));
}


void ConfigSortingPage::updateWidgets()
//Read from preferences and set widgets
{
	QStringList selectedDicts = config->dictionary_sortlist();
	QStringList selectedFields = config->field_sortlist();

	QStringList availDicts = dictNames;
	QStringList availFields = fields;
	foreach(const QString &dict, selectedDicts)
		availDicts.removeAll(dict);
	foreach(const QString &field, selectedFields)
		availDicts.removeAll(field);

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
	for(int i=0; i< dictionary_order->selectedListWidget()->count();i++)
		list.append(dictionary_order->selectedListWidget()->item(i)->text());
	config->setDictionary_sortlist(list);

	list.clear();
	for(int i=0; i< field_order->selectedListWidget()->count();i++)
		list.append(field_order->selectedListWidget()->item(i)->text());
	config->setField_sortlist(list);
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

#include "configsortingpage.moc"

