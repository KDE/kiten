/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2006  Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

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

#ifndef KITEN_DICTFILEEDICT_H
#define KITEN_DICTFILEEDICT_H

#include <config-kiten.h>

#include "dictFile.h"
#include "indexedEDICTFile.h"
#include "linearEDICTFile.h"

#include <sys/types.h>
#include <QtCore/QFile>
#include <QtCore/QMap>

class QString;
class QByteArray;
class QStringList;

class DictQuery;
class DictionaryPreferenceDialog;
class KConfigSkeleton;
class KConfigSkeletonItem;

#include "entryEdict.h"

class /* NO_EXPORT */ dictFileEdict : public dictFile {
	public:
	dictFileEdict();
	virtual ~dictFileEdict();

	virtual bool validDictionaryFile(const QString &filename);
	bool validQuery(const DictQuery &query);

	bool loadDictionary(const QString &file, const QString &name);

	virtual EntryList *doSearch(const DictQuery &query);
	virtual QStringList listDictDisplayOptions(QStringList) const;

	virtual DictionaryPreferenceDialog *preferencesWidget(KConfigSkeleton*,QWidget *parent=NULL);

	virtual void loadSettings(KConfigSkeleton*);

	protected:
	virtual QMap<QString,QString> displayOptions() const;
	QStringList *loadListType(KConfigSkeletonItem *, QStringList *,
			const QMap<QString,QString> &);
	//This is a blatant abuse of protected methods to make the kanji subclass easy
	virtual inline Entry *makeEntry(QString x) { return new EntryEDICT(getName(),x); }

	linearEDICTFile m_file;
	static QStringList *displayFields;

	friend class EntryEDICT;
};

#endif
