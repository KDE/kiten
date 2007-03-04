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

#ifndef ENTRYLIST_H
#define ENTRYLIST_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

#include "libkitenexport.h"
#include "DictQuery.h"
#include "Entry.h"

class KITEN_EXPORT EntryList : public QList<Entry*> {

public:
	typedef QListIterator<Entry*> EntryIterator;

	EntryList() : QList<Entry*>() {}
	EntryList(const EntryList &old) : QList<Entry*>(old) {}
	virtual ~EntryList();
	void deleteAll();

	QString toString(Entry::printType=Entry::printAuto) const;
	QString toHTML(Entry::printType=Entry::printAuto) const;

	QString toString(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	QString toHTML(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	QString toKVTML(unsigned int start, unsigned int length) const;

	void sort(QStringList &sortOrder,QStringList &dictionaryOrder);

	const EntryList& operator+=(const EntryList &other);
	void appendList(const EntryList *);
	DictQuery getQuery() const;
	void setQuery(const DictQuery&);

protected:
	DictQuery query;

private: //Utility Methods
	inline QString noResultsHTML();
};

#endif
