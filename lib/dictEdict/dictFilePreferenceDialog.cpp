/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dictFilePreferenceDialog.h"

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QLayout>
#include <QtGui/QFrame>
#include <QtCore/QStringList>
#include <QtCore/QString>

#include <kactionselector.h>
#include <kconfigskeleton.h>
#include <klocale.h>

dictFileFieldSelector::dictFileFieldSelector(KConfigSkeleton *iconfig, 
		const QString &dictionaryTypeName, QWidget *parent) :
	DictionaryPreferenceDialog(parent,dictionaryTypeName), dictName(dictionaryTypeName) {

	QVBoxLayout *newTabLayout = new QVBoxLayout();

	//Make selection box
	ListView = new KActionSelector();
	ListView->setAvailableLabel(i18n("&Available Fields:"));
	newTabLayout->addWidget(ListView);

	//Add layout to our widget
	this->setLayout(newTabLayout);

	//Create Default List
	completeList.append("--NewLine--");
	completeList.append("--NewLine--");
	completeList.append("--NewLine--");
	completeList.append("Word/Kanji");
	completeList.append("Reading");
	completeList.append("Meaning");

	//Make connections
	connect(ListView, SIGNAL(added(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(ListView, SIGNAL(removed(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(ListView, SIGNAL(movedUp(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(ListView, SIGNAL(movedDown(QListWidgetItem*)), this, SLOT(settingChanged()));

	config = iconfig;
	updateWidgets();
}

dictFileFieldSelector::~dictFileFieldSelector() {}

void dictFileFieldSelector::setAvailable(const QStringList &list) {
	completeList = list;
	updateWidgets();
}

void dictFileFieldSelector::addAvailable(const QStringList &list) {
	completeList += list;
	updateWidgets();
}

void dictFileFieldSelector::setDefaultList(const QStringList &list) {
	defaultList = list;
}

void dictFileFieldSelector::updateWidgets() {
	readFromPrefs();
}
void dictFileFieldSelector::updateWidgetsDefault() {
	
}

void dictFileFieldSelector::updateSettings() {
	writeToPrefs();
}

void dictFileFieldSelector::settingChanged() {
	emit widgetChanged();
}

void dictFileFieldSelector::readFromPrefs() {
	QStringList selectedList;

	config->setCurrentGroup("dicts_"+dictName);

	QStringList actionList = completeList;
	QString itemName = dictName + "__displayFields";
	KConfigSkeletonItem *item = config->findItem(itemName);
	if(item != NULL) {
		selectedList = item->property().toStringList();
	} else {	//it's not currently in the preferences list
		config->addItem(new KConfigSkeleton::ItemStringList(
					"dicts_"+dictName,itemName,*new QStringList()),itemName);
		config->readConfig();
		selectedList = config->findItem(itemName)->property().toStringList();
	}

	foreach( const QString &it, selectedList)
		actionList.removeAt(actionList.indexOf(it)); //don't just use remove()... will remove all

	ListView->availableListWidget()->clear();
	ListView->selectedListWidget()->clear();
	ListView->availableListWidget()->addItems(actionList);
	ListView->selectedListWidget()->addItems(selectedList);
}

void dictFileFieldSelector::writeToPrefs() {
	config->setCurrentGroup("dicts_"+dictName);
	QStringList theList;
	KConfigSkeletonItem *item;
	QString itemName;

	for(int i=0; i < ListView->selectedListWidget()->count(); i++)
		theList.append(ListView->selectedListWidget()->item(i)->text());

	itemName = dictName+"__displayFields";
	item = config->findItem(itemName);
	if(!item) {
		item = new KConfigSkeleton::ItemStringList("dicts_"+dictName,itemName,*new QStringList());
		config->addItem(item,itemName);
	}
	item->setProperty(theList);

	config->writeConfig();
}

#include "dictFilePreferenceDialog.moc"
