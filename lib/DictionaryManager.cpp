/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

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

#include "DictionaryManager.h"
#include "DictionaryPreferenceDialog.h"
#include "DictQuery.h"
#include "Entry.h"
#include "EntryList.h"

#include "dictFile.h"

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfigskeleton.h>
#include <QtCore/QString>
#include <QtCore/QFile>

/* Includes to handle various types of dictionaries
IMPORTANT: To add a dictionary type, add the header file here and add it to the
 if statement under addDictionary() */
#include "dictEdict/dictFileEdict.h"
#include "dictKanjidic/dictFileKanjidic.h"

/** IMPORTANT: To add a dictionary type, you have to manually add the creation
	step here, the next method, and #include your header file above. If you have
	fully implemented the interface in DictionaryManager.h, It should simply work.*/
dictFile *DictionaryManager::makeDictFile(const QString type) {
	if(type == "edict")
		return new dictFileEdict();
	if(type == "kanjidic")
		return new dictFileKanjidic();
	//Add new dictionary types here!!!

	return NULL;
}
/** IMPORTANT: To add a dictionary type, you have to manually add the creation
	step here, the prev method, and #include your header file above. If you have
	fully implemented the interface in DictionaryManager.h, It should simply work.*/
QStringList DictionaryManager::listDictFileTypes() {
	QStringList list;
	list.append("edict");
	list.append("kanjidic");
	//Add your dictionary type here!
	return list;
}

/** Given a named Dict file/name/type... create and add the object if it
  seems to work properly on creation.
  */
bool DictionaryManager::addDictionary(const QString file, const QString name,
		const QString type) {

	if(dictManagers.contains(name)) //This name already exists in the list!
		return false;

	dictFile *newDict = makeDictFile(type);
	if(newDict == NULL)
		return false;

	if(!newDict->loadDictionary(file,name)) {
		kDebug() << "Dictionary load FAILED: " << newDict->getName() << endl;
		delete newDict;
		return false;
	}

	kDebug() << "Dictionary Loaded : " << newDict->getName() << endl;
	dictManagers.insert(name,newDict);
	return true;
}

/** The constructor. Set autodelete on our dictionary list */
DictionaryManager::DictionaryManager() {
}

/** Delete everything in our hash */
DictionaryManager::~DictionaryManager() {
	QMutableHashIterator<QString, dictFile*> it(dictManagers);
	while(it.hasNext()) {
		it.next();
		delete it.value();
		it.remove();
	}
}

/** Remove a dictionary from the list, and delete the dictionary object
  (it should close files, deallocate memory, etc). */
bool DictionaryManager::removeDictionary(const QString name) {
	dictFile *file = dictManagers.take(name);
	delete file;
	return true;
}

/** Return a list of the dictionaries by their name (our key)
  Note that this dictionary name does not necessarily have to have anything
  to do with the actual dictionary name... */
QStringList DictionaryManager::listDictionaries() const {
	QStringList ret;
	foreach(dictFile *it, dictManagers)
		ret.append(it->getName());
	return ret;
}

/** Return the dictionary type and file used by a named dictionary.
  returns a pair of empty QStrings if you specify an invalid name */
QPair<QString, QString> DictionaryManager::listDictionaryInfo(const QString name) const {
	if(!dictManagers.contains(name)) //This name not in list!
		return qMakePair(QString(),QString()); //KDE4 CHANGE
	return qMakePair(dictManagers[name]->getName(),dictManagers[name]->getFile());
}

/** Return a list of the names of each dictionary of a given type. */
QStringList DictionaryManager::listDictionariesOfType(const QString type) const {
	QStringList ret;
	QHash<QString, dictFile*>::const_iterator it = dictManagers.begin();
	while(it != dictManagers.end()) {
		if(it.value()->getType() == type)
			ret.append(it.key());
		++it;
	}
	return ret;
}

/** Examine the DictQuery and farm out the search to the specialized dict 
  managers. Note that a global search limit will probably be implemented 
  either here or in the dictFile implementations... probably both */
EntryList *DictionaryManager::doSearch(const DictQuery &query) const {
	EntryList *ret=new EntryList();

	//There are two basic modes.... one in which the query
	//Specifies the dictionary list, one in which it does not
	QStringList dictsFromQuery = query.getDictionaries();
	if(dictsFromQuery.isEmpty()) { //None specified, search all
		foreach( dictFile *it, dictManagers) {
			EntryList *temp=it->doSearch(query);
			ret->appendList(temp);
			delete temp;
		}
	} else {
		foreach( const QString &target, dictsFromQuery) {
			dictFile *newestFound = dictManagers.find(target).value();
			if(newestFound != 0) {
				EntryList *temp = newestFound->doSearch(query);
				ret->appendList(temp);
				delete temp;
			}
		}
	}

	ret->setQuery(query); //Store the query for later use.
	kDebug() << "From query: '" << query.toString() << "'"<< endl;
	kDebug() << "Found " << ret->count() << " results"<<endl;
	return ret;
}

/** For this case, we let polymorphism do most of the work. We assume that the user wants
  to pare down the results, so we let the individual entry metching methods run over the
  new query and accept (and copy) any of those that pass */
EntryList *DictionaryManager::doSearchInList(const DictQuery &query, const EntryList *list) const {
	EntryList *ret = new EntryList();

	foreach( Entry* it, *list) {
		if(it->matchesQuery(query)) {
			Entry *x = it->clone();
			ret->append(x);
		}
	}
	ret->setQuery(query + list->getQuery());
	return ret;
}

void DictionaryManager::loadDictSettings(const QString dictName, KConfigSkeleton *config) {
	dictFile *dict = this->makeDictFile(dictName);
	if(dict != NULL) {
		config->setCurrentGroup("dicts_"+dictName.toLower());
		dict->loadSettings(config);
	}
}

void DictionaryManager::loadSettings(const KConfig &config) { //TODO
}

QMap<QString,DictionaryPreferenceDialog*>
DictionaryManager::generatePreferenceDialogs(KConfigSkeleton *config, QWidget *parent) {
	QMap<QString,DictionaryPreferenceDialog*> result;
	QStringList dictTypes = listDictFileTypes();
	foreach(const QString &dictType, dictTypes) {
		dictFile *tempDictFile = makeDictFile(dictType);
		DictionaryPreferenceDialog *newDialog =
				tempDictFile->preferencesWidget(config,parent);
		if(newDialog==NULL)
			continue;
		result.insert(dictType,newDialog);
		delete tempDictFile;
	}
	return result;
}

QMap<QString, QString>
DictionaryManager::generateExtendedFieldsList() {
	QMap<QString,QString> result;
	QStringList dictTypes = listDictFileTypes();
	foreach(const QString &dictType, dictTypes) {
		dictFile *tempDictFile = makeDictFile(dictType);
		QMap<QString,QString> tempList = tempDictFile->getSearchableAttributes();
		QMap<QString,QString>::const_iterator it = tempList.constBegin();
		while( it != tempList.constEnd() ) {
			if(!result.contains(it.key()))
				result.insert(it.key(),it.value());
			++it;
		}
		delete tempDictFile;
	}
	return result;
}

