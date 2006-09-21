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

#include "dictQuery.h"

#include <QtCore/QStringList>
#include <QtCore/QString>

#include <kdebug.h>



/*****************************************************************************
*	Constructors, Destructors, Initilizers, and 
*	Global Status Indicators.
*****************************************************************************/
dictQuery::dictQuery() : QHash<QString,QString>()
{
	init();
}

dictQuery::dictQuery(const QString& str) : QHash<QString,QString>() {
	kDebug() << "dictQuery Constructor: " << str << endl;
	init();
	this->operator=((QString)str);
}

dictQuery::dictQuery(const dictQuery& orig):QHash<QString,QString>(orig) {
	init();
	this->operator=((dictQuery&)orig);
}

void dictQuery::init() {
	matchType=matchExact;
}

dictQuery::~dictQuery()
{
}

bool dictQuery::isEmpty() const {
//We're only empty if the two strings are empty too
	return QHash<QString,QString>::isEmpty() && meaning.isEmpty()
		&& pronunciation.isEmpty() && word.isEmpty();
}
void dictQuery::clear() {
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
dictQuery &dictQuery::operator=(const dictQuery &d) {
	if ( &d == this ) return *this;
	clear();
 	//Copy the dictionary first
	dictQuery::Iterator it(d);
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

dictQuery &dictQuery::operator+=(const dictQuery &d) {
	return operator=(this->toString()+mainDelimiter+d.toString());
}

bool operator==( dictQuery other, dictQuery query ) {
	if( (other.pronunciation != query.pronunciation)
		|| (other.meaning != query.meaning) 
		|| (other.word != query.word)
		|| (other.entryOrder != query.entryOrder)
		|| (other.count() != query.count()) ) 
		return false;

	dictQuery::Iterator it( other );
	while(it.hasNext()) {
		it.next();
		if( it.value() != query[it.key()])
			return false;
	}
	return true;
}

bool operator<( dictQuery A, dictQuery B) {
	dictQuery::Iterator it( A );
	while(it.hasNext()) {
		//The default case is for properties to need
		//to match exactly in B, if present in A
		//There are plenty of exceptions here though
		it.next();
		if(it.key() == "R") {
			foreach( QString str, it.value().split("") )
				if(B.getProperty("R").contains(str)==0)
					return false;
		} else if( it.value() != B[it.key()])
			return false;
	}

	if(!A.pronunciation.isEmpty()) {
		QStringList aList = A.pronunciation.split(dictQuery::mainDelimiter);
		QStringList bList = B.pronunciation.split(dictQuery::mainDelimiter);
		foreach( QString str, aList )
			if(bList.contains(str)==0)
				return false;
	}
		
	if(!A.meaning.isEmpty()) {
		QStringList aList = A.meaning.split(dictQuery::mainDelimiter);
		QStringList bList = B.meaning.split(dictQuery::mainDelimiter);
		foreach( QString str, aList )
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
const QString dictQuery::toString() const {
	if(isEmpty())
		return QString();

	QString reply;
	foreach( QString it, entryOrder ) {
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

dictQuery &dictQuery::operator=(const QString &str) {
	QStringList parts = str.split(mainDelimiter);
	dictQuery result;
	foreach( QString it, parts) {
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
			case dictQuery::strTypeLatin :
				if(result.entryOrder.removeAll(meaningMarker) > 0 )
					result.setMeaning(result.getMeaning() + mainDelimiter + it);
				else
					result.setMeaning(it);
				break;
			case dictQuery::strTypeKana :
				if(result.entryOrder.removeAll(pronunciationMarker)>0)
					result.setPronounciation(result.getPronounciation() 
					                                     + mainDelimiter + it );
				else
					result.setPronounciation(it);
				break;

			case dictQuery::strTypeKanji :
				result.entryOrder.removeAll(wordMarker);
				result.setWord( it ); //Only one of these allowed
				break;

			case dictQuery::mixed :
				qWarning("mixed wtf?");
			case dictQuery::stringParseError :
				qWarning("wtf?");
#ifdef USING_QUERY_EXCEPTIONS
				throw invalidQueryException(it);
#endif
				break;
		}
	}
	kDebug() << "Query: ("<<result.getWord() << ") ["<<result.getPronounciation()<<"] :"<<
		result.getMeaning()<<endl;
	this->operator=(result);
	return *this;
}
//Private utility method for the above... confirms that an entire string
//is either completely japanese or completely english
dictQuery::stringTypeEnum dictQuery::stringTypeCheck(QString in) {
	stringTypeEnum firstType;
	//Split into individual characters
	if(in.size() <= 0)
		return dictQuery::stringParseError;
	
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
				return dictQuery::mixed;
			}
		}
	}
	return firstType;
}
//Private utility method for the stringTypeCheck
//Just checks and returns the type of the first character in the string
//that is passed to it.
dictQuery::stringTypeEnum dictQuery::charTypeCheck(QChar ch) {
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
*	An array of Property List accessors
*	
*****************************************************************************/
QString dictQuery::getProperties() const {
	QStringList copy = getPropertiesList();
	return copy.join(mainDelimiter);
}

QStringList dictQuery::getPropertiesList() const {
	QStringList copy = getPropertyKeysList();
	QStringList result;
	for ( QStringList::Iterator it = copy.begin(); it != copy.end(); ++it )
		result.append(*it+propertySeperator+*this->find(*it));
	return result;
}

QString dictQuery::getPropertyKeys() const {
	QStringList copy = getPropertyKeysList();
	return copy.join(mainDelimiter);
}
QStringList dictQuery::getPropertyKeysList() const {
	QStringList copy = entryOrder;
	copy.removeAll(pronunciationMarker);
	copy.removeAll(meaningMarker);
	return copy;
}

QString dictQuery::getProperty(const QString &key) const {
	if(!this->contains(key))
		return QString(QString::null);
	return this->find(key).value();
}

bool dictQuery::hasProperty(const QString &key) const {
	return entryOrder.contains(key)>0;
}

/**************************************************************
*	Mutators for the Properties (including QDict overrides)
**************************************************************/
//TODO: Add i18n handling and alternate versions of property names
bool dictQuery::setProperty(const QString& key,const QString& value) {
	return replace(key,value);
}

bool dictQuery::removeProperty(const QString &key){
	return remove(key);
}

bool dictQuery::insert(const QString& key, const QString& item){
	if(key==pronunciationMarker || key==meaningMarker |
		key.isEmpty() || item.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(key+propertySeperator+item);
#else
		return false;
#endif
	if ( ! QHash<QString,QString>::contains( key ) )
		entryOrder.append(QString(key));
	QHash<QString,QString>::insert( key, item );
	return true;
}

bool dictQuery::replace(const QString& key, const QString& item){
	if(key==pronunciationMarker || key==meaningMarker ||
		key.isEmpty() || item.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(key+propertySeperator+item);
#else
		return false;
#endif
	if ( ! QHash<QString,QString>::contains( key ) )
		entryOrder.append(key);
	QHash<QString,QString>::insert(key,item);
	return true;
}

bool dictQuery::remove(const QString& key) {
	if(QHash<QString,QString>::contains(key))
		return entryOrder.removeAll(key);
	return false;
}

QString* dictQuery::take ( const QString & key ) {
	entryOrder.removeAll(key);
	return take(key);
}

/*****************************************************************************
*	Meaning and Pronounciation Accessors and Mutators
****************************************************************************/
QString dictQuery::getMeaning() const {
	return meaning;
}

bool dictQuery::setMeaning(const QString &newMeaning) {
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

QString dictQuery::getPronounciation() const {
	return pronunciation;
}

bool dictQuery::setPronounciation(const QString &newPro) {
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

QString dictQuery::getWord() const{
	return word;
}

bool dictQuery::setWord(const QString &newWord) {
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
QStringList dictQuery::getDictionaries() const {
	return targetDictionaries;
}

void dictQuery::setDictionaries(const QStringList newDictionaries) {
	targetDictionaries = newDictionaries;
}

/**************************************************************
  Match Type Accessors and Mutators
  ************************************************************/
dictQuery::matchTypeSettings dictQuery::getMatchType() const {
	return matchType;
}

void dictQuery::setMatchType(matchTypeSettings newType) {
	matchType = newType;
}

/**************************************************************
*	Aliases to handle different forms of operator arguments
**************************************************************/
inline bool operator==( QString other, dictQuery query ) {
	dictQuery x(other); return x == query;
}
inline bool operator==( dictQuery query, QString other ) {
	return other==query;
}
inline bool operator!=( dictQuery q1, dictQuery q2 ) {
	return !(q1==q2);
}
inline bool operator!=( QString other, dictQuery query ) {
	return !(other==query);
}
inline bool operator!=( dictQuery query, QString other ) {
	return !(query==other);
}
inline bool operator<=( dictQuery a, dictQuery b) {
	return (a<b || a==b);
}
inline bool operator>=( dictQuery a, dictQuery b) {
	return (a>b || a==b);
}
inline bool operator>( dictQuery a, dictQuery b) {
	return b < a; 
}
dictQuery &operator+( const dictQuery &a, const dictQuery &b) {
	return (*(new dictQuery(a))) += b;
}
dictQuery &operator+( const dictQuery &a, const QString &b) {
	return (*(new dictQuery(a))) += b;
}
dictQuery &operator+( const QString &a,   const dictQuery &b)  {
	return (*(new dictQuery(a))) += b;
}
dictQuery    &dictQuery::operator+=(const QString &str) {
	dictQuery x(str);
	return operator+=(x);
}
#ifndef QT_NO_CAST_ASCII
dictQuery    &dictQuery::operator=(const char *str) {
	QString x(str);
	return operator=(x);
}
dictQuery    &dictQuery::operator+=(const char *str) {
	dictQuery x(str);
	return operator+=(x);
}
#endif

/**************************************************************
*	Set our constants declared in the class
**************************************************************/
const QString dictQuery::pronunciationMarker("__@\\p");
const QString dictQuery::meaningMarker("__@\\m"); 
const QString dictQuery::wordMarker("_@\\w");
const QString dictQuery::mainDelimiter(" ");
const QString dictQuery::propertySeperator(":");
