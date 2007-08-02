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

/*
TODO: Add features to limit the number of hits on a per-search basis.

	Add a mechanism (either through subclassing, or directly) for use
		for marking "requested" fields for the dcop system.
*/

#include "DictQuery.h"

#include <QtCore/QStringList>
#include <QtCore/QString>

#include <kdebug.h>

/*****************************************************************************
*	Constructors, Destructors, Initilizers, and
*	Global Status Indicators.
*****************************************************************************/
DictQuery::DictQuery() : QHash<QString,QString>()
{
	init();
}

DictQuery::DictQuery(const QString& str) : QHash<QString,QString>() {
	kDebug() << "DictQuery Constructor: " << str;
	init();
	this->operator=((QString)str);
}

DictQuery::DictQuery(const DictQuery& orig):QHash<QString,QString>(orig) {
	init();
	this->operator=((DictQuery&)orig);
}

void DictQuery::init() {
	matchType=matchExact;
}

DictQuery::~DictQuery()
{
}

bool DictQuery::isEmpty() const {
//We're only empty if the two strings are empty too
	return QHash<QString,QString>::isEmpty() && meaning.isEmpty()
		&& pronunciation.isEmpty() && word.isEmpty();
}
void DictQuery::clear() {
	QHash<QString,QString>::clear();
	meaning="";
	pronunciation="";
	word="";
	entryOrder.clear();
}

/*****************************************************************************
*	Methods that involve multiple instances of the class
*	(comparison, copy etc)
*****************************************************************************/
DictQuery &DictQuery::operator=(const DictQuery &d) {
	if ( &d == this ) return *this;
	clear();
	//Copy the dictionary first
	DictQuery::Iterator it(d);
	while(it.hasNext()) {
		it.next();
		QHash<QString,QString>::insert( it.key(),it.value() );
	}
	meaning=d.meaning;
	pronunciation=d.pronunciation;
	word = d.word;
	entryOrder = d.entryOrder;
	return *this;
}

DictQuery &DictQuery::operator+=(const DictQuery &d) {
	return operator=(this->toString()+mainDelimiter+d.toString());
}

bool operator==(const DictQuery &other, const DictQuery &query ) {
	if( (other.pronunciation != query.pronunciation)
		|| (other.meaning != query.meaning)
		|| (other.word != query.word)
		|| (other.entryOrder != query.entryOrder)
		|| (other.count() != query.count()) )
		return false;

	DictQuery::Iterator it( other );
	while(it.hasNext()) {
		it.next();
		if( it.value() != query[it.key()])
			return false;
	}
	return true;
}

bool operator<(const DictQuery &A, const DictQuery &B) {
	DictQuery::Iterator it( A );
	while(it.hasNext()) {
		//The default case is for properties to need
		//to match exactly in B, if present in A
		//There are plenty of exceptions here though
		it.next();
		if(it.key() == "R") {
			foreach( const QString &str, it.value().split("") )
				if(B.getProperty("R").contains(str)==0)
					return false;
		} else if( it.value() != B[it.key()])
			return false;
	}

	if(!A.pronunciation.isEmpty()) {
		QStringList aList = A.pronunciation.split(DictQuery::mainDelimiter);
		QStringList bList = B.pronunciation.split(DictQuery::mainDelimiter);
		foreach( const QString &str, aList )
			if(bList.contains(str)==0)
				return false;
	}

	if(!A.meaning.isEmpty()) {
		QStringList aList = A.meaning.split(DictQuery::mainDelimiter);
		QStringList bList = B.meaning.split(DictQuery::mainDelimiter);
		foreach( const QString &str, aList )
			if(bList.contains(str)==0)
				return false;
	}

	//Assume only one entry for word
	if(!A.word.isEmpty())
		if(A.word != B.word)
			return false;

	return true;
}


/*****************************************************************************
*	Methods to extract from QStrings and recreate QStrings
*
*****************************************************************************/
const QString DictQuery::toString() const {
	if(isEmpty())
		return QString();

	QString reply;
	foreach( const QString &it, entryOrder ) {
		if(it == pronunciationMarker)
			reply += pronunciation+mainDelimiter;
		else if(it == meaningMarker)
			reply += meaning+mainDelimiter;
		else if(it == wordMarker)
			reply += word+mainDelimiter;
		else
			reply += it + propertySeperator
				+ *this->find(it) + mainDelimiter;
	}
	reply.truncate(reply.length()-mainDelimiter.length());

	return reply;
}

