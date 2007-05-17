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
#ifndef __DICTIONARYMANAGER_H_
#define __DICTIONARYMANAGER_H_

#include "libkitenexport.h"

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QPair>

class QWidget;
class QString;
class QStringList;

class Entry;
class EntryList;
class KConfigSkeleton;
class KConfig;

class dictFile;
class DictQuery;
class DictionaryPreferenceDialog;

/** @short The DictionaryManager class is the fundamental dictionary management class. All
 * interfaces with the rest of the programs using the various dictionaries will
 * work through this "interface class" to keep the formatting and other such
 * nasty details away from programs and sections which just want to use the
 * dictionary without bothering with the internal formatting details. As a
 * general rule, call this class with a DictQuery to get a list of
 * entries as the result.
 *
 * The idea is that the interfaces need to know how to load a query, pass the
 * query to dictionary.	DictionaryManager will return to them an EntryList object,
 * each Entry knows how to display itself (via the magic of C++ polymorphism).
 * There are some setup and preference handling methods which complicate
 * things, but generally speaking this is the way this should work.
 */

class KITEN_EXPORT DictionaryManager {
	public:
	DictionaryManager();
	virtual ~DictionaryManager();

	bool addDictionary(const QString &file,const QString &name,const QString &type);
	bool removeDictionary(const QString &name);
	//List names of each open dict
	QStringList listDictionaries() const;
	//Returns type and file
	QPair<QString, QString> listDictionaryInfo(const QString &name) const;
	//Convenience function for prefs
	QStringList listDictionariesOfType(const QString &type) const;
	//This is the main search routine that most of kiten should use
	EntryList *doSearch(const DictQuery &query) const;
	//This is used to search in results
	EntryList *doSearchInList(const DictQuery &query, const EntryList *list) const;
	//Static methods to handle adding modules to the system in an easier way
	static dictFile *makeDictFile(const QString&);
	//Mostly required for the Preferences system
	static QStringList listDictFileTypes();
	//Generate the Preference widgets
	static QMap<QString,DictionaryPreferenceDialog*>
		generatePreferenceDialogs(KConfigSkeleton *config, QWidget *parent=NULL);
	//Generate a list of all of the extended fields
	static QMap<QString,QString> generateExtendedFieldsList();
	//Used to trigger reloading the display settings for a particular dictionary
	void loadDictSettings(const QString &dict, KConfigSkeleton*);
	//Used to load general settings related to output
	void loadSettings(const KConfig&);

	private:
	QHash<QString,dictFile*> dictManagers;	//List is indexed by dictionary names.
};

#endif
