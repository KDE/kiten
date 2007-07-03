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
#include "EntryList.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextcodec.h>

#include "DictQuery.h"
#include "Entry.h"

#include <iostream>
#include <cassert>
#include <sys/mman.h>
#include <stdio.h>



EntryList::~EntryList() {
//	kdDebug() << "A copy of EntryList is being deleted... watch your memory!" << endl;
}

void
EntryList::deleteAll() {
	while(!this->isEmpty())
		delete this->takeFirst();
}

/** Returns the EntryList as HTML */
//TODO: Some intelligent decision making regarding when to print what when AutoPrinting is on
QString EntryList::toHTML(unsigned int start, unsigned int length, Entry::printType type) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result;
	for (unsigned int i = 0; i < max; ++i)
	{
		Entry *it = at(i);
		if(length-- > 0)
			result = result + "<div class=\"Entry\" index=\"" +
				QString::number(i) + "\" dict=\"" + it->getDictName()
				+ "\">" + it->toHTML(type) + "</div>";
		else
			break;

	}
	return result;
}

QString EntryList::toKVTML(unsigned int start, unsigned int length) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result = "<?xml version=\"1.0\"?>\n<!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">\n"
		"<kvtml encoding=\"UTF-8\" "
		" generator=\"kiten v42.0\""
		" title=\"To be determined\">\n";
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toKVTML() + '\n';
		else
			break;
	}
	return result +"</kvtml>\n";
}

QString EntryList::toHTML(Entry::printType type) const {
	return toHTML(0,count(),type);
}

/** Returns the EntryList as HTML */
//TODO: Some intelligent decision making... regarding the output format (differ for
//different types of searches?
QString EntryList::toString(unsigned int start, unsigned int length, Entry::printType type) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result;
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toString(type);
		else
			break;
	}
	return result;
}

QString EntryList::toString(Entry::printType type) const {
	return toString(0,count(),type);
}

/** sorts the EntryList in a C++ish, thread-safe manner. */
class sortFunctor {
	public:
		QStringList *dictionary_order;
		QStringList *sort_order;
		bool operator() (const Entry *n1, const Entry *n2) const {
			return n1->sort(*n2,*dictionary_order,*sort_order);
		}
};

void EntryList::sort(QStringList &sortOrder, QStringList &dictionaryOrder) {
	sortFunctor sorter;
	sorter.dictionary_order = &dictionaryOrder;
	sorter.sort_order = &sortOrder;

	qSort(this->begin(),this->end(),sorter);
}

/** displays an HTML version of the "no results" message */
inline QString EntryList::noResultsHTML()
{
	return "<div class=\"noResults\">No Results Found</div>";
}

const EntryList& EntryList::operator+=(const EntryList &other) {
	foreach(Entry *it, other)
			this->append(it);
	return *this;
}

void EntryList::appendList(const EntryList *other) {
	foreach(Entry *it, *other)
		append(it);
}

/**This method retrieves an earlier saved query in the EntryList,
  this should be the query that generated the list. */
DictQuery EntryList::getQuery() const {
	return query;
}

/**This allows us to save a query in the EntryList for later
  retrieval */
void EntryList::setQuery(const DictQuery &newQuery) {
	query = newQuery;
}

