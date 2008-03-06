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

#ifndef KITEN_DICTFILEDEINFLECT_H
#define KITEN_DICTFILEDEINFLECT_H

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "dictFile.h"
#include "entry.h"
#include "dictQuery.h"

class /* NO_EXPORT */ dictFileDeinflect: public dictFile
{
	public:
		dictFileDeinflect() : dictFile() {dictionaryType = "Deinflect";}
		virtual ~dictFileDeinflect() {}

		bool loadDictionary(const QString &file, const QString &name);
		bool loadNewDictionary(const QString &file, const QString &name)
				{ return loadDictionary(file,name); }
		QStringList listDictDisplayOptions(QStringList orig) const
				{ return QStringList("Word/Kanji") << QString("Deinflected Word"); }

		EntryList *doSearch (const dictQuery &query);

		//TODO: The following methods (validDictionaryFile & validQuery)
		bool validDictionaryFile(const QString &filename) { return true; }
		bool validQuery(const dictQuery &query) { return true; }

};


#endif
