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
#ifndef DICTQUERY_H
#define DICTQUERY_H

#include <QtCore/QHash>
#include <QtCore/QHashIterator>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QChar>

#include <qregexp.h>
#include "libkitenexport.h"

/**
@author Joseph Kerian
*/

/* This file implements and wraps the a query string. 
	The new query string input is of the following format:
	<QS> ::= <M>dictQuery::mainDelimiter<QS>|<R>dictQuery::mainDelimiter<QS>|
	      <O>dictQuery::mainDelimiter<QS>|NULL
	<M>  ::= kana<M>|kana
	<R>  ::= character<R>|character
	<O>  ::= <C>dictQuery::propertySeperator<D>
	<C>  ::= character<C>|character
	<D>  ::= character<D>|character
*/

typedef QHash<QString,QString> __stupid_stub_dictQuery_type;

class LIBKITEN_EXPORT dictQuery : public __stupid_stub_dictQuery_type
{
public:
	typedef QHashIterator<QString,QString> Iterator;  //Can be used to iterate over properties
	static const QString mainDelimiter;	   //Seperates main entries (usually space)
	static const QString propertySeperator;//Seperates keys from property values(":")

	dictQuery();
	dictQuery(const QString&);
	dictQuery(const dictQuery&);
	virtual ~dictQuery();

//Overloaded from QDict
	bool isEmpty() const;
	virtual void clear ();

//Overloaded from QDict... for the love of god don't use these
	bool insert(const QString& key, const QString& item);
	bool replace(const QString& key, const QString& item);
	bool remove(const QString& key);
	QString* take ( const QString & key );
/* The following QDict methods DO NOT INCLUDE pronounciations/meanings!!!! 
	virtual uint count() const;
	uint size () const
	type * find ( const QString & key ) const
	type * operator[] ( const QString & key ) const
	void resize ( uint newsize )
	void statistics () const */

//QString based accessors/mutators	//Example setting "<kanji> me S:4 H:123 L:14"
	dictQuery &operator=(const dictQuery&);
	const QString toString() const;	//Returns "<kanji> me S:4 H:123 L:14"
	inline operator QString() const { return toString(); }

	QString getProperties() const;	//Returns "S:4 H:123 L:14"
	QStringList getPropertiesList() const;//Returns "S:4","H:123","L:14"
	QString getPropertyKeys() const;	//Returns "S H L" from example
	QStringList getPropertyKeysList() const;	//Returns "S","H","L"
	 
	 QStringList getDictionaries() const;     //Set the target dictionaries
	 void setDictionaries(const QStringList); //Get the target dictionaries

	QString getProperty(const QString&) const;	//Returns "4" for input "S"
	bool hasProperty(const QString& key) const;
	bool setProperty(const QString& key, const QString& value); //return true if success
	bool removeProperty(const QString&); //if found: removes+true, else return false
		
	QString getMeaning() const;
	bool setMeaning(const QString &);
	QString getPronounciation() const;
	bool setPronounciation(const QString&);
	QString getWord() const;
	bool setWord(const QString&);

	//Overridden operators evaluate according to a 'set' mentality
	//dictQueries are equal if they contain the same elements
	//order is not relevent to these comparisons
	friend bool operator==( dictQuery, dictQuery);
	friend inline bool operator==( QString, dictQuery);
	friend inline bool operator==( dictQuery, QString);
	friend inline bool operator!=( dictQuery, dictQuery);
	friend inline bool operator!=( QString, dictQuery);
	friend inline bool operator!=( dictQuery, QString );
	//These run much quicker if in A<B, A contains fewer elements
	friend bool operator<( dictQuery, dictQuery );
	friend inline bool operator>( dictQuery, dictQuery );
	friend inline bool operator<=( dictQuery, dictQuery );
	friend inline bool operator>=( dictQuery, dictQuery );

	//These "lump" whatever is being added on the right side to the left side
	//So they will tend to be ordered as "A1 B2 C3" + "D E C4" = "A1 B2 C4 D E"
	dictQuery &operator+=(const dictQuery&);
	dictQuery &operator=(const QString&);
	dictQuery    &operator+=(const QString&);
	friend dictQuery &operator+( const dictQuery&, const dictQuery&);
	friend dictQuery &operator+( const dictQuery&, const QString&);
	friend dictQuery &operator+( const QString&,   const dictQuery&);
#ifndef QT_NO_CAST_ASCII
	dictQuery    &operator=(const char *);
	dictQuery    &operator+=(const char *);
#endif
	 
	 //Specify the type of matching
	 enum matchTypeSettings {matchExact, matchBeginning, matchEnd, matchAnywhere};
	 matchTypeSettings getMatchType() const;
	 void setMatchType(const matchTypeSettings);

protected:	//The QDict itself tracks properties as key->value pairs
	QString meaning;		//Stores the (presumably english) meaning
	QString pronounciation;	//Stores the (presumed non-english) prounounciation
	 QString word;             //The 'key' word (this can potentially contain kanji)
	QStringList entryOrder;	//Keeps track of the order that things were entered
	 QStringList targetDictionaries; //Tracks what dictionaries this entry will go into
	 matchTypeSettings matchType;

	static const QString pronounciationMarker; //Internal markers in entryOrder
	static const QString meaningMarker;        //For where pronounciation and Meaningfound
	 static const QString wordMarker;           // and word

	void init();

	enum stringTypeEnum {strTypeKanji, strTypeKana, strTypeLatin, mixed, stringParseError};
	stringTypeEnum stringTypeCheck(QString); //returns a string's consistant class.
	stringTypeEnum charTypeCheck(QChar);   //Same for the first char of a string
};

//Currently... KDE doesn't seem to want to use exceptions
//#define USING_QUERY_EXCEPTIONS
#ifdef USING_QUERY_EXCEPTIONS
class invalidQueryException {
	public:
		invalidQueryException(QString x) {val=x;}
		invalidQueryException(QString m="Invalid Query String",QString x)
	{val=x;msg=m;}
		QString value() {return val;}
		QString message() {return msg;}
	protected:
		QString val;
		QString msg;
};
#endif

#endif
