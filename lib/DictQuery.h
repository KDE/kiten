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
#ifndef KITEN_DICTQUERY_H
#define KITEN_DICTQUERY_H

#include <QtCore/QHash>
#include <QtCore/QHashIterator>
#include <QtCore/QStringList>
#include <QtCore/QString>
class QChar;

#include "libkitenexport.h"

 /**
	* @short A class to allow users of libkiten to properly setup a database
	* query.
	*
	* In general, you either pass or parse in parameters from users or automated
	* programs to be later sent to the dictionary manager.
	*
	* @long This class is one of the three critical classes (along with
	* dictionary and EntryList) that are needed to use libkiten. Essentially...
	* you feed the dictionary class a DictQuery, and dictionary will return an
	* EntryList that matches the query.
	*
	* @code
	* dictionary dictManager();
	* //Load some dictionaries via dictionary class methods
	* EntryList *results;
	* DictQuery myQuery("kanji");
	* results = dictManager.doSearch(myQuery);
	* //Print results (if any)
	* @endcode
	*
	* The internal implementation understands four distinct types of data:
	* Japanese Kanji
	* Japanese Kana
	* English Characters
	* Property Pairs of the form <i>name</i>:<i>value</i>
	*
	* It is left up to the individual dictionary types to parse these values
	* for matching and appropriateness to each dictionary.
	* You can use the setDictionaries() method to narrow the range of the
	* dictionaries that it will apply to.
	*
	* A DictQuery object can be considered an "unordered set" of values.
	* When testing for equality or comparison, each property or text entry above
	* is seen as a unique item in a set. Order is not important for these
	* operations. The object will attempt to preserve the order from parsed
	* strings, but it does not consider this order to be important.
	*
	* In general, application level programs are expected to use the
	* QString based interfaces, and dictionary implementations and other
	* parts of libkiten are expected to use the direct accessors and mutators,
	* although specialized interfaces (such as kitenradselect) may use property
	* mutators for a limited set of properties. (in this case, radicals)
	*
	* The query string input is of the following format:
	* <QS> ::= <M>DictQuery::mainDelimiter<QS>|<R>DictQuery::mainDelimiter<QS>|
	*       <O>DictQuery::mainDelimiter<QS>|NULL
	* <M>  ::= kana<M>|kana
	* <R>  ::= character<R>|character
	* <O>  ::= <C>DictQuery::propertySeperator<D>
	* <C>  ::= character<C>|character
	* <D>  ::= character<D>|character
	*
	* @author Joseph Kerian \<jkerian@gmail.com>
	*/

class KITEN_EXPORT DictQuery
{
public:
	/**
	  * This is the main delimiter that the DictQuery uses when parsing strings.
	  * It is set to "space" at the moment.
	  */
	static const QString mainDelimiter;
	/**
	  * This is the delimiter that DictQuery uses when parsing property strings
	  * of the form <i>strokes:4</i>. It is set to ":" at the moment.
	  */
	static const QString propertySeperator;
	/**
	  * Normal constructor.
	  * This will create an empty query object.
	  */
	DictQuery();
	/**
	  * Constructor with a given QString.
	  * @param str This QString will be parsed as described below in
	  * operator=(const QString&)
	  */
	DictQuery(const QString& str);
	/**
	  * Copy constructor
	  */
	DictQuery(const DictQuery& orig);
	/**
	  * Destructor
	  */
	virtual ~DictQuery();

	/**
	  * @return true if the DictQuery is completely empty
	  */
	bool isEmpty() const;
	/**
	  * Removes all text/entries from the DictQuery
	  */
	virtual void clear ();
	/**
	  * The assignment copy operator
	  */
	DictQuery &operator=(const DictQuery&);
	/**
	  * The clone method
	  */
	virtual DictQuery *clone() const { return new DictQuery(*this); }
	/**
	  * This returns a QString that represents the query. This may be the same
	  * as the original string, but some slight changes may have occurred if you
	  * have done any manipulations on the DictQuery.
	  */
	const QString toString() const;
	/**
	  * This is a converstion to a QString... useful in a surpring
	  * number of cases
	  */
	operator QString() const { return toString(); }

	/**
	 * Use this to get a list of all the property keys in the query
	  */
	const QList<QString> listPropertyKeys() const;
	/**
	  * Returns a given extended attribute
	  */
	QString operator[] (const QString &key) const;
	/**
	  * Sets a given extended attribute
	  */
	QString operator[] (const QString &key);
	/**
	  * Get a specific property by key (is the same as using operator[] const)
	  */
	QString getProperty(const QString&) const;
	/**
	  * Verify if a given DictQuery object has a search parameter of a
	  * particular property
	  */
	bool hasProperty(const QString& key) const;
	/**
	  * Set a particular property... this does significantly more error checking
	  * than the operator[] version, and will return false if there was a
	  * problem (an empty value or bad key)
	  * @returns false on failure
	  */
	bool setProperty(const QString& key, const QString& value);
	/**
	  * Remove all instances of a property
	  * @returns true if the DictQuery had properties of the given type
	  */
	bool removeProperty(const QString& key);
	/**
	  * Returns and removes the property
	  */
	QString takeProperty(const QString& key);

