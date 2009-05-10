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

#include <sys/mman.h>

class EntryList::Private {
public:
	Private() : m_storedScrollValue(0), m_sorted(false), m_sortedByDictionary(false) { }
	Private(const Private &old) : m_storedScrollValue(old.m_storedScrollValue),
		m_sorted(old.m_sorted), m_sortedByDictionary(old.m_sortedByDictionary),
		query(old.query) {}

	int m_storedScrollValue;
	bool m_sorted;
	bool m_sortedByDictionary;
	DictQuery query;
};

EntryList::EntryList() : QList<Entry*>(), d(new Private) {
}

EntryList::EntryList(const EntryList &old) : QList<Entry*> (old), d(new Private(*(old.d)))
{
}

EntryList::~EntryList() {
	delete d;
//	kdDebug() << "A copy of EntryList is being deleted... watch your memory!" << endl;
}

int EntryList::scrollValue() const { return d->m_storedScrollValue; }
void EntryList::setScrollValue(int val) { d->m_storedScrollValue = val; }
void
EntryList::deleteAll() {
	while(!this->isEmpty())
		delete this->takeFirst();
	d->m_sorted = false;
}

/* Returns the EntryList as HTML */
//TODO: Some intelligent decision making regarding when to print what when AutoPrinting is on
QString EntryList::toHTML(unsigned int start, unsigned int length) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result;
	QString temp;
	QString &lastDictionary = temp;
	const QString fromDictionary = i18n("From Dictionary:");
	QString query(getQuery());
	for (unsigned int i = 0; i < max; ++i)
	{
		Entry *it = at(i);
		if(d->m_sortedByDictionary) {
			const QString &newDictionary = it->getDictName();
			if(lastDictionary != newDictionary) {
				lastDictionary = newDictionary;
				result += "<div class=\"DictionaryHeader\">"+fromDictionary+" "+newDictionary+"</div>";
			}
		}
		if(length-- > 0)
			result += "<div class=\"Entry\" index=\"" +
				QString::number(i) + "\" dict=\"" + it->getDictName()
				+ "\">" + it->toHTML() + "</div>";
		else
			break;

	}
	//result.replace(query, "<query>" + query + "</query>");
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

QString EntryList::toHTML() const {
	return toHTML(0,count());
}

/* Returns the EntryList as HTML */
//TODO: Some intelligent decision making... regarding the output format (differ for
//different types of searches?
QString EntryList::toString(unsigned int start, unsigned int length) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result;
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toString();
		else
			break;
	}
	return result;
}

QString EntryList::toString() const {
	return toString(0,count());
}

/* sorts the EntryList in a C++ish, thread-safe manner. */
class sortFunctor {
	public:
		QStringList *dictionary_order;
		QStringList *sort_order;
		bool operator() (const Entry *n1, const Entry *n2) const {
			return n1->sort(*n2,*dictionary_order,*sort_order);
		}
};

void EntryList::sort(QStringList &sortOrder, QStringList &dictionaryOrder) {
	//Don't shortcut sorting, unless we start to keep track of the last sorting order,
	//Otherwise we won't respond when the user changes the sorting order
	sortFunctor sorter;
	sorter.dictionary_order = &dictionaryOrder;
	sorter.sort_order = &sortOrder;

	qStableSort(this->begin(),this->end(),sorter);
	d->m_sorted = true;
	d->m_sortedByDictionary = dictionaryOrder.size() > 0;
}

const EntryList& EntryList::operator+=(const EntryList &other) {
	foreach(Entry *it, other)
			this->append(it);
	if(other.size() > 0)
		d->m_sorted = false;
	return *this;
}

const EntryList& EntryList::operator=(const EntryList &other) {
	QList<Entry*>::operator=(other);
	*d = *(other.d);
	return *this;
}

void EntryList::appendList(const EntryList *other) {
	foreach(Entry *it, *other)
		append(it);
	if(other->size() > 0)
		d->m_sorted = false;
}

/**This method retrieves an earlier saved query in the EntryList,
  this should be the query that generated the list. */
DictQuery EntryList::getQuery() const {
	return d->query;
}

/**This allows us to save a query in the EntryList for later
  retrieval */
void EntryList::setQuery(const DictQuery &newQuery) {
	d->query = newQuery;
}

