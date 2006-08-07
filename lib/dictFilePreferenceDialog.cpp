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

#include <qwidget.h>
#include <QVBoxLayout>
#include <qlistwidget.h>
#include <kactionselector.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qframe.h>
#include <kconfigskeleton.h>
#include <klocale.h>

#include "dictFilePreferenceDialog.h"

dictFileFieldSelector::dictFileFieldSelector(KConfigSkeleton *iconfig, 
		const QString &dictionaryTypeName, 
		QWidget *parent, const char *name): DictionaryPreferenceDialog(parent,name) {

	dictName = dictionaryTypeName;

	QVBoxLayout *newTabLayout = new QVBoxLayout();

	//Make top selection box	
	ListView = new KActionSelector();
	ListView->setAvailableLabel(i18n("&Available List Fields:"));
	newTabLayout->addWidget(ListView);

	//Make Seperator Line
	QFrame *seperatorLine = new QFrame(this);
	seperatorLine->setFrameShape(QFrame::HLine);
	seperatorLine->setFrameShadow(QFrame::Sunken);
	newTabLayout->addWidget(seperatorLine);

	//Make Bottom Selection Box
	FullView = new KActionSelector();
	FullView->setAvailableLabel(i18n("&Available Full View Fields:"));
	newTabLayout->addWidget(FullView);

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
	connect(FullView, SIGNAL(added(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(FullView, SIGNAL(removed(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(FullView, SIGNAL(movedUp(QListWidgetItem*)), this, SLOT(settingChanged()));
	connect(FullView, SIGNAL(movedDown(QListWidgetItem*)), this, SLOT(settingChanged()));
	
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

void dictFileFieldSelector::setDefaultFull(const QStringList &list) {
	defaultFull = list;
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
	QStringList actionList, selectedList;

	config->setCurrentGroup("dicts_"+dictName);
	
	actionList = completeList;
	selectedList = fetchItemFromPreferences(dictName, QString("List"));
	foreach( QString it, selectedList)
		actionList.removeAt(actionList.indexOf(it)); //don't just use remove()... will remove all

	ListView->availableListWidget()->clear();
	ListView->selectedListWidget()->clear();
	ListView->availableListWidget()->addItems(actionList);
	ListView->selectedListWidget()->addItems(selectedList);
	
	actionList = completeList;
	selectedList = fetchItemFromPreferences(dictName, QString("Full"));
	for(QStringList::Iterator it = selectedList.begin(); it != selectedList.end(); ++it)
		actionList.removeAt(actionList.indexOf(*it)); 

	FullView->availableListWidget()->clear();
	FullView->selectedListWidget()->clear();
	FullView->availableListWidget()->addItems(actionList);
	FullView->selectedListWidget()->addItems(selectedList);
}

void dictFileFieldSelector::writeToPrefs() {
	config->setCurrentGroup("dicts_"+dictName);
	QStringList theList;
	KConfigSkeletonItem *item;
	QString itemName;
	
	theList = extractList(ListView);
	itemName = dictName+"__displayFieldsListView";
	item = config->findItem(itemName);
	if(!item) {
		item = new KConfigSkeleton::ItemStringList("dicts_"+dictName,itemName,*new QStringList());
		config->addItem(item,itemName);
	}
	item->setProperty(theList);

	theList = extractList(FullView);
	itemName = dictName+"__displayFieldsFullView";
	item = config->findItem(itemName);
	if(!item) {
		item = new KConfigSkeleton::ItemStringList("dicts_"+dictName,itemName,*new QStringList());
		config->addItem(item,itemName);
	}
	item->setProperty(theList);
	
	config->writeConfig();
}

QStringList dictFileFieldSelector::extractList(KActionSelector *box) {
	QStringList result;
	for(int i=0; i < box->selectedListWidget()->count(); i++)
		result.append(box->selectedListWidget()->item(i)->text());
	return result;
}

QStringList
dictFileFieldSelector::fetchItemFromPreferences(const QString &dictName, const QString &type) {
	QString itemName = dictName + "__displayFields"+type+"View";
	KConfigSkeletonItem *item = config->findItem(itemName);
	if(item != NULL)
		return item->property().toStringList();

	//else it's not currently in the preferences list
	config->addItem(new KConfigSkeleton::ItemStringList(
				"dicts_"+dictName,itemName,*new QStringList()),itemName);
	config->readConfig();
	return config->findItem(itemName)->property().toStringList();
}
#include "dictFilePreferenceDialog.moc"
