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

#ifndef __DICTFILEKANJIDIC_H_
#define __DICTFILEKANJIDIC_H_

#include "dictFileEdict.h" //dictFileEdict definition
#include "entryKanjidic.h"

class dictQuery;
class QString;
class KConfigSkeleton;

class Entry;
class EntryList;
class DictionaryPreferenceDialog;

class /* NO_EXPORT */  dictFileKanjidic : public dictFileEdict {
	public:
	dictFileKanjidic();
	virtual ~dictFileKanjidic() {}

	bool validDictionaryFile(const QString &filename);
	bool validQuery(const dictQuery &query);

	virtual void loadSettings(KConfigSkeleton *);
	QMap<QString,QString> displayOptions() const;

	protected:
	virtual inline Entry *makeEntry(QString x) {return new EntryKanjidic(getName(),x);}

	static QStringList *displayFieldsList,*displayFieldsFull;
	friend class EntryKanjidic;
};

#endif
