/* This file is part of Kiten, a KDE Japanese Reference Tool...
    Copyright (C) 2001 by Jason Katz-Brown
              (C) 2006 by Joseph Kerian  <jkerian@gmail.com>
              (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com>

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

#include "dictFileEdict.h"

#include <kconfig.h>
#include <kconfigskeleton.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kapplication.h>

#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>

#include "dictFilePreferenceDialog.h"
#include "DictQuery.h"  //DictQuery classs
#include "Entry.h"      //Entry and EntryList classes
#include "EntryList.h"

/** Per instructions in the super-class, this constructor basically sets the
  dictionaryType member variable to identify this as an edict-type database handler */
dictFileEdict::dictFileEdict() : dictFile("edict")
{
}

QStringList *dictFileEdict::displayFields = NULL;

/** The destructor... ditch our memory maps and close our files here
  (if they were open) */
dictFileEdict::~dictFileEdict() {
}

/** Scan a potential file for the correct format, remembering to skip comment
  characters. This is not a foolproof scan, but it should be checked before adding
  a new dictionary.
  Valid EDICT format is considered:
  <kanji or kana>+ [<kana>] /latin characters & symbols/separated with slashes/
  Comment lines start with... something... not remembering now.
 */
bool dictFileEdict::validDictionaryFile(const QString &filename) {
	QFile file(filename);
	int totalLineCounter = 0;
	bool returnFlag = true;

	if(!file.exists())	//The easy test... does it exist?
		return false;
	if(!file.open(QIODevice::ReadOnly)) //And can we read it?
		return false;

	//Now we can actually check the file
	QTextStream fileStream(&file);
	fileStream.setCodec(QTextCodec::codecForName("eucJP"));
	QString commentMarker("？？？？"); //Note: Don't touch this! vim seems to have
														//An odd text codec error here too :(
	QRegExp formattedLine("^\\S+\\s+(\\[\\S+\\]\\s+)?/.*/$");
	while( !fileStream.atEnd() ) {
		QString line = fileStream.readLine();
		totalLineCounter++;

		if(line.left(4)==commentMarker)
			continue;
		if(line.contains(formattedLine)) //If it matches our regex
			continue;

		returnFlag = false;
		break;
	}

	file.close();
	return returnFlag;
}

/** Reject queries that specify anything we don't understand */
//TODO: Actually write this method (validQuery)
bool dictFileEdict::validQuery(const DictQuery &query) {
	return true;
}

/** Load up the dictionary... */
bool dictFileEdict::loadDictionary(const QString &fileName, const QString &dictName) {
	if(m_file.valid()) return false; //Already loaded

	if(m_file.loadFile(fileName)) {
		dictionaryName = dictName;
		dictionaryFile = fileName;
		return true;
	}
	return false;
}

/* Do a search, respond with a list of entries.
 The general strategy will be to take the first word of the query, and do a
 binary search on the dictionary for that item. Take all results and filter
 them using the rest of the query with the validate method.
 */
EntryList *dictFileEdict::doSearch(const DictQuery &i_query) {
	if(i_query.isEmpty() || !m_file.valid())	//No query or dict, no results.
		return new EntryList();

	DictQuery query(i_query);
	kDebug()<< "Search from : " << getName();

	QString firstChoice = query.getWord();
	if(firstChoice.length() == 0) {
		firstChoice = query.getPronunciation();
		if(firstChoice.length() == 0) {
			firstChoice = query.getMeaning().split(" ").first().toLower();
			if(firstChoice.length() == 0) {
				//The nastiest situation... we have to assemble a search string
				//from the first property
				QList<QString> keys = query.listPropertyKeys();
				if(keys.size() == 0) //Shouldn't happen... but maybe in the future
					return new EntryList();
				firstChoice = keys[0];
				firstChoice = firstChoice + query.getProperty(firstChoice);
				//TODO: doSearch: some accomodation for searching for ranges and such of properties
			}
		}
	} else
		firstChoice = firstChoice.at(0); //Only search for one kanji or the
		                                 //binary lookup mechanism breaks

	QVector<QString> preliminaryResults = m_file.findMatches(firstChoice);

	if(preliminaryResults.size() == 0)	//If there were no matches... return an empty list
		return new EntryList();

	EntryList *results = new EntryList();
	foreach(const QString &it, preliminaryResults) {
//		kDebug() << "result: " << it << endl;
			Entry *result = makeEntry(it);
			if(result->matchesQuery(query))
				results->append(result);
	}
	return results;
}

/** Make a list of all the extra fields in our db.. Entry uses this to decide
  what goes in the interpretations it gives. */
QStringList dictFileEdict::listDictDisplayOptions(QStringList x) const {
	x += displayOptions().keys();
	return x;
}

QMap<QString,QString> dictFileEdict::displayOptions() const {
	QMap<QString,QString> list;
	list["Common(C)"]="C";
	list["Part of speech(type)"]="type";
	return list;
}

DictionaryPreferenceDialog *dictFileEdict::preferencesWidget(KConfigSkeleton *config, QWidget *parent) {
	dictFileFieldSelector *dialog = new dictFileFieldSelector(config, getType(),parent);
	dialog->addAvailable(listDictDisplayOptions(QStringList()));
	return dialog;
}

void
dictFileEdict::loadSettings(KConfigSkeleton *config) {
	QMap<QString,QString> long2short = displayOptions();
	long2short["Word/Kanji"]="Word/Kanji";
	long2short["Reading"]="Reading";
	long2short["Meaning"]="Meaning";
	long2short["--Newline--"]="--Newline--";

	KConfigSkeletonItem *item = config->findItem(getType()+"__displayFields");
	this->displayFields = loadListType(item,this->displayFields,long2short);
}

QStringList *
dictFileEdict::loadListType(KConfigSkeletonItem *item, QStringList *list,
		const QMap<QString,QString> &long2short) {
	QStringList listFromItem;

	if(item != NULL)
		listFromItem = item->property().toStringList();
	if(!listFromItem.isEmpty()) {
		if(list != NULL)
			delete list;
		list = new QStringList();
		foreach(const QString &it, listFromItem)
			if(long2short.contains(it))
				list->append(long2short[it]);
	}
	return list;
}
