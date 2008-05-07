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
DictQuery::DictQuery()
{
	init();
}

DictQuery::DictQuery(const QString& str) {
	init();
	this->operator=((QString)str);
}

DictQuery::DictQuery(const DictQuery& orig) {
	init();
	this->operator=((DictQuery&)orig);
}

void DictQuery::init() {
	m_matchType=matchExact;
}

DictQuery *DictQuery::clone() const {
	return new DictQuery(*this);
}

DictQuery::operator QString() const {
	//kDebug() << "DictQuery toString operator called!";
	return toString();
}

DictQuery::~DictQuery()
{
}

bool DictQuery::isEmpty() const {
//We're only empty if the two strings are empty too
	return m_extendedAttributes.isEmpty() && m_meaning.isEmpty()
		&& m_pronunciation.isEmpty() && m_word.isEmpty();
}
void DictQuery::clear() {
	m_extendedAttributes.clear();
	m_meaning="";
	m_pronunciation="";
	m_word="";
	m_entryOrder.clear();
}

/*****************************************************************************
*	Methods that involve multiple instances of the class
*	(comparison, copy etc)
*****************************************************************************/
DictQuery &DictQuery::operator=(const DictQuery &d) {
	if ( &d == this ) return *this;
	clear();
	m_matchType = d.m_matchType;
	m_extendedAttributes = d.m_extendedAttributes;
	m_meaning=d.m_meaning;
	m_pronunciation=d.m_pronunciation;
	m_word = d.m_word;
	m_entryOrder = d.m_entryOrder;
	return *this;
}

DictQuery &DictQuery::operator+=(const DictQuery &d) {
	foreach(const QString &item, d.m_entryOrder) {
		if(item == meaningMarker) {
			if(m_entryOrder.removeAll(meaningMarker) > 0 )
				setMeaning(getMeaning() + mainDelimiter + d.getMeaning());
			else
				setMeaning(d.getMeaning());
		} else if(item == pronunciationMarker) {
			if(m_entryOrder.removeAll(pronunciationMarker)>0)
				setPronunciation(getPronunciation() + mainDelimiter + d.getPronunciation() );
			else
				setPronunciation(d.getPronunciation());
		} else if(item == wordMarker) {
			m_entryOrder.removeAll(wordMarker);
			setWord( d.getWord() ); //Only one of these allowed
		} else {
			setProperty(item,d.getProperty(item));
		}
	}
	return *this;
}

DictQuery operator+(const DictQuery &a, const DictQuery &b) {
	DictQuery val(a);
	val += b;
	return val;
}

bool operator==(const DictQuery &other, const DictQuery &query ) {
	if( (other.m_pronunciation != query.m_pronunciation)
		|| (other.m_meaning != query.m_meaning)
		|| (other.m_word != query.m_word)
		|| (other.m_entryOrder != query.m_entryOrder)
		|| (other.m_extendedAttributes != query.m_extendedAttributes)
		|| (other.m_matchType != query.m_matchType)
	  )
		return false;

	return true;
}
bool operator!=(const DictQuery &other, const DictQuery &query ) {
	return !(other == query);
}

bool operator<(const DictQuery &A, const DictQuery &B) {
	QHash<QString,QString>::const_iterator it = A.m_extendedAttributes.begin();
	QHash<QString,QString>::const_iterator it_end = A.m_extendedAttributes.end();
	for(;it != it_end; ++it) {
		QString B_version = B.m_extendedAttributes.value(it.key());
		if(A.m_extendedAttributes[it.key()] != B_version) {
			if(!B_version.contains(",") && !B_version.contains("-"))
				return false;
			//TODO: check for multi-values or ranges in DictQuery operator<
		}
	}

	if(!A.m_pronunciation.isEmpty()) {
		QStringList aList = A.m_pronunciation.split(DictQuery::mainDelimiter);
		QStringList bList = B.m_pronunciation.split(DictQuery::mainDelimiter);
		foreach( const QString &str, aList )
			if(bList.contains(str)==0)
				return false;
	}

	if(!A.m_meaning.isEmpty()) {
		QStringList aList = A.m_meaning.split(DictQuery::mainDelimiter);
		QStringList bList = B.m_meaning.split(DictQuery::mainDelimiter);
		foreach( const QString &str, aList )
			if(bList.contains(str)==0)
				return false;
	}

	//Assume only one entry for word
	if(!A.m_word.isEmpty())
		if(A.m_word != B.m_word)
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
	foreach( const QString &it, m_entryOrder ) {
		if(it == pronunciationMarker)
			reply += m_pronunciation+mainDelimiter;
		else if(it == meaningMarker)
			reply += m_meaning+mainDelimiter;
		else if(it == wordMarker)
			reply += m_word+mainDelimiter;
		else
			reply += it + propertySeperator + m_extendedAttributes.value(it)
				+ mainDelimiter;
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
					break;
				result.setProperty(prop[0],prop[1]);
				//replace or throw an error with duplicates?
			} else switch(stringTypeCheck(it)) {
				case DictQuery::strTypeLatin :
					if(result.m_entryOrder.removeAll(meaningMarker) > 0 )
						result.setMeaning(result.getMeaning() + mainDelimiter + it);
					else
						result.setMeaning(it);
					break;
				case DictQuery::strTypeKana :
					if(result.m_entryOrder.removeAll(pronunciationMarker)>0)
						result.setPronunciation(result.getPronunciation()
																		 + mainDelimiter + it );
					else
						result.setPronunciation(it);
					break;

				case DictQuery::strTypeKanji :
					result.m_entryOrder.removeAll(wordMarker);
					result.setWord( it ); //Only one of these allowed
					break;

				case DictQuery::mixed :
					kWarning() <<"DictQuery: String parsing error - mixed type";
					break;
				case DictQuery::stringParseError :
					kWarning() << "DictQuery: String parsing error";
			}
		}
	//kDebug() << "Query: ("<<result.getWord() << ") ["<<result.getPronunciation()<<"] :"<<
	//	result.getMeaning()<<endl;
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
	if(0x3040 <= ch.unicode() && ch.unicode() <= 0x30FF /*|| ch.unicode() & 0x31F0*/)
		return strTypeKana;
	return strTypeKanji;
}

