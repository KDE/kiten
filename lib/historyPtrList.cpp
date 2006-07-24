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
#include <QtCore/QList>
#include <QtCore/QMutableListIterator>

#include "dictquery.h"
#include "entry.h"
#include "historyPtrList.h"


historyPtrList::historyPtrList():m_index(-1) {
}

void
historyPtrList::addItem(EntryList *newItem) {
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
	while(count() >= 20) {//TODO: Make this grab the max size from KConfig
		 temp = this->takeFirst();
		 temp->deleteAll();
		 delete temp;
	}

	//Now add the item
	append(newItem);
	m_index = count()-1;
}

//Get a StringList of the History Items
QStringList
historyPtrList::toStringList() {	
	QStringList result;
	
	foreach(EntryList *p, (QList<EntryList*>)(*this)) {
		result.append(p->getQuery().toString());
	}
	
	return result;
}

QStringList
historyPtrList::toStringListPrev() {
	QStringList result;

	for(int i=0; i<m_index; i++)
		result.append(this->at(i)->getQuery().toString());

	return result;
}

QStringList
historyPtrList::toStringListNext() {
	historyPtrList localCopy(*this);
	
	int currentPosition = m_index + 1;
	while(currentPosition--)
		localCopy.removeFirst();
	
	return localCopy.toStringList();
}

void
historyPtrList::next(int distance) {
	if(distance + m_index > count() - 1)
		m_index = count() - 1;
	else
		m_index += distance;
}

void
historyPtrList::prev(int distance) {
	if(m_index - distance < 0)
		m_index = 0;
	else
		m_index -= distance;
}
