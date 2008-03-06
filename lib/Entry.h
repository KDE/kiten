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

#ifndef KITEN_ENTRY_H
#define KITEN_ENTRY_H

#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QHash>
#include "libkitenexport.h"

#include "DictQuery.h"

class Entry;
class EntryList;
class QString;

/** The Entry class is a generic base class for each particular entry in a given dictionary. It's used as the
  basic class to ferry information back to the user application. */
class KITEN_EXPORT Entry {

	friend class EntryListModel;

private:
	/** Default constructor, should not be used. Made private to serve as a warning
	 that you're doing something wrong if you try to call this */
	Entry();
protected:
	/** A constructor that takes a dictionary and a string to parse. THIS MUST
	  BE OVERRIDDEN IN EVERY SUBCLASS */
	Entry(const QString &sourceDictionary, const QString &parse);

	/** Copy constructor. */
	Entry(const Entry&);
	/** Constructor that includes the dictionary source */
	Entry(const QString &sourceDictionary);
	/** A constructor that includes the basic information, nicely separated */
	Entry(const QString &sourceDictionary, const QString &word,
			const QStringList &readings, const QStringList &meanings);
	/** A constructor pattern that includes support for the extended info (copied) */
	Entry(const QString &sourceDictionary, const QString &word, const QStringList
			&readings, const QStringList &meanings, const QHash<QString,QString> &extendedInfo);

public:
	/** Generic Destructor */
	virtual ~Entry();
	/** A clone method, this should just implement "return new EntrySubClass(*this)" */
	virtual Entry *clone() const = 0;

	/** Fairly important method, this tests if this particular entry matches a query */
	virtual bool matchesQuery(const DictQuery&) const;

	/** An enum used by the print methods as an argument */
	typedef enum printType {printBrief, printVerbose, printAuto};

	/** Get the dictionary name that generated this Entry */
	QString getDictName() const { return sourceDict; }
	/** Get the word from this Entry. If the entry is of type kanji/kana/meaning/etc, this will return
	 * the kanji. If it is of kana/meaning/etc, it will return kana  */
	QString getWord() const { return Word; }
	/** Get a QString containing all of the meanings known, connected by the outputListDelimiter  */
	QString getMeanings() const {return Meanings.join(outputListDelimiter);}
	QStringList getMeaningsList() const { return Meanings; }
	QString getReadings() const {return Readings.join(outputListDelimiter);}
	QStringList getReadingsList() const { return Readings; }
	const QHash<QString,QString> &getExtendedInfo() const { return ExtendedInfo; }
	QString getExtendedInfoItem(const QString &x) const { return ExtendedInfo[x]; }
	virtual bool extendedItemCheck(const QString &key, const QString &value) const;

	/** An entry should be able to generate a representation of itself in (valid)
		HTML */
	virtual QString toHTML(printType=printAuto) const;
	/** KVTML format for exporting */
	virtual QString toKVTML() const;
	/** This will return a pure text interpretation of the Entry */
	virtual QString toString(printType=printAuto) const;

	/** the favoredPrintType parameter may be used by the HTML and toString
	  printing methods */
	virtual void setFavoredPrintType(const printType);
	virtual printType getFavoredPrintType() const;

	/* An entry should be able to parse an in-file representation of an entry
		as a QString and put it back.  The latter will be useful for writing
		to dictionaries on disk at some point. */
	virtual bool loadEntry(const QString &entryLine) = 0;
	virtual QString dumpEntry() const = 0;

	/* This is needed for sorting */
	virtual bool sort(const Entry &, const QStringList &dictionaryList,
			const QStringList &fieldList) const;
	virtual bool sortByField(const Entry &that, const QString &field) const;

protected:
	// The actual data of this entry
	QString Word;
	QStringList Meanings;
	QStringList Readings;
	QHash<QString,QString> ExtendedInfo;

	//A bit of metadata
	QString sourceDict;
	printType favoredPrintType;
	QString outputListDelimiter;

	void init();

	/* New functions for Entry doing direct display */
	virtual QString makeLink(const QChar&) const;
	virtual QString makeLink(const QString&) const;
	virtual QString HTMLWord() const;
	virtual QString HTMLReadings() const;
	virtual QString HTMLMeanings() const;

	/* Handy Utility functions for matching to lists and identifying char types */
	bool listMatch(const QString&,const QStringList&) const;
	bool isKanji(const QChar&) const;
};


#endif