/*****************************************************************************
*	An array of Property List accessors and mutators
*
*****************************************************************************/
QString DictQuery::getProperty(const QString &key) const {
	return (*this)[key];
}

const QList<QString> DictQuery::listPropertyKeys() const {
	return m_extendedAttributes.keys();
}
const QString DictQuery::operator[] (const QString &key) const {
	return m_extendedAttributes.value(key);
}
QString DictQuery::operator[] (const QString &key) {
	return m_extendedAttributes[key];
}

bool DictQuery::hasProperty(const QString &key) const {
	return m_entryOrder.contains(key)>0;
}

//TODO: Add i18n handling and alternate versions of property names
//TODO: further break down the barrier between different types
bool DictQuery::setProperty(const QString& key,const QString& value) {
	if(key==pronunciationMarker || key==meaningMarker ||
		key.isEmpty() || value.isEmpty())
		return false;
	if ( ! m_extendedAttributes.contains( key ) )
		m_entryOrder.append(key);
	m_extendedAttributes.insert(key,value);
	return true;
}

bool DictQuery::removeProperty(const QString &key) {
	if(m_extendedAttributes.contains(key))
		return m_entryOrder.removeAll(key);
	return false;
}

QString DictQuery::takeProperty ( const QString & key ) {
	m_entryOrder.removeAll(key);
	return m_extendedAttributes.take(key);
}

/*****************************************************************************
*	Meaning and Pronunciation Accessors and Mutators
****************************************************************************/
QString DictQuery::getMeaning() const {
	return m_meaning;
}

bool DictQuery::setMeaning(const QString &newMeaning) {
	if(newMeaning.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newMeaning);
#else
		return false;
#endif
	m_meaning=newMeaning;
	if(!m_entryOrder.contains(meaningMarker))
		m_entryOrder.append(meaningMarker);
	return true;
}

QString DictQuery::getPronunciation() const {
	return m_pronunciation;
}

bool DictQuery::setPronunciation(const QString &newPro) {
	if(newPro.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newPro);
#else
		return false;
#endif
	m_pronunciation=newPro;
	if(!m_entryOrder.contains(pronunciationMarker))
		m_entryOrder.append(pronunciationMarker);
	return true;
}

QString DictQuery::getWord() const{
	return m_word;
}

bool DictQuery::setWord(const QString &newWord) {
	if(newWord.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newWord);
#else
		return false;
#endif
	m_word=newWord;
	if(!m_entryOrder.contains(wordMarker))
		m_entryOrder.append(wordMarker);
	return true;
}
/*************************************************************
  Handlers for getting and setting dictionary types
  *************************************************************/
QStringList DictQuery::getDictionaries() const {
	return m_targetDictionaries;
}

void DictQuery::setDictionaries(const QStringList &newDictionaries) {
	m_targetDictionaries = newDictionaries;
}

/**************************************************************
  Match Type Accessors and Mutators
  ************************************************************/
DictQuery::MatchType DictQuery::getMatchType() const {
	return m_matchType;
}

void DictQuery::setMatchType(MatchType newType) {
	m_matchType = newType;
}

/**************************************************************
*	Aliases to handle different forms of operator arguments
*	Disabled at the moment
*************************************************************
bool operator==( const QString &other, const DictQuery &query ) {
	DictQuery x(other); return x == query;
}
bool operator==( const DictQuery &query, const QString &other ) {
	return other==query;
}
bool operator!=( const DictQuery &q1, const DictQuery &q2 ) {
	return !(q1==q2);
}
bool operator!=( const QString &other, const DictQuery &query ) {
	return !(other==query);
}
bool operator!=( const DictQuery &query, const QString &other ) {
	return !(query==other);
}
inline bool operator<=( const DictQuery &a, const DictQuery &b) {
	return (a<b || a==b);
}
bool operator>=( const DictQuery &a, const DictQuery &b) {
	return (b>a || a==b);
}
bool operator>( const DictQuery &a, const DictQuery &b) {
	return b < a;
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
*/
/**************************************************************
*	Set our constants declared in the class
**************************************************************/
const QString DictQuery::pronunciationMarker("__@\\p");
const QString DictQuery::meaningMarker("__@\\m");
const QString DictQuery::wordMarker("_@\\w");
const QString DictQuery::mainDelimiter(" ");
const QString DictQuery::propertySeperator(":");
