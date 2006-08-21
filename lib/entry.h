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

#ifndef ENTRY_H
#define ENTRY_H


#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QHash>
#include "libkitenexport.h"

#include "dictQuery.h"

class Entry;
class EntryList;
class QString;
class dictFile;
class dictQuery;

class LIBKITEN_EXPORT Entry {
public:
	/** Default constructor, should not be used */
	Entry();
	/** Copy constructor */
	Entry(const Entry&);
	/** Constructor that includes the dictionary source */
	Entry(const QString &sourceDictionary);
	/** A constructor that takes a dictionary and a string to parse. THIS MUST
	  BE OVERRIDDEN IN EVERY SUBCLASS */
	Entry(const QString &sourceDictionary, const QString &parse);
	/** A constructor that includes the basic information, nicely seperated */
	Entry(const QString &sourceDictionary, const QString &word, 
			const QStringList &readings, const QStringList &meanings);
	/** A constructor pattern that includes support for the extended info QDict (copied) */
	Entry(const QString &sourceDictionary, const QString &word, const QStringList 
			&readings, const QStringList &meanings, const QHash<QString,QString> &extendedInfo);
	/** Generic Destructor, note that any strings in the QDict will be destroyed here */	
	virtual ~Entry();
	/** A clone method, this should just implement "return new EntrySubClass(*this)" */
	virtual Entry *clone() const = 0;

	/** Fairly important method, this tests if this particular entry matches a query */
	virtual bool matchesQuery(const dictQuery&) const;

	/** Any enum used by the print methods as an argument */
	enum printType {printBrief, printVerbose, printAuto};

	/* Some regular old accessors */
	QString getDictName() const { return sourceDict; }
	QString getWord() const { return Word; }
	QString getMeanings() const {return Meanings.join(outputListDelimiter);}
	QStringList getMeaningsList() const { return Meanings; }
	QString getReadings() const {return Readings.join(outputListDelimiter);}
	QStringList getReadingsList() const { return Readings; }
	QHash<QString,QString> getExtendedInfo() const { return ExtendedInfo; }
	QString getExtendedInfoItem(const QString x) const { return ExtendedInfo[x]; }

	/* An entry should be able to generate a representation of itself in (valid)
		HTML */
	virtual QString toHTML(printType=printAuto) const;
	/** This will return a pure text interpretation of the Entry */
	virtual QString toString(printType=printAuto) const;
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
	virtual bool sortByField(const Entry &that, const QString field) const;

protected:
	QString sourceDict;
	QString Header;
	QString Word;

	QStringList Meanings;
	QStringList Readings;

	QHash<QString,QString> ExtendedInfo;

	printType favoredPrintType;
	QString outputListDelimiter;
	
	void init();

	/* An entry should be able to generate a representation of itself in 
		plain ol' text (for buttons and such) */
	virtual QString toBriefText() const;
	virtual QString toVerboseText() const;
	/* Various forms of HTML Output */	
	virtual QString toBriefHTML() const;
	virtual QString toVerboseHTML() const;
	
	/* New functions for Entry doing direct display */
	virtual QString makeLink(const QChar) const;
	virtual QString makeLink(const QString) const;
	virtual QString HTMLWord() const;
	virtual QString HTMLReadings() const;
	virtual QString HTMLMeanings() const;

	/* Handy Utility functions for matching to lists and identifying char types */
	bool listMatch(const QString&,const QStringList&) const;
	bool isKanji(const QChar) const;
};


class LIBKITEN_EXPORT EntryList : public QList<Entry*> {
public:
	typedef QListIterator<Entry*> EntryIterator;
	
	EntryList() {}
	EntryList(const EntryList &old) : QList<Entry*>(old) {}
	virtual ~EntryList();
	void deleteAll();

	QString toString(Entry::printType=Entry::printAuto) const;
	QString toHTML(Entry::printType=Entry::printAuto) const;
	
	QString toString(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	QString toHTML(unsigned int start, unsigned int length,
			Entry::printType=Entry::printAuto) const;
	
	void sort(QStringList &sortOrder,QStringList &dictionaryOrder);

	const EntryList& operator+=(const EntryList &other);
	void appendList(const EntryList *);
	dictQuery getQuery() const;
	void setQuery(const dictQuery&);

protected:
	dictQuery query;

private: //Utility Methods
	inline QString noResultsHTML();
};

#endif
