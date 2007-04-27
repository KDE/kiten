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
#include <k3process.h>
#include <kstandarddirs.h>

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextcodec.h>

#include "DictQuery.h"
#include "Entry.h"

#include <iostream>
#include <cassert>
#include <sys/mman.h>
#include <stdio.h>


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
	: Word(word)
	, Meanings(meanings)
	, Readings(reading)
	, sourceDict(sourceDictionary)
{
	init();
}

Entry::Entry(const QString &sourceDictionary, const QString &word,
		const QStringList &readings, const QStringList &meanings,
		const QHash<QString,QString> &extendedInfo)
	: Word(word)
	, Meanings(meanings)
	, Readings(readings)
	, ExtendedInfo(extendedInfo)
	, sourceDict(sourceDictionary)
{
	init();
}

Entry::Entry(const Entry &src)
	: Word(src.Word)
	, Meanings(src.Meanings)
	, Readings(src.Readings)
	, ExtendedInfo(src.ExtendedInfo)
	, sourceDict(src.sourceDict)
{
	outputListDelimiter=src.outputListDelimiter;
	favoredPrintType = src.favoredPrintType;
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
		case printBrief:
			return "<div class=\"EntryBrief\">" + HTMLWord() +
				HTMLReadings() + HTMLMeanings() + "</div>";
		case printVerbose:
			return "<div class=\"EntryVerbose\">" + HTMLWord() +
				HTMLReadings() + HTMLMeanings() + "</div>";
		default:
			if(favoredPrintType != printAuto)
				return toHTML(favoredPrintType);
			else
				return toHTML(printBrief);
	}
}

/** This method should return the entry object in a simple QString format
  Brief form should be useable in quick summaries, for example
  Verbose form might be used to save a complete list of entries to a file,
  for example */
QString Entry::toString(printType type) const {
	switch(type) {
		case printBrief:
			return Word;
		case printVerbose:
			return Word + " (" + getReadings() + ") " + getMeanings();
		default:
			if(favoredPrintType != printAuto)
				return toString(favoredPrintType);
			else
				return toString(printBrief);
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

inline QString Entry::toKVTML() const
{
	/*
   <e m="1" s="1">
	   <o width="414" l="en" q="t">(eh,) excuse me</o>
	   <t width="417" l="jp" q="o">(あのう、) すみません </t>
   </e>
   */
	//TODO: en should not necessarily be the language here.
	return "<e>\n<o l=\"en\">" + getMeanings() + "</o>\n"
		"<t l=\"jp-kanji\">" + getWord() + "</t>\n" +
		"<t l=\"jp-kana\">" + getReadings() + "</t></e>\n\n";
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
	return "<span class=\"Readings\">"+copy.join(outputListDelimiter)+"</span>";
}

/** Prepares Meanings for output as HTML */
inline QString Entry::HTMLMeanings() const
{
	return "<span class=\"Meanings\">" + Meanings.join(outputListDelimiter)
		+ "</span>";
}

bool Entry::matchesQuery(const DictQuery &query) const {
	if(!query.getWord().isEmpty()) {
	/*	if(query.getMatchType() == DictQuery::matchExact &&
				this->getWord() != query.getWord())
				return false;
		if(query.getMatchType() == DictQuery::matchBeginning &&
				!this->getWord().startsWith(query.getWord()))
				return false;
		if(query.getMatchType() == DictQuery::matchEnd &&
				!this->getWord().endsWith(query.getWord()))
				return false;
		if(query.getMatchType() == DictQuery::matchAnywhere &&
	*/ if(!this->getWord().contains(query.getWord()))
				return false;
	}

	if(!query.getMeaning().isEmpty())
		if(!listMatch(Meanings.join(" "),
						query.getMeaning().split(DictQuery::mainDelimiter) ) )
			return false;

	if(!query.getPronunciation().isEmpty())
		if(!listMatch(Readings.join(" "),
					query.getPronunciation().split(DictQuery::mainDelimiter) ) )
			return false;

	DictQuery::Iterator it = query.getPropertyIterator();
	while(it.hasNext()) {
		it.next();
		QString extendedItem = getExtendedInfoItem(it.key());
		//if( !extendedItemCheck(it.key(), it.value()))
			//return false;

	}

	return true;

}

/*
bool Entry::extendedItemCheck(const DictQuery& query)
{

}
*/

//Returns true if all members of test are in list
bool Entry::listMatch(const QString &list, const QStringList &test) const {
	foreach(const QString &it, test)
		if(!list.contains(it))
			return false;
	return true;
}

/* This version of sort only sorts dictionaries...
	This is a replacement for a operator< function... so we return true if
	"this" should show up first on the list. */
bool Entry::sort(const Entry &that, const QStringList &dictOrder,
		const QStringList &fields) const {
	if(this->sourceDict != that.sourceDict) {
		foreach(const QString &dict, dictOrder) {
			if(dict == that.sourceDict)
				return false;
			if(dict == this->sourceDict)
				return true;
		}
	} else {
		foreach(const QString &field, fields) {
			if(this->getExtendedInfoItem(field) !=
					that.getExtendedInfoItem(field))
				return this->sortByField(that,field);
		}
	}
	return false; //If we reach here, they match as much as possible
}

bool Entry::sortByField(const Entry &that, const QString field) const {
	return this->getExtendedInfoItem(field) < that.getExtendedInfoItem(field);
}

