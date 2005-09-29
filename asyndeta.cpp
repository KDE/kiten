/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

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

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <qfile.h>

#include "asyndeta.h"
#include "dict.h"

int Asyndeta::refCount = 0;
Dict::Index *Asyndeta::index = 0;

Asyndeta::Asyndeta()
{
	refCount++;

	if (!index)
	{
		// create index
		index = new Dict::Index;
	}
}

Asyndeta::~Asyndeta()
{
	refCount--;
	if (refCount == 0)
	{
		delete index;
		index = 0;
	}
}

Dict::Index *Asyndeta::retrieveIndex()
{
	return index;
}

void Asyndeta::readKitenConfiguration()
{
	KStandardDirs *dirs = KGlobal::dirs();
	KConfig config(dirs->findResource("config", "kitenrc"));

	QString globaledict = dirs->findResource("data", "kiten/edict");
	QString globalkanjidic = dirs->findResource("data", "kiten/kanjidic");

	config.setGroup("edict");

	bool edictUseGlobal = config.readBoolEntry("__useGlobal", true);

	QStringList DictNameList = config.readListEntry("__NAMES");
	QStringList DictList;

	QStringList::Iterator it;

	for (it = DictNameList.begin(); it != DictNameList.end(); ++it)
		DictList.append(config.readEntry(*it));

	QString personalDict(personalDictionaryLocation());
	if (QFile::exists(personalDict))
	{
		DictList.prepend(personalDict);
		DictNameList.prepend(i18n("Personal"));
	}

	if (!globaledict.isNull() && edictUseGlobal)
	{
		DictList.prepend(globaledict);
		DictNameList.prepend("Edict");
	}

	index->setDictList(DictList, DictNameList);

	config.setGroup("kanjidic");

	bool kanjidicUseGlobal = config.readBoolEntry("__useGlobal", true);

	DictList.clear();
	DictNameList = config.readListEntry("__NAMES");

	for (it = DictNameList.begin(); it != DictNameList.end(); ++it)
		DictList.append(config.readEntry(*it));

	if (!globalkanjidic.isNull() && kanjidicUseGlobal)
	{
		DictList.prepend(globalkanjidic);
		DictNameList.prepend("Kanjidic");
	}

	index->setKanjiDictList(DictList, DictNameList);
}

QString Asyndeta::personalDictionaryLocation()
{
	return KGlobal::dirs()->saveLocation("data", "kiten/dictionaries/", true).append("personal");
}

