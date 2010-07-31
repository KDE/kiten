/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>

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

#include <kdebug.h>
#include <klistview.h>
#include <kfiledialog.h>
#include <tqpushbutton.h>
#include <tqstringlist.h>

#include "configdictionaries.moc"

ConfigDictionaries::ConfigDictionaries(TQWidget *parent, const char* name, WFlags f) :
	ConfigDictionariesBase(parent, name, f)
{
	changed = false;
	config = Config::self();
	connect(delSelEdictButton, TQT_SIGNAL(clicked()), TQT_SLOT(slotDelSelEdict()));
	connect(addEdictButton, TQT_SIGNAL(clicked()), TQT_SLOT(slotAddEdict()));
	connect(delSelKanjidicButton, TQT_SIGNAL(clicked()), TQT_SLOT(slotDelSelKanjidic()));
	connect(addKanjidicButton, TQT_SIGNAL(clicked()), TQT_SLOT(slotAddKanjidic()));
}

void ConfigDictionaries::updateWidgets()
{
	readDictionaries();
}
void ConfigDictionaries::updateWidgetsDefault()
{
	// no default for custom edict list or
	// should we really delete all items in the list?
}
void ConfigDictionaries::updateSettings()
{
	writeDictionaries();
}
bool ConfigDictionaries::hasChanged()
{
	return changed;
}
bool ConfigDictionaries::isDefault()
{
	// no default for custom edict list or
	// should we really delete all items in the list?
	return true;
}

void ConfigDictionaries::readDictionaries()
{
	readDictionaryList("edict");
	readDictionaryList("kanjidic");
}

void ConfigDictionaries::writeDictionaries()
{
	writeDictionaryList("edict");
	writeDictionaryList("kanjidic");
	changed = false;
}

void ConfigDictionaries::readDictionaryList(const TQString& group)
{
	KListView* list;
	TQStringList names;
	if (group == "edict")
	{
		list = edictList;
		names = config->edict__NAMES();
	}
	else //if (group == "kanjidic")
	{
		list = kanjidicList;
		names = config->kanjidic__NAMES();
	}

	TQStringList::Iterator it;
	for (it = names.begin(); it != names.end(); ++it)
	{
		TQString name = group + "_" + *it;
		if (!config->findItem(name))
			config->addItem(new KConfigSkeleton::ItemString(group, *it, *new TQString()), name);
	}
	config->readConfig();
	list->clear();
	for (it = names.begin(); it != names.end(); ++it)
		(void) new TQListViewItem(list, *it, config->findItem(group + "_" + *it)->property().asString());
}

void ConfigDictionaries::writeDictionaryList(const TQString& group)
{
	KListView* list;
	if (group == "edict")
		list = edictList;
	else //if (group == "kanjidic")
		list = kanjidicList;
	
	TQStringList names;

	TQListViewItemIterator it(list);
	for (; it.current(); ++it )
	{
		names.append(it.current()->text(0));
		TQString name = group + "_" + it.current()->text(0);
		KConfigSkeletonItem* item = config->findItem(name);
		if (!item)
		{
			item = new KConfigSkeleton::ItemString(group, it.current()->text(0), *new TQString());
			config->addItem(item, name);
		}
		item->setProperty(it.current()->text(1));
	}

	if (group == "edict")
		config->setEdict__NAMES(names);
	else //if (group == "kanjidic")
		config->setKanjidic__NAMES(names);
	config->writeConfig();
}

void ConfigDictionaries::slotAddEdict() { add(edictList); }
void ConfigDictionaries::slotDelSelEdict() { delSel(edictList); }
void ConfigDictionaries::slotAddKanjidic() { add(kanjidicList); }
void ConfigDictionaries::slotDelSelKanjidic() { delSel(kanjidicList); }

void ConfigDictionaries::add(KListView* list)
{
	TQListViewItem *item = list->firstChild();
	TQString filename = KFileDialog::getOpenFileName(item? TQFileInfo(item->text(1)).dirPath(true).append("/") : TQString::null);
	TQString name = TQFileInfo(filename).fileName();

	(void) new TQListViewItem(list, name, filename);
	changed = true;
	emit widgetChanged();
}

void ConfigDictionaries::delSel(KListView* list)
{
	TQListViewItem *file = list->selectedItem();
	if (!file)
		return;

	delete file;
	changed = true;
	emit widgetChanged();
}