	/**
	  * Returns a list of the dictionaries that this particular query
	  * will target. An empty list (the default) will search all dictionaries
	  * that the user has selected
	  */
	QStringList getDictionaries() const;
	/**
	  * Set the list of dictionaries to search. This will be read and used
	  * by the dictionary manager.
	  */
	void setDictionaries(const QStringList&);

	/**
	  * Accessor for the non-japanese meaning field
	  */
	QString getMeaning() const;
	/**
	  * Mutator for the Meaning field
	  */
	bool setMeaning(const QString &);
	/**
	  * Accessor for the Pronunciation field (generally kana)
	  */
	QString getPronunciation() const;
	/**
	  * Mutator for the Pronunciation field
	  */
	bool setPronunciation(const QString&);
	/**
	  * Accessor for the Word/Kanji field (this is usually used for anything
	  * containing kanji).
	  */
	QString getWord() const;
	/**
	  * Mutator for the Word/Kanji field. If what you are setting contains
	  * only kana, consider using the setPronunciation instead.
	  */
	bool setWord(const QString&);

	/**
	  * A simple setwise comparison of two DictQuery objects
	  * Note that order is not important here... only each element
	  * that is one of the DictQuery objects appears in the other
	  */
	KITEN_EXPORT friend bool operator==(const DictQuery&, const DictQuery&);
	/**
	  * Convenient inverted override of operator==(DictQuery,DictQuery)
	  */
	KITEN_EXPORT friend bool operator!=(const DictQuery&, const DictQuery&);
	/**
	  * Set-wise strictly less than. A better way to think of this
	  * might be the "subset" operator
	  */
	KITEN_EXPORT friend bool operator<(const DictQuery&, const DictQuery&);
	/**
	  * Convenient override of operator<(DictQuery,DictQuery) and operator==
	  */
	KITEN_EXPORT friend bool operator<=(const DictQuery&, const DictQuery&);
	/**
	  * This will append the properties and other elements of the added kanji
	  * onto the elements of the current element. If regenerated as a string,
	  * it should look something like concatenation
	  */
	DictQuery &operator+=(const DictQuery&);
	/**
	  * A simple string parser, look above for examples and explanations
	  */
	DictQuery &operator=(const QString&);
	/**
	  * A simple override of operator+=(const DictQuery&)
	  */
	DictQuery    &operator+=(const QString&);
	/**
	  * Simple addition... similer to operator+=
	  */
	KITEN_EXPORT friend DictQuery &operator+( const DictQuery&, const DictQuery&);
#ifndef QT_NO_CAST_ASCII
	/**
	  * An ascii cast variant of the operator=
	  * Only available if QT_NO_CAST_ASCII is not defined on lib compilation
	  */
	DictQuery    &operator=(const char *);
#endif

	 //Specify the type of matching
	/**
	  * @enum This enum is used to define the type of matching this query is supposed
	  * to do. The names are fairly self-explanatory
	  */
	enum matchTypeSettings {matchExact, matchBeginning, matchEnd, matchAnywhere};
	/**
	  * Get which match type is currently set on the DictQuery
	  */
	matchTypeSettings getMatchType() const;
	/**
	  * Set a match type. If this is not called, the default is matchExact.
	  */
	void setMatchType(matchTypeSettings);

protected:	//The QDict itself tracks properties as key->value pairs
	QString m_meaning;		//Stores the (presumably english) meaning
	QString m_pronunciation;	//Stores the (presumed non-english) pronunciation
	QString m_word;             //The 'key' word (this can potentially contain kanji)
	QHash<QString,QString> m_extendedAttributes;
	QStringList m_entryOrder;	//Keeps track of the order that things were entered
	QStringList m_targetDictionaries; //Tracks what dictionaries this entry will go into
	matchTypeSettings m_matchType;

	static const QString pronunciationMarker; //Internal markers in entryOrder
	static const QString meaningMarker;        //For where pronunciation and Meaningfound
	static const QString wordMarker;           // and word

	void init();

	enum stringTypeEnum {strTypeKanji, strTypeKana, strTypeLatin, mixed, stringParseError};
	static stringTypeEnum stringTypeCheck(const QString &in); //returns a string's consistant class.
	static stringTypeEnum charTypeCheck(const QChar &ch);   //Same for the first char of a string
};

//Currently... KDE doesn't seem to want to use exceptions
#ifdef LIBKITEN_USING_EXCEPTIONS
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
