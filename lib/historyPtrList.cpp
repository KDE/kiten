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
#include <kdebug.h>
#include <q3ptrlist.h>

#include "dictquery.h"
#include "entry.h"
#include "historyPtrList.h"


historyPtrList::historyPtrList() {
	setAutoDelete(true);
}

void
historyPtrList::addItem(EntryList *newItem) {
	//If we're currently looking at something prior to the end of the list
	//Remove everything in the list up to this point.
	int currentPosition = at()+1;
	while(currentPosition < count())
		removeLast();
	

	//Now... check to make sure our history isn't 'fat'
	while(count() >= 20) //TODO: Make this grab the max size from KConfig
		removeFirst();

	//Now add the item
	append(newItem);
}

//Get a StringList of the History Items
QStringList
historyPtrList::toStringList() {	
	Q3PtrListIterator<EntryList> it(*this);
	QStringList result;
	EntryList *curr;
	
	for(it.toFirst(); (curr=it.current()) != 0; ++it)
		result.append(curr->getQuery().toString());

	return result;
}

QStringList
historyPtrList::toStringListPrev() {
	Q3PtrListIterator<EntryList> it(*this);
	QStringList result;
	EntryList *curr;
	
	for(it.toFirst(); (curr=it.current()) != 0 && it.current() != this->current(); ++it)
		result.append(curr->getQuery().toString());

	return result;
}

QStringList
historyPtrList::toStringListNext() {
	historyPtrList localCopy(*this);
	localCopy.setAutoDelete(false);
	
	int currentPosition = at() + 1;
	while(currentPosition--)
		localCopy.removeFirst();
	
	return localCopy.toStringList();
}

void
historyPtrList::next(int distance) {
	for(int i=0; i< distance; i++)
		if(Q3PtrList<EntryList>::next() == 0)
			last();
}

void
historyPtrList::prev(int distance) {
	for(int i=0; i< distance; i++)
		if(Q3PtrList<EntryList>::prev() == 0)
			first();
}

EntryList*
historyPtrList::at(uint index) {
	EntryList *ret = Q3PtrList<EntryList>::at(index);
	if(ret == 0)
		ret = last();
	return ret;
}


