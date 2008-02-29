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
#ifndef KITEN_DICTIONARYMANAGER_H_
#define KITEN_DICTIONARYMANAGER_H_

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
 *
 * @author Joseph Kerian <jkerian@gmail.com>
 */

class KITEN_EXPORT DictionaryManager {
	public:
	/** Basic constructor */
	DictionaryManager();
	/** Basic destructor */
	virtual ~DictionaryManager();

	/** Open a dictionary specified by @p file, named @p name, of type @p type */
	bool addDictionary(const QString &file,const QString &name,const QString &type);
	/** Close a dictionary by @p name */
	bool removeDictionary(const QString &name);
	/** List names of each open dictionary */
	QStringList listDictionaries() const;
	/** Returns type and file for an open dictionary of a given @p name */
	QPair<QString, QString> listDictionaryInfo(const QString &name) const;
	/** Lists all dictionaries of a given @p type (Convenient for preference dialogs) */
	QStringList listDictionariesOfType(const QString &type) const;
	/** This is the main search routine that most of kiten should use */
	EntryList *doSearch(const DictQuery &query) const;
	/** This should be significantly more efficient than repeating a doSearch call */
	EntryList *doSearchInList(const DictQuery &query, const EntryList *list) const;
	/** Static method, used to create the polymorphic dictFile object. Do not use externally.
			If you are adding a new dictionary type, see the instructions in the code. */
	static dictFile *makeDictFile(const QString&);
	/** Get a list of all supported dictionary types. Useful for preference code */
	static QStringList listDictFileTypes();
	/** Given a @p config and @p parent widget, return a mapping from dictionary types to preference dialogs.
	  If a particular dictionary type does not provide a preference dialog, it will not be included in this list,
	  so occasionally keys(returnvalue) != listDictFileTypes() */
	static QMap<QString,DictionaryPreferenceDialog*>
		generatePreferenceDialogs(KConfigSkeleton *config, QWidget *parent=NULL);
	/** Compiles a list of all fields beyond the basic three (word/pronounciation/meaning) that all dictionary
	  types support. This can be used to generate a preference dialog, or provide more direct references */
	static QMap<QString,QString> generateExtendedFieldsList();
	/** Trigger loading preferences from a given KConfigSkeleton @p config object for a dictionary of type @p dict */
	void loadDictSettings(const QString &dict, KConfigSkeleton* config);
	/** Load general settings */
	void loadSettings(const KConfig&);

	private:
	QHash<QString,dictFile*> dictManagers;	//List is indexed by dictionary names.
};

#endif
