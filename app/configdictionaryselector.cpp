/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>
           (C) 2006 Joseph Kerian <jkerian@gmail.com>
			  (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

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

#include "configdictionaryselector.h"

#include <QtCore/QStringList>

#include <QtGui/QTableWidget>

#include <kdebug.h>
#include <kfiledialog.h>
#include <kconfigskeleton.h>


ConfigDictionarySelector::ConfigDictionarySelector(const QString &dictionaryName, 
	QWidget *parent,KConfigSkeleton *iconfig,Qt::WFlags f)
{
	setupUi(this);
	dictName = dictionaryName;
	config = iconfig;

	connect(addButton,SIGNAL(clicked()), this, SLOT(addDictSLOT()));
	connect(delButton,SIGNAL(clicked()), this, SLOT(deleteDictSLOT()));
	__useGlobal->setObjectName(QString("kcfg_"+dictName+"__useGlobal"));
}

void ConfigDictionarySelector::updateWidgets()
//Read from preferences to the active list
{
	QStringList names;

	config->setCurrentGroup("dicts_"+dictName);
	KConfigSkeletonItem *item = config->findItem(dictName+"__NAMES");
	if(item != NULL)
		names = item->property().toStringList();

	foreach(const QString &it, names) {
		QString name = dictName + '_' + it;
		if (!config->findItem(name))
			config->addItem(new KConfigSkeleton::ItemString(dictName, it, *new QString()), name);
			//Don't touch the *new QString()... that's a reference for a reason... stupid KDE
	}
	config->readConfig();
	fileList->clear();
	foreach(const QString &it, names) {
		QStringList newRow(it);
		newRow << config->findItem(dictName+'_'+it)->property().toString();
		(void) new QTreeWidgetItem(fileList, newRow);
	}
}

void ConfigDictionarySelector::updateSettings()
{
	QStringList names;

	KConfigGroup group = config->config()->group("dicts_"+dictName.toLower());

	for(int i=0; i<fileList->topLevelItemCount(); i++) {
		QTreeWidgetItem *it = fileList->topLevelItem(i);
		QString dictionaryName = it->text(0);
		QString dictionaryPath = it->text(1);
		names.append(dictionaryName);

		if (!group.hasKey(dictionaryName))
		{
			KConfigSkeletonItem *item = new KConfigSkeleton::ItemPath(group.name(),
					dictionaryName,*new QString());
			config->addItem(item, dictionaryName);
		}
		group.writeEntry(dictionaryName,dictionaryPath);
	}

	//This feels distinctly hackish to me... :(
	config->findItem(dictName+"__NAMES")->setProperty(names);
	config->writeConfig();
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

void ConfigDictionarySelector::addDictSLOT() {
   QTreeWidgetItem *item = fileList->topLevelItem(0);

	QString filename = KFileDialog::getOpenFileName(
			item? QFileInfo(item->text(1)).absolutePath().append("/")
			: QString() );
	QString name = QFileInfo(filename).fileName();
	if(filename.isNull())
		return;

	QStringList newRow(name);
	newRow << filename;
	(void) new QTreeWidgetItem(fileList, newRow);
	emit widgetChanged();
}

void ConfigDictionarySelector::deleteDictSLOT() {
	foreach(QTreeWidgetItem *file, fileList->selectedItems()) {
		if (!file)
			return;
		delete file;
		emit widgetChanged();
	}
}

#include "configdictionaryselector.moc"

