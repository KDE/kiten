/***************************************************************************
 *   Copyright (C) 2005 by Joseph Kerian   *
 *   jkerian@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/* 
TODO: Add features to limit the number of hits on a per-search basis.

	Add a mechanism (either through subclassing, or directly) for use
		for marking "requested" fields for the dcop system.
*/

#include "dictquery.h"

#include <q3dict.h>
#include <qstringlist.h>
#include <qstring.h>
#include <kdebug.h>



/*****************************************************************************
*	Constructors, Destructors, Initilizers, and 
*	Global Status Indicators.
*****************************************************************************/
dictQuery::dictQuery() : Q3Dict<QString>()
{
	init();
}

dictQuery::dictQuery(const QString& str) : Q3Dict<QString>() {
	init();
	this->operator=((QString)str);
}

dictQuery::dictQuery(const dictQuery& orig):Q3Dict<QString>(orig) {
	init();
	this->operator=((dictQuery&)orig);
}

void dictQuery::init() {
	setAutoDelete(TRUE);
	matchType=matchExact;
}

dictQuery::~dictQuery()
{
}

bool dictQuery::isEmpty() const {
//We're only empty if the two strings are empty too
	return Q3Dict<QString>::isEmpty() && meaning.isEmpty()
		&& pronounciation.isEmpty() && word.isEmpty();
}
void dictQuery::clear() {
	Q3Dict<QString>::clear();
	meaning="";
	pronounciation="";
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
	for(; it.current(); ++it)
		Q3Dict<QString>::insert( it.currentKey(),
				new QString(*(it.current())) );
	meaning=d.meaning;
	pronounciation=d.pronounciation;
	word = d.word;
	entryOrder = d.entryOrder;
	return *this;
}

dictQuery &dictQuery::operator+=(const dictQuery &d) {
	return operator=(this->toString()+mainDelimiter+d.toString());
}

bool operator==( dictQuery other, dictQuery query ) {
	if( (other.pronounciation != query.pronounciation)
		|| (other.meaning != query.meaning) 
		|| (other.word != query.word)
		|| (other.entryOrder != query.entryOrder)
		|| (other.count() != query.count()) ) 
		return false;

	dictQuery::Iterator it( other );
	for( ; it.current(); ++it )
		if( it.current() != query[it.currentKey()])
			return false;
	return true;
}

