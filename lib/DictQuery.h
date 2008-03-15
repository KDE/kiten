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
	* This class is one of the three critical classes (along with
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
	* Property Pairs of the form \<i\>name\</i\>:\<i\>value\</i\>
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
	* &lt;QS&gt; ::= &lt;M&gt;DictQuery::mainDelimiter&lt;QS&gt;|&lt;R&gt;DictQuery::mainDelimiter&lt;QS&gt;|
	*       &lt;O&gt;DictQuery::mainDelimiter&lt;QS&gt;|NULL
	* &lt;M&gt;  ::= kana&lt;M&gt;|kana
	* &lt;R&gt;  ::= character&lt;R&gt;|character
	* &lt;O&gt;  ::= &lt;C&gt;DictQuery::propertySeperator&lt;D&gt;
	* &lt;C&gt;  ::= character&lt;C&gt;|character
	* &lt;D&gt;  ::= character&lt;D&gt;|character
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
	  * @param str the QString will be parsed as described below in operator=(const QString&)
	  */
	DictQuery(const QString& str);
	/**
	  * Copy constructor
	  * @param orig the original DictQuery to be copied
	  */
	DictQuery(const DictQuery& orig);
	/**
	  * Destructor
	  */
	~DictQuery();

	/**
	  * @return true if the DictQuery is completely empty
	  */
	bool isEmpty() const;
	/**
	  * Removes all text/entries from the DictQuery
	  */
	void clear ();
	/**
	  * The assignment copy operator
	  */
	DictQuery &operator=(const DictQuery&);
	/**
	  * The clone method
	  */
	DictQuery *clone() const;
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
	operator QString() const;

	/**
	 * Use this to get a list of all the property keys in the query
	  */
	const QList<QString> listPropertyKeys() const;
	/**
	  * Returns a given extended attribute
	  */
	const QString operator[] (const QString &) const;
	/**
	  * Sets a given extended attribute
	  */
	QString operator[] (const QString &);
	/**
	  * Get a specific property by key (is the same as using operator[] const)
	  */
	QString getProperty(const QString&) const;
	/**
	  * Verify if a given DictQuery object has a search parameter of a
	  * particular property
	  */
	bool hasProperty(const QString&) const;
	/**
	  * Set a particular property... this does significantly more error checking
	  * than the operator[] version, and will return false if there was a
	  * problem (an empty value or bad key)
	  * @param key the key for this entry
	  * @param value the value to set this to, will overwrite the current contents of this location
	  * @returns false on failure
	  */
	bool setProperty(const QString& key, const QString& value);
	/**
	  * Remove all instances of a property
	  * @returns true if the DictQuery had properties of the given type
	  */
	bool removeProperty(const QString&);
	/**
	  * Returns and removes the property
	  */
	QString takeProperty(const QString&);

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
	KITEN_EXPORT friend DictQuery operator+( const DictQuery&, const DictQuery&);
#ifndef QT_NO_CAST_ASCII
	/**
	  * An ascii cast variant of the operator=
	  * Only available if QT_NO_CAST_ASCII is not defined on lib compilation
	  */
	DictQuery    &operator=(const char *);
#endif

	 //Specify the type of matching
	/**
	  * This enum is used to define the type of matching this query is supposed
	  * to do. The names are fairly self-explanatory
	  */
	enum MatchType {matchExact, matchBeginning, matchAnywhere};
	/**
	  * Get which match type is currently set on the DictQuery
	  */
	MatchType getMatchType() const;
	/**
	  * Set a match type. If this is not called, the default is matchExact.
	  */
	void setMatchType(MatchType);

	/** This enum is used as the return type for the two utility functions, stringTypeCheck and
	 * charTypeCheck */
	enum stringTypeEnum {strTypeKanji, strTypeKana, strTypeLatin, mixed, stringParseError};
	/** A simple utility routine to tell us what sort of string we have
	 * If the string contains only kanji, kana or non-kanji/kana characters, the result is strTypeKanji,
	 * strTypeKana or strTypeLatin (perhaps a misnomer... but so far it's valid).
	 * If the string contains both kanji and kana, the type returned is strTypeKanji
	 * If the string contains any other combination, the return type is mixed */
	static stringTypeEnum stringTypeCheck(const QString &);
	/** This utility does the same thing for QChar as stringTypeCheck does for QString. At the moment
	 * the implementation is rather simple, and it assumes that anything that is not latin1 or kana is
	 * a kanji */
	static stringTypeEnum charTypeCheck(const QChar &);

private:
	/** Stores the (english or otherwise non-japanese) meaning */
	QString m_meaning;
	/** Stores the pronunciation in kana */
	QString m_pronunciation;
	/** The main word, this usually contains kanji */
	QString m_word;
	/** Any amount of extended attributes, grade leve, heisig/henshall/etc index numbers, whatever you want */
	QHash<QString,QString> m_extendedAttributes;
	/** The order that various attributes, meanings, and pronunciations were entered, so we can
	 * regenerate the list for the user if they need them again */
	QStringList m_entryOrder;
	/** A list of dictionaries to limit the search to, and empty list implies "all loaded dictionaries" */
	QStringList m_targetDictionaries;
	/** What MatchType is this set to */
	MatchType m_matchType;

	/** Marker in the m_entryOrder for the location of the pronunciation element */
	static const QString pronunciationMarker;
	/** Marker in the m_entryOrder for the location of the translated meaning element */
	static const QString meaningMarker;
	/** Marker in the m_entryOrder for the location of the word (kanji) element */
	static const QString wordMarker;

	/** This function needs to be called by all constructors, just to setup default values */
	void init();
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
