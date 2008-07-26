/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>
			    (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

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
#include "HistoryPtrList.h"
#include "EntryList.h"

#include <QtCore/QList>
#include <QtCore/QMutableListIterator>

class HistoryPtrList::Private {
public:
		Private() : m_index(-1) {}
		static const int s_max_size = 20;
		int m_index;
		QList<EntryList*> m_list;
};

HistoryPtrList::HistoryPtrList():d(new Private) {
}

HistoryPtrList::~HistoryPtrList() {
	for(int i=d->m_list.size()-1; i>=0; i--) {
		d->m_list.at(i)->deleteAll();
		delete d->m_list.at(i);
	}
	delete d;
}

void
HistoryPtrList::addItem(EntryList *newItem) {
	if(!newItem) return;
	//If we're currently looking at something prior to the end of the list
	//Remove everything in the list up to this point.
	int currentPosition = d->m_index+1;
	EntryList *temp;
	while(currentPosition < count()) {
		temp = d->m_list.takeLast();
		temp->deleteAll();
		delete temp;
	}

	//Now... check to make sure our history isn't 'fat'
	while(count() >= d->s_max_size) {
		 temp = d->m_list.takeFirst();
		 temp->deleteAll();
		 delete temp;
	}
	d->m_index = count()-1; //Since we have trimmed down to the current position

	//One other odd case... if this query is a repeat of the last query
	//replace the current one with the new one
	if(d->m_list.size() > 0) {
		if(current()->getQuery() == newItem->getQuery()) {
			temp = d->m_list.takeLast();
			temp->deleteAll();
			delete temp;
		}
	}
	//Now add the item
	d->m_list.append(newItem);
	d->m_index = count()-1;
}

//Get a StringList of the History Items
QStringList
HistoryPtrList::toStringList() {
	QStringList result;

	foreach(const EntryList *p, d->m_list)
		result.append(p->getQuery().toString());

	return result;
}

QStringList
HistoryPtrList::toStringListPrev() {
	QStringList result;

	for(int i=0; i<d->m_index; i++)
		result.append(d->m_list.at(i)->getQuery().toString());

	return result;
}

QStringList
HistoryPtrList::toStringListNext() {
	HistoryPtrList localCopy(*this);

	int currentPosition = d->m_index + 1;
	while(currentPosition--)
		localCopy.d->m_list.removeFirst();

	return localCopy.toStringList();
}

void
HistoryPtrList::next(int distance) {
	if(distance + d->m_index > count() - 1)
		d->m_index = count() - 1;
	else
		d->m_index += distance;
}

void
HistoryPtrList::prev(int distance) {
	if(d->m_index - distance < 0)
		d->m_index = 0;
	else
		d->m_index -= distance;
}

EntryList*
HistoryPtrList::current() {
	if(d->m_index == -1) return NULL;
	return d->m_list.at(d->m_index);
}

void
HistoryPtrList::setCurrent(int i) {
	if(i<count() && i>=0)
		d->m_index=i;
}

int
HistoryPtrList::index() {
	return d->m_index;
}

int
HistoryPtrList::count() {
	return d->m_list.size();
}
