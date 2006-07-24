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

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qfileinfo.h> 
#include <qregexp.h>
#include <qtextcodec.h>

#include "dictquery.h"
#include "entry.h"

#include <iostream>
#include <cassert>
#include <sys/mman.h> 
#include <stdio.h>



EntryList::~EntryList() {
//	kdDebug() << "A copy of EntryList is being deleted... watch your memory!" << endl;
}

void
EntryList::deleteAll() {
	while(!this->isEmpty())
		delete this->takeFirst();
}

/** Returns the EntryList as HTML */
//TODO: Some intelligent decision making regarding when to print what when AutoPrinting is on
QString EntryList::toHTML(unsigned int start, unsigned int length, Entry::printType type) const {
	unsigned int max = count();
	if(start > max) return QString::null;
	if(start+length > max) length = max-start;

	QString result;
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toHTML(type);
		else
			break;
	}
	return result;
}
	
QString EntryList::toHTML(Entry::printType type) const {
	return toHTML(0,count(),type);
}

/** Returns the EntryList as HTML */
//TODO: Some intelligent decision making... regarding the output format (differ for
//different types of searches? 
QString EntryList::toString(unsigned int start, unsigned int length, Entry::printType type) const {
	unsigned int max = count();
	if(start > max) return QString::null;
	if(start+length > max) length = max-start;

	QString result;
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toString(type);
		else
			break;
	}
	return result;
}
	
QString EntryList::toString(Entry::printType type) const {
	return toString(0,count(),type);
}

/* TODO: sorting of Entry objects within the EntryList */
/** sorts the EntryList.  Should be called after the EntryList is filled. */
void EntryList::sort(sortType type) {
}

void EntryList::sort(QStringList dictionaryOrder, sortType type) {
}

/** displays an HTML version of the "no results" message */
inline QString EntryList::noResultsHTML()
{
	return "<div class=\"noResults\">No Results Found</div>";
}

const EntryList& EntryList::operator+=(const EntryList &other) {
	foreach(Entry *it, other)
			this->append(it);
	return *this;
}

void EntryList::appendList(const EntryList *other) {
	foreach(Entry *it, *other)
		append(it);
}

/**This method retrieves an earlier saved query in the EntryList,
  this should be the query that generated the list. */
dictQuery EntryList::getQuery() const {
	return query;
}

/**This allows us to save a query in the EntryList for later
  retrieval */
void EntryList::setQuery(const dictQuery &newQuery) {
	query = newQuery;
}


///////////////////////////////////////////////////////////////
/** The default constructor, unless you really know what you're doing,
  THIS SHOULD NOT BE USED. For general use, other entities will need
 to have the information provided by the other constructors
 (particularly the sourceDictionary)*/
Entry::Entry() {
	init();
}
Entry::Entry(const QString &sourceDictionary)
	: sourceDict(sourceDictionary)
{
	init();
}
Entry::Entry(const QString &sourceDictionary,const QString &parseMe)
	: sourceDict(sourceDictionary)
{
	init();
	kDebug() << "ERROR: This method should not have been called. If the"<<endl<<
		"\tsubclass of Entry did have a(QString dict,QString parseString)"<<endl<<
		"\tconstructor, you need to call Entry(dict) instead for init."<<endl;
	kDebug() << "You tried to create this obj: "<<parseMe<<endl;
}

Entry::Entry(const QString &sourceDictionary, const QString &word,
	  			const QStringList &reading, const QStringList &meanings)
	: sourceDict(sourceDictionary)
	, Word(word)
	, Meanings(meanings)
	, Readings(reading)
{
	init();
}

Entry::Entry(const QString &sourceDictionary, const QString &word, 
		const QStringList &readings, const QStringList &meanings, 
		const QHash<QString,QString> &extendedInfo)
	: sourceDict(sourceDictionary)
	, Word(word)
	, Meanings(meanings)
	, Readings(readings)
	, ExtendedInfo(extendedInfo)
{
	init();
}

Entry::Entry(const Entry &src)
	: sourceDict(src.sourceDict)
	, Word(src.Word)
	, Meanings(src.Meanings)
	, Readings(src.Readings)
	, ExtendedInfo(src.ExtendedInfo)
{
	init();
}

void Entry::init() {
	outputListDelimiter=i18n("; ");
	favoredPrintType = printBrief;
}

Entry::~Entry() {
//	kdDebug() << "nuking : " << Word << endl;

}

/** Main switching function for displaying to the user */
QString Entry::toHTML(printType type) const {
	switch(type) {
		case printBrief: return toBriefHTML();
	 	case printVerbose: return toVerboseHTML();
		case printAuto:
		default:
			if(favoredPrintType != printAuto)
				return toHTML(favoredPrintType);
			else
				return toBriefHTML();
	}
}

