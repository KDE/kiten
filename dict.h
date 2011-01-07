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

#include <tqfile.h>
#include <tqmemarray.h>
#include <tqptrlist.h>
#include <tqregexp.h>
#include <tqstringlist.h>

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
KDE_EXPORT TextType textType(const TQString &text);

// File needs to be able to give out Arrays based on its mmap'd data.
// But, we don't want the users of the arrays to have to remember to
// resetRawData() after using them, since that's bound to fail sooner or later.
//
// This class handles it for us.
template<class T> class Array : public TQMemArray<T>
{
public:
	Array(T *, int);
	virtual ~Array();

private:
	T *data;
	int dataSize;
};

template<class T> Array<T>::Array(T *d, int s)
	: TQMemArray<T>()
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
	File(TQString path, TQString name);
	~File();

	TQString name(void);

	Array<const unsigned char> dict(void);
	Array<const uint32_t> index(void);

	int dictLength(void);
	int indexLength(void);

	// replacement for exceptions thrown in the constructor
	bool isValid(void);

	unsigned char lookup(unsigned i, int offset);
	TQCString lookup(unsigned i);
private:
	TQString myName;

	TQFile dictFile;
	const unsigned char * dictPtr;

	TQFile indexFile;
	const uint32_t * indexPtr;

	bool valid;
};

class KDE_EXPORT Entry
{
public:
	// EDict ctor
	Entry(const TQString &, const TQString &, const TQStringList &);
	// Kanjidict ctor
	Entry(const TQString &, TQStringList &, TQStringList &, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount);
	// default (for containers)
	Entry(const TQString & = TQString::null);
	// for a heading
	Entry(const TQString &, bool header);

	TQString dictName();
	TQString header();
	TQStringList meanings();
	TQStringList readings();
	TQString firstReading();

	bool kanaOnly();
	TQString kanji();

	bool extendedKanjiInfo();
	unsigned int grade();
	unsigned int strokes();
	unsigned int miscount();
	unsigned int freq();

protected:
	TQString DictName;
	TQString Header;
	TQStringList Meanings;

	TQString Kanji;
	bool KanaOnly;
	TQStringList Readings;

	bool ExtendedKanjiInfo;
	unsigned int Grade;
	unsigned int Strokes;
	unsigned int Miscount;
	unsigned int Freq;
};

struct SearchResult
{
	TQValueList<Entry> list;
	TQStringList results;
	int count, outOf;
	bool common;
	TQString text;
};

enum SearchType { Search_Beginning, Search_FullWord, Search_Anywhere };
enum DictionaryType { Edict, Kanjidict };

class KDE_EXPORT Index : public QObject
{
Q_OBJECT

public:
	Index();
	virtual ~Index();

	void setDictList(const TQStringList &files, const TQStringList &names);
	void setKanjiDictList(const TQStringList &files, const TQStringList &names);

	SearchResult search(TQRegExp, const TQString &, bool common);
	SearchResult searchKanji(TQRegExp, const TQString &, bool common);
	SearchResult searchPrevious(TQRegExp, const TQString &, SearchResult, bool common);

	// convenience function to create suitable regexps
	static TQRegExp createRegExp(SearchType type, const TQString &text, DictionaryType dictionaryType, bool caseSensitive = false);

private:
	TQPtrList<File> dictFiles;
	TQPtrList<File> kanjiDictFiles;

	void loadDictList(TQPtrList<File> &fileList, const TQStringList &dictList, const TQStringList &dictNameList);

	TQStringList doSearch(File &, const TQString &);
	SearchResult scanResults(TQRegExp regexp, TQStringList results, bool common);
	SearchResult scanKanjiResults(TQRegExp regexp, TQStringList results, bool common);
	int stringCompare(File &, int index, TQCString);
};

// lotsa helper functions
KDE_EXPORT TQString prettyKanjiReading(TQStringList);
KDE_EXPORT TQString prettyMeaning(TQStringList);
KDE_EXPORT Entry parse(const TQString &);
KDE_EXPORT Entry kanjiParse(const TQString &);
KDE_EXPORT Dict::Entry firstEntry(Dict::SearchResult);
KDE_EXPORT TQString firstEntryText(Dict::SearchResult);

int eucStringCompare(const char *str1, const char *str2);
bool isEUC(unsigned char c);
}

#endif
