/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/


#ifndef DICT_H
#define DICT_H

#include <qfile.h>
#include <qmemarray.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstringlist.h>

#include <sys/types.h>
#ifdef __osf__
typedef unsigned int uint32_t;
typedef int int32_t;
#else
#include <inttypes.h>
#endif

namespace Dict
{

enum TextType { Text_Kanji, Text_Kana, Text_Latin };

// returns the TextType of the first part of the text
KDE_EXPORT TextType textType(const QString &text);

// File needs to be able to give out Arrays based on its mmap'd data.
// But, we don't want the users of the arrays to have to remember to
// resetRawData() after using them, since that's bound to fail sooner or later.
//
// This class handles it for us.
template<class T> class Array : public QMemArray<T>
{
public:
	Array(T *, int);
	virtual ~Array();

private:
	T *data;
	int dataSize;
};

template<class T> Array<T>::Array(T *d, int s)
	: QMemArray<T>()
	, data(d)
	, dataSize(s)
{
	setRawData(data, dataSize / sizeof(T));
}

template<class T> Array<T>::~Array()
{
	resetRawData(data, dataSize / sizeof(T));
}

// File manages all the files, pointers, and memory management associated
// with a single dictionary.
class File
{
public:
	File(QString path, QString name);
	~File();

	QString name(void);

	Array<const unsigned char> dict(void);
	Array<const uint32_t> index(void);

	int dictLength(void);
	int indexLength(void);

	// replacement for exceptions thrown in the constructor
	bool isValid(void);

	unsigned char lookup(unsigned i, int offset);
	QCString lookup(unsigned i);
private:
	QString myName;

	QFile dictFile;
	const unsigned char * dictPtr;

	QFile indexFile;
	const uint32_t * indexPtr;

	bool valid;
};

class KDE_EXPORT Entry
{
public:
	// EDict ctor
	Entry(const QString &, const QString &, const QStringList &);
	// Kanjidict ctor
	Entry(const QString &, QStringList &, QStringList &, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount);
	// default (for containers)
	Entry(const QString & = QString::null);
	// for a heading
	Entry(const QString &, bool header);

	QString dictName();
	QString header();
	QStringList meanings();
	QStringList readings();
	QString firstReading();

	bool kanaOnly();
	QString kanji();

	bool extendedKanjiInfo();
	unsigned int grade();
	unsigned int strokes();
	unsigned int miscount();
	unsigned int freq();

protected:
	QString DictName;
	QString Header;
	QStringList Meanings;

	QString Kanji;
	bool KanaOnly;
	QStringList Readings;

	bool ExtendedKanjiInfo;
	unsigned int Grade;
	unsigned int Strokes;
	unsigned int Miscount;
	unsigned int Freq;
};

struct SearchResult
{
	QValueList<Entry> list;
	QStringList results;
	int count, outOf;
	bool common;
	QString text;
};

enum SearchType { Search_Beginning, Search_FullWord, Search_Anywhere };
enum DictionaryType { Edict, Kanjidict };

class KDE_EXPORT Index : public QObject
{
Q_OBJECT

public:
	Index();
	virtual ~Index();

	void setDictList(const QStringList &files, const QStringList &names);
	void setKanjiDictList(const QStringList &files, const QStringList &names);

	SearchResult search(QRegExp, const QString &, bool common);
	SearchResult searchKanji(QRegExp, const QString &, bool common);
	SearchResult searchPrevious(QRegExp, const QString &, SearchResult, bool common);

	// convenience function to create suitable regexps
	static QRegExp createRegExp(SearchType type, const QString &text, DictionaryType dictionaryType, bool caseSensitive = false);

private:
	QPtrList<File> dictFiles;
	QPtrList<File> kanjiDictFiles;

	void loadDictList(QPtrList<File> &fileList, const QStringList &dictList, const QStringList &dictNameList);

	QStringList doSearch(File &, const QString &);
	SearchResult scanResults(QRegExp regexp, QStringList results, bool common);
	SearchResult scanKanjiResults(QRegExp regexp, QStringList results, bool common);
	int stringCompare(File &, int index, QCString);
};

// lotsa helper functions
KDE_EXPORT QString prettyKanjiReading(QStringList);
KDE_EXPORT QString prettyMeaning(QStringList);
KDE_EXPORT Entry parse(const QString &);
KDE_EXPORT Entry kanjiParse(const QString &);
KDE_EXPORT Dict::Entry firstEntry(Dict::SearchResult);
KDE_EXPORT QString firstEntryText(Dict::SearchResult);

int eucStringCompare(const char *str1, const char *str2);
bool isEUC(unsigned char c);
}

#endif
