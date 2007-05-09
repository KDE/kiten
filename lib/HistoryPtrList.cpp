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


HistoryPtrList::HistoryPtrList():m_index(-1) {
}

HistoryPtrList::~HistoryPtrList() {
	for(int i=size()-1; i>=0; i--)
		delete this->at(i);
}

void
HistoryPtrList::addItem(EntryList *newItem) {
	//If we're currently looking at something prior to the end of the list
	//Remove everything in the list up to this point.
	int currentPosition = m_index+1;
	EntryList *temp;
	while(currentPosition < count()) {
		temp = this->takeLast();
		temp->deleteAll();
		delete temp;
	}

	//Now... check to make sure our history isn't 'fat'
	while(count() >= 20) {
		 temp = this->takeFirst();
		 temp->deleteAll();
		 delete temp;
	}

	//One other odd case... if this query is a repeat of the last query
	//replace the current one with the new one
	if(size() > 0) {
		if(current()->getQuery() == newItem->getQuery()) {
			temp = this->takeLast();
			temp->deleteAll();
			delete temp;
		}
	}

	//Now add the item
	append(newItem);
	m_index = count()-1;
}

//Get a StringList of the History Items
QStringList
HistoryPtrList::toStringList() {
	QStringList result;

	foreach(EntryList *p, (QList<EntryList*>)(*this))
		result.append(p->getQuery().toString());

	return result;
}

QStringList
HistoryPtrList::toStringListPrev() {
	QStringList result;

	for(int i=0; i<m_index; i++)
		result.append(this->at(i)->getQuery().toString());

	return result;
}

QStringList
HistoryPtrList::toStringListNext() {
	HistoryPtrList localCopy(*this);

	int currentPosition = m_index + 1;
	while(currentPosition--)
		localCopy.removeFirst();

	return localCopy.toStringList();
}

void
HistoryPtrList::next(int distance) {
	if(distance + m_index > count() - 1)
		m_index = count() - 1;
	else
		m_index += distance;
}

void
HistoryPtrList::prev(int distance) {
	if(m_index - distance < 0)
		m_index = 0;
	else
		m_index -= distance;
}

EntryList*
HistoryPtrList::current() {
	return at(m_index);
}

void
HistoryPtrList::setCurrent(int i) {
	if(i<count() && i>=0)
		m_index=i;
}

int
HistoryPtrList::index() {
	return m_index;
}

int
HistoryPtrList::count() {
	return QList<EntryList*>::size();
}
