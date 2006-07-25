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
#ifndef __DICTIONARY_H_
#define __DICTIONARY_H_

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

class dictFile;
class dictQuery;

/** The dictionary class is the fundamental dictionary management class. All 
 * interfaces with the rest of the programs using the various dictionaries will
 * work through this "interface class" to keep the formatting and other such 
 * nasty details away from programs and sections which just want to use the 
 * dictionary without bothering with the internal formatting details. As a 
 * general rule, call the dictionary class with a dictQuery to get a list of 
 * entries as the result. 
 *
 * The idea is that the interfaces need to know how to load a query, pass the 
 * query to dictionary.	Dictionary will return to them an EntryList object, 
 * each Entry knows how to display itself (via the magic of C++ polymorphism). 
 * There are some setup and preference handling methods which complicate 
 * things, but generally speaking this is the way this should work.
 */

class LIBKITEN_EXPORT dictionary {
	public:
	dictionary();
	virtual ~dictionary();

	bool addDictionary(const QString file,const QString name,const QString type);
	bool removeDictionary(const QString name);
	//List names of each open dict
	QStringList listDictionaries() const;
	//Returns type and file
	QPair<QString, QString> listDictionaryInfo(const QString name) const; 
	//Convenience function for prefs
	QStringList listDictionariesOfType(const QString type) const;
	//This is the main search routine that most of kiten should use
	EntryList *doSearch(const dictQuery &query) const; 
	//This is used to search in results
	EntryList *doSearchInList(const dictQuery &query, const EntryList *list) const;
	//Static methods to handle adding modules to the system in an easier way
	static dictFile *makeDictFile(const QString);
	//Mostly required for the Preferences system
	static QStringList listDictFileTypes();
	//Used to trigger relaoding the display settings..//TODO.
	void loadDisplaySettings() {}

	private:
	QHash<QString,dictFile*> dictManagers;	//List is indexed by dictionary names.
};


class DictionaryPreferenceDialog;
/** This is a virtual class that enforces the interface between the dictionary
 *  class and the dictionary handler files. IMPLEMENT in combination with an
 *   Entry subclass to add a new dictionary format. Also see the addDictionary
 *   method in the dictionary class. */
class LIBKITEN_EXPORT dictFile {
public:
	/** Please see the comment at the dictionaryType protected variable*/
	dictFile() {}
	virtual ~dictFile() {}
	/** Test to see if a dictionary file is of the proper type */
	virtual bool validDictionaryFile(const QString filename) = 0; 
	/** Is this query relevent to this dictionary type? */
	virtual bool validQuery(const dictQuery &query) = 0;		
	/** This actually conducts the search. This is usually most of the work */
	virtual EntryList *doSearch(const dictQuery &query) = 0;	
	/** Load a dictionary (as in system startup) */
	virtual bool loadDictionary(const QString file, const QString name)=0;
  	/** Load a new dictionary (as from add dictionary dialog */
	virtual bool loadNewDictionary(const QString file, const QString name)=0;
	/** Return a list of the fields that can be displayed, note the following 
	  should probably always be retured: --NewLine--, Word/Kanji, Meaning, 
	  Reading.  This function is passed a list originally containing those
	  items. This function is used to enumerate possible types the user 
	  chooses to have displayed in the preferences dialog box. 
	  This will often be a very similer list to getSearchableAttributes(),
	  but due to optional forms of spelling and other situations, it may
	  not be exactly the same. Note: The "Dictionary" option will be
	  appended to your list at the end*/
	virtual QStringList listDictDisplayOptions(QStringList) const =0 ;
	/** If you want your own dialog to pick preferences for your dict...
	  override this */
	virtual DictionaryPreferenceDialog *preferencesWidget(KConfigSkeleton *config,QWidget *parent=NULL, const char *name=0) 
				{if(parent==parent && name==name && config==config) return NULL;}
	
	/** Inline functions to return quick info */
	virtual inline const QString &getName() const {return dictionaryName;}
	/** Inline functions to return quick info */
	virtual inline const QString &getType() const {return dictionaryType;}
	/** Inline functions to return quick info */
	virtual inline const QString &getFile() const {return dictionaryFile;}
	/** Fetch a list of searchable attributes and their codes */
	virtual inline const QMap<QString,QString> &getSearchableAttributes() const 
													{return searchableAttributes;}
protected:
	/** Name is the 'primary key' of the list of dictionaries. You will want to 
	 * place this into your Entry objects to identify where they came from 
	 * (fairly important) */
	QString dictionaryName;

	/** This is mostly a placeholder, but your class will get asked what file 
	 * it is using, so either be sure to put something here, or override 
	 * getFile() and respond with something that will be sensical in a 
	 * dictionary selection dialog box */
	QString dictionaryFile;

	/** This MUST BE SET IN THE CONSTRUCTOR. The dictionary class occasionally 
	 * uses this value and it's important for it to be set at anytime after the
	 * constructor is called. It also must be unique to the dictionary type. If
	 * relevent, specify dictionary versions here. */
	QString dictionaryType; 
	/** This is not currently used, but it will be used to allow the users a list
	 * of possible search types (probably through a drop down menu) at some point
	 * in the future. You may also find it useful in your dictFile implementation
	 * to translate from extended attribute keys into the simpler one or two letter
	 * code keys. These should take the format of:
	 *  (grade => G), (strokes => S), (stroke => S)
	 * for a simple example appropriate to kanji. Note the lower case keys and that
	 * duplicate references are acceptable. */
	QMap<QString,QString> searchableAttributes;
};

#endif
