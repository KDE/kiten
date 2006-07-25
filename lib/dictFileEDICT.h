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

#ifndef __DICTFILEEDICT_H_
#define __DICTFILEEDICT_H_

#include "dictionary.h" //dictFile definition

#include <sys/types.h>
#ifdef __osf__
typedef unsigned int uint32_t;
typedef int int32_t;
#else
#include <inttypes.h>
#endif

class QFile;
class QString;
class QByteArray;

class dictQuery;
class DictionaryPreferenceDialog;
class KConfigSkeleton;

#include "entryEDICT.h"

class /* NO_EXPORT */ dictFileEdict : public dictFile {
	public:
	dictFileEdict();
	virtual ~dictFileEdict();
	
	virtual bool validDictionaryFile(const QString filename);
	bool validQuery(const dictQuery &query);
	
	bool loadDictionary(const QString file, const QString name);
	bool loadNewDictionary(const QString file, const QString name);
	
	virtual EntryList *doSearch(const dictQuery &query);
	virtual QStringList listDictDisplayOptions(QStringList) const;

	virtual inline QString getFile() { return dictFile.fileName(); }

	virtual DictionaryPreferenceDialog *preferencesWidget(KConfigSkeleton*,QWidget *parent=NULL, const char *name=0);

	protected:
	//This is a blatant abuse of protected methods to make the kanji subclass easy
	virtual inline Entry *makeEntry(QString x) { return new EntryEDICT(getName(),x); }

	//Utility methods for handling the index files
	unsigned char lookupDictChar(unsigned);
	QByteArray lookupDictLine(unsigned);
	QByteArray lookupFullLine(unsigned);
	int equalOrSubstring(const char*, const char *);
	int findMatches(const char*, const char *);
	
	static const int indexFileVersion = 14;	//This code understands version 14 index (.xjdx) files
	
	QFile dictFile;
	const unsigned char *dictPtr;

	QFile indexFile;
	const uint32_t *indexPtr;

	bool valid;
};

#endif