DictQuery &DictQuery::operator=(const QString &str) {
	QStringList parts = str.split(mainDelimiter);
	DictQuery result;
	if(str.length() > 0)
		foreach( const QString &it, parts) {
			if(it.contains(propertySeperator)) {
				QStringList prop = it.split(propertySeperator);
				if(prop.count() != 2)
#ifdef USING_QUERY_EXCEPTIONS
					throw invalidQueryException(it);
#else
					break;
#endif
				result.setProperty(prop[0],prop[1]);
				//replace or throw an error with duplicates?
			} else switch(stringTypeCheck(it)) {
				case DictQuery::strTypeLatin :
					if(result.entryOrder.removeAll(meaningMarker) > 0 )
						result.setMeaning(result.getMeaning() + mainDelimiter + it);
					else
						result.setMeaning(it);
					break;
				case DictQuery::strTypeKana :
					if(result.entryOrder.removeAll(pronunciationMarker)>0)
						result.setPronunciation(result.getPronunciation()
																		 + mainDelimiter + it );
					else
						result.setPronunciation(it);
					break;

				case DictQuery::strTypeKanji :
					result.entryOrder.removeAll(wordMarker);
					result.setWord( it ); //Only one of these allowed
					break;

				case DictQuery::mixed :
					qWarning("DictQuery: String parsing error - mixed type");
				case DictQuery::stringParseError :
					qWarning("DictQuery: String parsing error");
#ifdef USING_QUERY_EXCEPTIONS
					throw invalidQueryException(it);
#endif
					break;
			}
		}
	kDebug() << "Query: ("<<result.getWord() << ") ["<<result.getPronunciation()<<"] :"<<
		result.getMeaning()<<endl;
	this->operator=(result);
	return *this;
}
//Private utility method for the above... confirms that an entire string
//is either completely japanese or completely english
DictQuery::stringTypeEnum DictQuery::stringTypeCheck(const QString &in) {
	stringTypeEnum firstType;
	//Split into individual characters
	if(in.size() <= 0)
		return DictQuery::stringParseError;

	firstType = charTypeCheck(in.at(0));
	for(int i=1; i<in.size(); i++ ){
		stringTypeEnum newType = charTypeCheck(in.at(i));
		if(newType != firstType) {
			if(firstType == strTypeKana && newType == strTypeKanji) {
				firstType = strTypeKanji;
			}
			else if(firstType == strTypeKanji && newType == strTypeKana)
				; //That's okay
			else
			{
				return DictQuery::mixed;
			}
		}
	}
	return firstType;
}
//Private utility method for the stringTypeCheck
//Just checks and returns the type of the first character in the string
//that is passed to it.
DictQuery::stringTypeEnum DictQuery::charTypeCheck(const QChar &ch) {
	if(ch.toLatin1()) {
		return strTypeLatin;
	}
	//The unicode character boundaries are:
	// 3040 - 309F Hiragana
	// 30A0 - 30FF Katakana
	// 31F0 - 31FF Katakana phonetic expressions (wtf?)
	if((ch.unicode() >= 0x3040 && ch.unicode() <= 0x30FF) /*|| ch.unicode() & 0x31F0*/)
		return strTypeKana;
	return strTypeKanji;
}

/*****************************************************************************
*	An array of Property List accessors and mutators
*
*****************************************************************************/
QHashIterator<QString,QString> DictQuery::getPropertyIterator() const {
		QHashIterator<QString,QString> it(*this);
		return it;
}
QStringList DictQuery::getPropertyList() const {
	QStringList copy = getPropertyKeysList();
	QStringList result;
	for ( QStringList::Iterator it = copy.begin(); it != copy.end(); ++it )
		result.append(*it+propertySeperator+*this->find(*it));
	return result;
}

QStringList DictQuery::getPropertyKeysList() const {
	QStringList copy = entryOrder;
	copy.removeAll(pronunciationMarker);
	copy.removeAll(meaningMarker);
	return copy;
}

QString DictQuery::getProperty(const QString &key) const {
	return (*this)[key];
}

bool DictQuery::hasProperty(const QString &key) const {
	return entryOrder.contains(key)>0;
}

