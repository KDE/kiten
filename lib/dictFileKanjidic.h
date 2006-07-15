 /***************************************************************************
 *   Copyright (C) 2001 by Jason Katz-Brown                                *
 *             (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
 *             (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __DICTFILEKANJIDIC_H_
#define __DICTFILEKANJIDIC_H_

#include "dictFileEDICT.h" //dictFileEdict definition
#include "entryKanjidic.h"

#include <qmap.h>

class dictQuery;
class QString;
class KConfigSkeleton;

class Entry;
class EntryList;
class DictionaryPreferenceDialog;

class /* KDE_EXPORT */ dictFileKanjidic : public dictFileEdict {
	public:
	dictFileKanjidic();
	virtual ~dictFileKanjidic() {}
	
	bool validDictionaryFile(const QString filename);
	bool validQuery(const dictQuery &query);

	QStringList listDictDisplayOptions(QStringList) const;

	DictionaryPreferenceDialog *preferencesWidget(KConfigSkeleton*,QWidget *, const char *);

	protected:
	virtual inline Entry *makeEntry(QString x) {return new EntryKanjidic(getName(),x);}
	const QMap<QString,QString> displayOptions() const;
};

#endif