/** This method should return the entry object in a simple QString format
  Brief form should be useable in quick summaries, for example
  Verbose form might be used to save a complete list of entries to a file,
  for example */
QString Entry::toString(printType type) const {
	switch(type) {
		case printBrief: return toBriefText();
	 	case printVerbose: return toVerboseText();
		default:
			if(favoredPrintType != printAuto)
				return toString(favoredPrintType);
			else
				return toBriefText();
	}
}

/**This method allows the creator of the object to 
  "register their vote" on how the object should eventually
  be displayed. It may be ignored later on though 
 If the print methods are called using printAuto, this
 value will be respected. If this value is not set, the
 object defaults to printBrief */
void Entry::setFavoredPrintType(const printType type) {
	favoredPrintType = type;
}

/**Fetch the information set in setFavoredPrintType */
Entry::printType Entry::getFavoredPrintType() const {
	return favoredPrintType;
}


/* New functions for Entry doing direct display */
/** Creates a one character link for the given @p entryCharacter. */
inline QString Entry::makeLink(const QChar entryCharacter) const
{
	return makeLink(QString(entryCharacter));
}

/** Creates a link for the given @p entryString. */
inline QString Entry::makeLink(const QString entryString) const
{
	return "<a href=\"" + entryString + "\">" + entryString + "</a>";
}

/** Determines whether @p characeter is a kanji character. */
bool Entry::isKanji(const QChar character) const
{
	ushort value = character.unicode();
	if(value < 255) return false;
	if(0x3040 <= value && value <= 0x30FF)
		return false; //Kana
	return true; //Note our folly here... we assuming any non-ascii/kana is kanji
}

/* New functions for direct display from Entry */
/** returns a brief HTML version of an Entry */
inline QString Entry::toBriefHTML() const
{
	return "<div class=\"EntryBrief\">" + HTMLWord() + HTMLReadings() + 
		HTMLMeanings() + "</div>"; 
}

/** returns an HTML version of an Entry that is rather complete.*/
inline QString Entry::toVerboseHTML() const
{
	return "<div class=\"EntryVerbose\">" + HTMLWord() + HTMLReadings() + 
		HTMLMeanings() + "</div>";
}

/** Prepares Word for output as HTML */
inline QString Entry::HTMLWord() const
{
	return "<span class=\"Word\">" + Word + "</span>";
}

/** Prepares Readings for output as HTML */
inline QString Entry::HTMLReadings() const
{
	QStringList copy = Readings;
	for(QStringList::iterator it = copy.begin(); it != copy.end();++it) //KDE4 CHANGE
		*it = makeLink(*it);
	return "<span class=\"Meanings\">"+copy.join(outputListDelimiter)+"</span>";
}

/** Prepares Meanings for output as HTML */
inline QString Entry::HTMLMeanings() const
{
	return "<span class=\"Meanings\">" + Meanings.join(outputListDelimiter)
		+ "</span>";
}

/** Creates a brief textual description of an Entry */
inline QString Entry::toBriefText() const
{
	return Word;
}

inline QString Entry::toVerboseText() const
{
	return Word + " (" + getReadings() + ") " + getMeanings();
}

bool Entry::matchesQuery(const dictQuery &query) const {
	if(!query.getWord().isEmpty()) {
	/*	if(query.getMatchType() == dictQuery::matchExact &&
				this->getWord() != query.getWord())
				return false;
		if(query.getMatchType() == dictQuery::matchBeginning &&
				!this->getWord().startsWith(query.getWord()))
				return false;
		if(query.getMatchType() == dictQuery::matchEnd &&
				!this->getWord().endsWith(query.getWord()))
				return false;
		if(query.getMatchType() == dictQuery::matchAnywhere &&
	*/ if(!this->getWord().contains(query.getWord()))
				return false;
	}

	if(!query.getMeaning().isEmpty())
		if(!listMatch(Meanings.join(" "),
						query.getMeaning().split(dictQuery::mainDelimiter) ) )
			return false;
		
	if(!query.getPronounciation().isEmpty())
		if(!listMatch(Readings.join(" "),
					query.getPronounciation().split(dictQuery::mainDelimiter) ) )
			return false;
	
	dictQuery::Iterator it(query);
	for(; it.current(); ++it) {
		QString extendedItem = getExtendedInfoItem(it.currentKey());
		if( extendedItem != *it.current() )
			return false;
	}
	return true;	
	
}

//Returns true if all members of test are in list
bool Entry::listMatch(const QString &list, const QStringList &test) const {
	for(QStringList::const_iterator it = test.constBegin(); it != test.constEnd();++it) //KDE4 CHANGE
		if(!list.contains(*it))
			return false;
	return true;
}
