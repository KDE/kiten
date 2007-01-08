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

#include "dictQuery.h"
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
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result;
	for (unsigned int i = 0; i < max; ++i)
	{
		Entry *it = at(i);
		if(length-- > 0)
			result = result + "<div class=\"Entry\" index=\"" +
				QString::number(i) + "\" dict=\"" + it->getDictName()
				+ "\">" + it->toHTML(type) + "</div>";
		else
			break;

	}
	return result;
}

QString EntryList::toKVTML(unsigned int start, unsigned int length) const {
	unsigned int max = count();
	if(start > max) return QString();
	if(start+length > max) length = max-start;

	QString result = "<?xml version=\"1.0\"?>\n<!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">\n"
		"<kvtml encoding=\"UTF-8\" "
		" generator=\"kiten v42.0\""
		" title=\"To be determined\">\n";
	foreach(Entry *it, *this) {
		if(length-- > 0)
			result = result + it->toKVTML() + "\n";
		else
			break;
	}
	return result +"</kvtml>\n";
}

QString EntryList::toHTML(Entry::printType type) const {
	return toHTML(0,count(),type);
}

/** Returns the EntryList as HTML */
//TODO: Some intelligent decision making... regarding the output format (differ for
//different types of searches?
QString EntryList::toString(unsigned int start, unsigned int length, Entry::printType type) const {
	unsigned int max = count();
	if(start > max) return QString();
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

/** sorts the EntryList in a C++ish, thread-safe manner. */
class sortFunctor {
	public:
		QStringList *dictionary_order;
		QStringList *sort_order;
		bool operator() (const Entry *n1, const Entry *n2) const {
			return n1->sort(*n2,*dictionary_order,*sort_order);
		}
};

void EntryList::sort(QStringList &sortOrder, QStringList &dictionaryOrder) {
	sortFunctor sorter;
	sorter.dictionary_order = &dictionaryOrder;
	sorter.sort_order = &sortOrder;

	qSort(this->begin(),this->end(),sorter);
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

	if(!query.getPronunciation().isEmpty())
		if(!listMatch(Readings.join(" "),
					query.getPronunciation().split(dictQuery::mainDelimiter) ) )
			return false;

	dictQuery::Iterator it = query.getPropertyIterator();
	while(it.hasNext()) {
		it.next();
		QString extendedItem = getExtendedInfoItem(it.key());
		//if( !extendedItemCheck(it.key(), it.value()))
			//return false;

	}

	return true;

}

/*
bool Entry::extendedItemCheck(const dictQuery& query)
{

}
*/

//Returns true if all members of test are in list
bool Entry::listMatch(const QString &list, const QStringList &test) const {
	foreach(QString it, test)
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
		foreach(QString dict, dictOrder) {
			if(dict == that.sourceDict)
				return false;
			if(dict == this->sourceDict)
				return true;
		}
	} else {
		foreach(QString field, fields) {
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

/* TYPES RELATED FUNCTIONS */

QHash<QString, Entry::WordType> *Entry::wordTypes = NULL;

QHash<QString, Entry::WordType>* Entry::WordTypes()
{
	if (wordTypes == NULL)
	{
		wordTypes = new QHash<QString, Entry::WordType>;
		(*wordTypes)["adj"] = adj;  /* 	adjective (keiyoushi) */
		(*wordTypes)["adj-na"] = adj_na;  /* 	adjectival nouns or quasi-adjectives (keiyodoshi) */
		(*wordTypes)["adj-no"] = adj_no;  /* 	nouns which may take the genitive case particle `no' */
		(*wordTypes)["adj-pn"] = adj_pn;  /* 	pre-noun adjectival (rentaishi) */
		(*wordTypes)["adj-t"] = adj_t;  /* 	`taru' adjective */
		(*wordTypes)["adv"] = adv;  /* 	adverb (fukushi) */
		(*wordTypes)["adv-n"] = adv_n;  /* 	adverbial noun */
		(*wordTypes)["adv-to"] = adv_to;  /* 	adverb taking the `to' particle */
		(*wordTypes)["aux"] = aux;  /* 	auxiliary */
		(*wordTypes)["aux-v"] = aux_v;  /* 	auxiliary verb */
		(*wordTypes)["aux-adj"] = aux_adj;  /* 	auxiliary adjective */
		(*wordTypes)["conj"] = conj;  /* 	conjunction */
		(*wordTypes)["exp"] = exp; /* 	Expressions (phrases, clauses;  etc.) */
		(*wordTypes)["id"] = id;  /* 	idiomatic expression */
		(*wordTypes)["inte"] = inte;  /* 	interjection (kandoushi)
										(*wordTypes)["int is a keyword"] = int is a keyword;  thus 'inte' instead */
		(*wordTypes)["iv"] = iv;  /* 	irregular verb */
		(*wordTypes)["n"] = n;  /* 	noun (common) (futsuumeishi) */
		(*wordTypes)["n-adv"] = n_adv;  /* 	adverbial noun (fukushitekimeishi) */
		(*wordTypes)["n-pref"] = n_pref; /* 	noun;  used as a prefix */
		(*wordTypes)["n-suf"] = n_suf; /* 	noun;  used as a suffix */
		(*wordTypes)["n-t"] = n_t;  /* 	noun (temporal) (jisoumeishi) */
		(*wordTypes)["neg"] = neg; /* 	negative (in a negative sentence;  or with negative verb) */
		(*wordTypes)["neg-v"] = neg_v;  /* 	negative verb (when used with) */
		(*wordTypes)["num"] = num;  /* 	numeric */
		(*wordTypes)["pref"] = pref;  /* 	prefix */
		(*wordTypes)["prt"] = prt;  /* 	particle */
		(*wordTypes)["suf"] = suf;  /* 	suffix */
		(*wordTypes)["v1"] = v1;  /* 	Ichidan verb */
		(*wordTypes)["v5"] = v5;  /* 	Godan verb (not completely classified) */
		(*wordTypes)["v5aru"] = v5aru;  /* 	Godan verb - -aru special class */
		(*wordTypes)["v5b"] = v5b;  /* 	Godan verb with `bu' ending */
		(*wordTypes)["v5g"] = v5g;  /* 	Godan verb with `gu' ending */
		(*wordTypes)["v5k"] = v5k;  /* 	Godan verb with `ku' ending */
		(*wordTypes)["v5k-s"] = v5k_s;  /* 	Godan verb - iku/yuku special class */
		(*wordTypes)["v5m"] = v5m;  /* 	Godan verb with `mu' ending */
		(*wordTypes)["v5n"] = v5n;  /* 	Godan verb with `nu' ending */
		(*wordTypes)["v5r"] = v5r;  /* 	Godan verb with `ru' ending */
		(*wordTypes)["v5r-i"] = v5r_i;  /* 	Godan verb with `ru' ending (irregular verb) */
		(*wordTypes)["v5s"] = v5s;  /* 	Godan verb with `su' ending */
		(*wordTypes)["v5t"] = v5t;  /* 	Godan verb with `tsu' ending */
		(*wordTypes)["v5u"] = v5u;  /* 	Godan verb with `u' ending */
		(*wordTypes)["v5u-s"] = v5u_s;  /* 	Godan verb with `u' ending (special class) */
		(*wordTypes)["v5uru"] = v5uru;  /* 	Godan verb - uru old class verb (old form of Eru) */
		(*wordTypes)["vi"] = vi;  /* 	intransitive verb */
		(*wordTypes)["vk"] = vk;  /* 	kuru verb - special class */
		(*wordTypes)["vs"] = vs;  /* 	noun or participle which takes the aux. verb suru */
		(*wordTypes)["vs-i"] = vs_i;  /* 	suru verb - irregular */
		(*wordTypes)["vs-s"] = vs_s;  /* 	suru verb - special class */
		(*wordTypes)["vt"] = vt;  /* 	transitive verb */
		(*wordTypes)["vz"] = vz;  /* 	zuru verb - (alternative form of -jiru verbs) */
	}
	return wordTypes;
}


QMultiHash<QString, Entry::WordType> *Entry::wordTypesPretty = NULL;

QMultiHash<QString, Entry::WordType>* Entry::WordTypesPretty()
{
	if (wordTypesPretty == NULL)
	{
		wordTypesPretty = new QMultiHash<QString, Entry::WordType>;
		wordTypesPretty->insert("Verb",  v1);

		//TODO: i18n needed
		wordTypesPretty->insert("Verb",  v5);
		wordTypesPretty->insert("Verb",  v5aru);
		wordTypesPretty->insert("Verb",  v5b);
		wordTypesPretty->insert("Verb",  v5g);
		wordTypesPretty->insert("Verb",  v5k);
		wordTypesPretty->insert("Verb",  v5k_s);
		wordTypesPretty->insert("Verb",  v5m);
		wordTypesPretty->insert("Verb",  v5n);
		wordTypesPretty->insert("Verb",  v5r);
		wordTypesPretty->insert("Verb",  v5r_i);
		wordTypesPretty->insert("Verb",  v5s);
		wordTypesPretty->insert("Verb",  v5t);
		wordTypesPretty->insert("Verb",  v5u);
		wordTypesPretty->insert("Verb",  v5u_s);
		wordTypesPretty->insert("Verb",  v5uru);
		wordTypesPretty->insert("Verb",  vi);
		wordTypesPretty->insert("Verb",  vk);
		wordTypesPretty->insert("Verb",  vs);
		wordTypesPretty->insert("Verb",  vs_i);
		wordTypesPretty->insert("Verb",  vs_s);
		wordTypesPretty->insert("Verb",  vt);
		wordTypesPretty->insert("Verb",  vz);

		wordTypesPretty->insert("Noun",  n);
		wordTypesPretty->insert("Noun",  n_adv);
		wordTypesPretty->insert("Noun",  n_pref);
		wordTypesPretty->insert("Noun",  n_suf);
		wordTypesPretty->insert("Noun",  n_t);
		wordTypesPretty->insert("Noun",  adv_n);
	}
	return wordTypesPretty;
}