//TODO: Add i18n handling and alternate versions of property names
//TODO: further break down the barrier between different types
bool DictQuery::setProperty(const QString& key,const QString& value) {
	if(key==pronunciationMarker || key==meaningMarker ||
		key.isEmpty() || value.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(key+propertySeperator+value);
#else
		return false;
#endif
	if ( ! QHash<QString,QString>::contains( key ) )
		entryOrder.append(key);
	QHash<QString,QString>::insert(key,value);
	return true;
}

bool DictQuery::removeProperty(const QString &key) {
	if(QHash<QString,QString>::contains(key))
		return entryOrder.removeAll(key);
	return false;
}

QString DictQuery::takeProperty ( const QString & key ) {
	entryOrder.removeAll(key);
	return take(key);
}

/*****************************************************************************
*	Meaning and Pronunciation Accessors and Mutators
****************************************************************************/
QString DictQuery::getMeaning() const {
	return meaning;
}

bool DictQuery::setMeaning(const QString &newMeaning) {
	if(newMeaning.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newMeaning);
#else
		return false;
#endif
	meaning=newMeaning;
	if(!entryOrder.contains(meaningMarker))
		entryOrder.append(meaningMarker);
	return true;
}

QString DictQuery::getPronunciation() const {
	return pronunciation;
}

bool DictQuery::setPronunciation(const QString &newPro) {
	if(newPro.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newPro);
#else
		return false;
#endif
	pronunciation=newPro;
	if(!entryOrder.contains(pronunciationMarker))
		entryOrder.append(pronunciationMarker);
	return true;
}

QString DictQuery::getWord() const{
	return word;
}

bool DictQuery::setWord(const QString &newWord) {
	if(newWord.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newWord);
#else
		return false;
#endif
	word=newWord;
	if(!entryOrder.contains(wordMarker))
		entryOrder.append(wordMarker);
	return true;
}
/*************************************************************
  Handlers for getting and setting dictionary types
  *************************************************************/
QStringList DictQuery::getDictionaries() const {
	return targetDictionaries;
}

void DictQuery::setDictionaries(const QStringList &newDictionaries) {
	targetDictionaries = newDictionaries;
}

/**************************************************************
  Match Type Accessors and Mutators
  ************************************************************/
DictQuery::matchTypeSettings DictQuery::getMatchType() const {
	return matchType;
}

void DictQuery::setMatchType(matchTypeSettings newType) {
	matchType = newType;
}

/**************************************************************
*	Aliases to handle different forms of operator arguments
**************************************************************/
inline bool operator==( const QString &other, const DictQuery &query ) {
	DictQuery x(other); return x == query;
}
inline bool operator==( const DictQuery &query, const QString &other ) {
	return other==query;
}
inline bool operator!=( const DictQuery &q1, const DictQuery &q2 ) {
	return !(q1==q2);
}
inline bool operator!=( const QString &other, const DictQuery &query ) {
	return !(other==query);
}
inline bool operator!=( const DictQuery &query, const QString &other ) {
	return !(query==other);
}
inline bool operator<=( const DictQuery &a, const DictQuery &b) {
	return (a<b || a==b);
}
inline bool operator>=( const DictQuery &a, const DictQuery &b) {
	return (a>b || a==b);
}
inline bool operator>( const DictQuery &a, const DictQuery &b) {
	return b < a;
}
DictQuery &operator+( const DictQuery &a, const DictQuery &b) {
	return (*(new DictQuery(a))) += b;
}
DictQuery &operator+( const DictQuery &a, const QString &b) {
	return (*(new DictQuery(a))) += b;
}
DictQuery &operator+( const QString &a,   const DictQuery &b)  {
	return (*(new DictQuery(a))) += b;
}
DictQuery    &DictQuery::operator+=(const QString &str) {
	DictQuery x(str);
	return operator+=(x);
}
#ifndef QT_NO_CAST_ASCII
DictQuery    &DictQuery::operator=(const char *str) {
	QString x(str);
	return operator=(x);
}
DictQuery    &DictQuery::operator+=(const char *str) {
	DictQuery x(str);
	return operator+=(x);
}
#endif

/**************************************************************
*	Set our constants declared in the class
**************************************************************/
const QString DictQuery::pronunciationMarker("__@\\p");
const QString DictQuery::meaningMarker("__@\\m");
const QString DictQuery::wordMarker("_@\\w");
const QString DictQuery::mainDelimiter(" ");
const QString DictQuery::propertySeperator(":");