bool operator<( dictQuery A, dictQuery B) {
	dictQuery::Iterator it( A );
	for( ; it.current(); ++it ) {
		//The default case is for properties to need
		//to match exactly in B, if present in A
		//There are plenty of exceptions here though
		if(it.currentKey() == "R") {
			QStringList aList = it.current()->split("");
			
			for ( QStringList::Iterator rad = aList.begin();
						rad != aList.end(); ++rad )
				if(B.getProperty("R").contains(*rad)==0)
					return false;
		} else if( it.current() != B[it.currentKey()])
			return false;
	}

	if(!A.pronounciation.isEmpty()) {
		QStringList aList = A.pronounciation.split(dictQuery::mainDelimiter);
		QStringList bList = B.pronounciation.split(dictQuery::mainDelimiter);
		for ( QStringList::Iterator it = aList.begin();
				it != aList.end(); ++it )
			if(bList.contains(*it)==0)
				return false;
	}
		
	if(!A.meaning.isEmpty()) {
		QStringList aList = A.meaning.split(dictQuery::mainDelimiter);
		QStringList bList = B.meaning.split(dictQuery::mainDelimiter);
		for ( QStringList::Iterator it = aList.begin();
				it != aList.end(); ++it )
			if(bList.contains(*it)==0)
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
		return QString::null;

	QString reply;
	QStringList entryCopy = entryOrder; //const keyword be damned
	for (QStringList::Iterator it=entryCopy.begin();
					it!=entryCopy.end(); ++it) {
		if(*it == pronounciationMarker)
			reply += pronounciation+mainDelimiter;
		else if(*it == meaningMarker)
			reply += meaning+mainDelimiter;
		else if(*it == wordMarker)
			reply += word+mainDelimiter;
		else
			reply += *it + propertySeperator
				+ *this->find(*it) + mainDelimiter;
	}
	reply.truncate(reply.length()-mainDelimiter.length());

	return reply;
}

dictQuery &dictQuery::operator=(const QString &str) {
	QStringList parts = str.split(mainDelimiter);
	dictQuery result;
	for ( QStringList::Iterator it = parts.begin(); it != parts.end(); ++it ) {
		if((*it).contains(propertySeperator)) {
			QStringList prop = it->split(propertySeperator);
			if(prop.count() != 2)
#ifdef USING_QUERY_EXCEPTIONS
				throw invalidQueryException(*it);
#else
				break;
#endif
			result.setProperty(prop[0],prop[1]);
			//replace or throw an error with duplicates?
		} else switch(stringTypeCheck(*it)) {
			case dictQuery::strTypeLatin :
				if(result.entryOrder.removeAll(meaningMarker) > 0 )
					result.setMeaning(result.getMeaning() + mainDelimiter + *it);
				else
					result.setMeaning(*it);
				break;
			case dictQuery::strTypeKana :
				if(result.entryOrder.removeAll(pronounciationMarker)>0)
					result.setPronounciation(result.getPronounciation() 
					                                     + mainDelimiter + *it );
				else
					result.setPronounciation(*it);
				break;

			case dictQuery::strTypeKanji :
				result.entryOrder.removeAll(wordMarker);
				result.setWord( *it ); //Only one of these allowed
				break;

			case dictQuery::mixed :
				qWarning("mixed wtf?");
			case dictQuery::stringParseError :
				qWarning("wtf?");
#ifdef USING_QUERY_EXCEPTIONS
				throw invalidQueryException(*it);
#endif
				break;
		}
	}
//	kdDebug() << "Query: ("<<result.getWord() << ") ["<<result.getPronounciation()<<"] :"<<
//		result.getMeaning()<<endl;
	this->operator=(result);
	return *this;
}
//Private utility method for the above... confirms that an entire string
//is either completely japanese or completely english
dictQuery::stringTypeEnum dictQuery::stringTypeCheck(QString in) {
	stringTypeEnum firstType;
	//Split into individual characters
	QStringList charList = in.split(QString(""));
	if(charList.count() <= 0)
		return dictQuery::stringParseError;
	
	QStringList::Iterator c = charList.begin();
	for(firstType = charTypeCheck((*c)); c!=charList.end(); ++c ){
		stringTypeEnum newType = charTypeCheck(*c);
		if(newType != firstType) {
			if(firstType == strTypeKana && newType == strTypeKanji)
				firstType = strTypeKanji;
			else if(firstType == strTypeKanji && newType == strTypeKana)
				; //That's okay
			else
				return dictQuery::mixed;
		}
	}
	return firstType;
}
//Private utility method for the stringTypeCheck
//Just checks and returns the type of the first character in the string
//that is passed to it.
dictQuery::stringTypeEnum dictQuery::charTypeCheck(QString ichar) {
	QChar ch = ichar[0];
	if(ch.toLatin1()) 
		return strTypeLatin;
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
	copy.removeAll(pronounciationMarker);
	copy.removeAll(meaningMarker);
	return copy;
}

QString dictQuery::getProperty(const QString &key) const {
	QString *result = this->find(key);
	if(result == 0) return QString(QString::null);
	return QString(*result);
}

bool dictQuery::hasProperty(const QString &key) const {
	return entryOrder.contains(key)>0;
}

/**************************************************************
*	Mutators for the Properties (including QDict overrides)
**************************************************************/
//TODO: Add i18n handling and alternate versions of property names
bool dictQuery::setProperty(const QString& key,const QString& value) {
	return replace(key,&value);
}

bool dictQuery::removeProperty(const QString &key){
	return remove(key);
}

bool dictQuery::insert(const QString& key, const QString *item){
	if(key==pronounciationMarker || key==meaningMarker ||
		key.isEmpty() || item->isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(key+propertySeperator+*item);
#else
		return false;
#endif
	if ( ! Q3Dict<QString>::find( key ) )
		entryOrder.append(QString(key));
	Q3Dict<QString>::insert( key, new QString(*item) );
	return true;
}

bool dictQuery::replace(const QString& key, const QString *item){
	if(key==pronounciationMarker || key==meaningMarker ||
		key.isEmpty() || item->isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(key+propertySeperator+*item);
#else
		return false;
#endif
	if ( ! Q3Dict<QString>::find( key ) )
		entryOrder.append(QString(key));
	Q3Dict<QString>::replace( key, new QString(*item) );
	return true;
}

bool dictQuery::remove(const QString& key) {
	if(Q3Dict<QString>::remove(key))
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
	return pronounciation;
}

bool dictQuery::setPronounciation(const QString &newPro) {
	if(newPro.isEmpty())
#ifdef USING_QUERY_EXCEPTIONS
		throw invalidQueryException(newPro);
#else
		return false;
#endif
	pronounciation=newPro;
	if(!entryOrder.contains(pronounciationMarker))
		entryOrder.append(pronounciationMarker);
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
const QString dictQuery::pronounciationMarker("__@\\p");
const QString dictQuery::meaningMarker("__@\\m"); 
const QString dictQuery::wordMarker("_@\\w");
const QString dictQuery::mainDelimiter(" ");
const QString dictQuery::propertySeperator(":");
