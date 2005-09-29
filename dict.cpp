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

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>

#include <qfileinfo.h> 
#include <qregexp.h>
#include <qtextcodec.h>

#include "dict.h"

#include <iostream>
#include <cassert>
#include <sys/mman.h> 
#include <stdio.h>

namespace
{
void msgerr(const QString &msg, const QString &dict = QString::null)
{
	QString output = msg;
	if (!dict.isNull())
		output = msg.arg(dict);
	KMessageBox::error(0, output);
}
}

using namespace Dict;

TextType Dict::textType(const QString &text)
{
	ushort first = text.at(0).unicode();
	
	if (first < 0x3000)
		return Text_Latin;
	// else if (first < 0x3040) // CJK Symbols and Punctuation
			// return Text_Kana;
		// else if (first < 0x30A0) // Hiragana
			// return Text_Kana;
	else if (first < 0x3100) // Katakana
		return Text_Kana;
	
	else /*if (first >= 0x3400 && first < 0x4DC0)*/ // CJK Unified Ideographs Extension A
		return Text_Kanji;
}

File::File(QString path, QString n)
	: myName(n)
	, dictFile(path)
	, dictPtr((const unsigned char *)MAP_FAILED)
	, indexFile(KGlobal::dirs()->saveLocation("data", "kiten/xjdx/", true) + QFileInfo(path).baseName() + ".xjdx")
	, indexPtr((const uint32_t *)MAP_FAILED)
	, valid(false)
{
	bool forceUpdate = false;

	bool indexFileExists = indexFile.exists();
	if (indexFileExists)
	{
		// ### change this if need be!!
		const int indexFileVersion = 14;

		// this up-to-date code from xjdservcomm.c
		// we need to check if the index needs to
		// remade

		int dictionaryLength;
		QFile dictionary(path);
		dictionaryLength = dictionary.size();
		dictionaryLength++;
		//kdDebug() << "dictionaryLength = " << dictionaryLength << endl;

		int32_t testWord[1];
		fread(&testWord[0], sizeof(int32_t), 1, fopen(indexFile.name().latin1(), "rb"));

		//kdDebug() << "testWord[0] = " << testWord[0] << endl;

		if (testWord[0] != (dictionaryLength + indexFileVersion))
			forceUpdate = true;
	}

	if (!indexFileExists || forceUpdate)
	{
		//kdDebug() << "creating " << indexFile.name() << endl;
		// find the index generator executable
		KProcess proc;
		proc << KStandardDirs::findExe("kitengen") << path << indexFile.name();
		// TODO: put up a status dialog and event loop instead of blocking
		proc.start(KProcess::Block, KProcess::NoCommunication);
	}

	if (!dictFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open dictionary %1."), path);
		return;
	}

	dictPtr = (const unsigned char *)mmap(0, dictFile.size(), PROT_READ, MAP_SHARED, dictFile.handle(), 0);
	if (dictPtr == (unsigned char*) MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1."), path);
		return;
	}

	if (!indexFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open index for dictionary %1."), path);
		return;
	}

	indexPtr = (const uint32_t*)mmap(0, indexFile.size(), PROT_READ, MAP_SHARED, indexFile.handle(), 0);
	if (indexPtr == (uint32_t*) MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1's index file."), path);
		return;
	}

	valid = true;
}

File::~File(void)
{
	if (dictPtr != (unsigned char*) MAP_FAILED)
		munmap((char *)dictPtr, dictFile.size());
	dictFile.close();

	if (indexPtr != (uint32_t*) MAP_FAILED)
		munmap((char *)indexPtr, indexFile.size());
	indexFile.close();
}

QString File::name(void)
{
	return myName;
}

Array<const unsigned char> File::dict(void)
{
	assert(valid);
	return Array<const unsigned char>(dictPtr, dictFile.size());
}

Array<const uint32_t> File::index(void)
{
	assert(valid);
	return Array<const uint32_t>(indexPtr, indexFile.size());
}

int File::dictLength(void)
{
	return dictFile.size();
}

int File::indexLength(void)
{
	return indexFile.size();
}

bool File::isValid(void)
{
	return valid;
}

// returns specified character from a dictionary
unsigned char File::lookup(unsigned i, int offset)
{
	uint32_t pos = indexPtr[i] + offset - 1;
	if (pos > dictFile.size()) return 10;
	return dictPtr[pos];
}

QCString File::lookup(unsigned i)
{
	uint32_t start = indexPtr[i] - 1;
	uint32_t pos = start;
	const unsigned size = dictFile.size();
	// get the whole word
	while(pos <= size && dictPtr[pos] != 0 && dictPtr[pos] != 0x0a)
		++pos;
	// put the word in the QCString
	QCString retval((const char *)(dictPtr + start), pos - start);
	// tack on a null
	char null = 0;
	retval.append(&null);
	// and away we go
	return retval;
}

// And last, Index itself is the API presented to the rest of Kiten
Index::Index()
	: QObject()
{
	dictFiles.setAutoDelete(true);
	kanjiDictFiles.setAutoDelete(true);
}

Index::~Index()
{
}

void Index::setDictList(const QStringList &list, const QStringList &names)
{
	loadDictList(dictFiles, list, names);
}

void Index::setKanjiDictList(const QStringList &list, const QStringList &names)
{
	loadDictList(kanjiDictFiles, list, names);
}

void Index::loadDictList(QPtrList<File> &fileList, const QStringList &dictList, const QStringList &dictNameList)
{
	fileList.clear();

	// check if we have a dict
	if (dictList.size() < 1)
	{
		msgerr(i18n("No dictionaries in list!"));
		return;
	}

	QStringList::ConstIterator it;
	QStringList::ConstIterator dictIt;
	for (it = dictList.begin(), dictIt = dictNameList.begin(); it != dictList.end(); ++it, ++dictIt)
	{
		File *f = new File(*it, *dictIt);
		// our ugly substitute for exceptions
		if (f->isValid())
			fileList.append(f);
		else
			delete f;
	}
}

QStringList Index::doSearch(File &file, const QString &text)
{
	// Do a binary search to find an entry that matches text
	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QCString eucString = codec.fromUnicode(text);

	QString prevResult;

	Array<const uint32_t> index = file.index();
	Array<const unsigned char> dict = file.dict();
	int lo = 0;
	int hi = index.size() - 1;
	unsigned cur;
	int comp = 0;

	do
	{
		cur = (hi + lo) / 2;
		comp = stringCompare(file, cur, eucString);

		if (comp < 0)
			hi = cur - 1;
		else if (comp > 0)
			lo = cur + 1;
	}
	while(hi >= lo && comp != 0 && !(hi == 0 && lo == 0));
	QStringList results;
	// A match?
	if (comp == 0)
	{
		// wheel back to make sure we get the first matching entry
		while(cur - 1 && 0 == stringCompare(file, cur - 1, eucString))
			--cur;

		// output every matching entry
		while(cur < index.size() && 0 == stringCompare(file, cur, eucString))
		{
			// because the index doesn't point
			// to the start of the line, find the
			// start of the line:
			int i = 0;
			while(file.lookup(cur, i - 1) != 0x0a) --i;

			QByteArray bytes(0);
			while(file.lookup(cur, i) != 0x0a) // get to end of our line
			{
				const char eucchar = file.lookup(cur, i);
				bytes.resize(bytes.size() + 1);
				bytes[bytes.size() - 1] = eucchar;
				++i;
			}

			QString result = codec.toUnicode(bytes) + QString("\n");
			if (prevResult != result)
			{
				results.append(result);
				prevResult = result;
			}

			++cur;
		}
	}

	// return all the entries found, or null if no match
	return results;
}

SearchResult Index::scanResults(QRegExp regexp, QStringList results, bool common)
{
	unsigned int num = 0;
	unsigned int fullNum = 0;

	SearchResult ret;
	
	//ret.results = results; //not here..
	
	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT " || (*itr).left(8) == "HEADING ")
		{
			ret.list.append(parse(*itr));
			continue;
		}

		int found = regexp.search(*itr);

		if (found >= 0)
		{
			++fullNum;
			if ((*itr).find(QString("(P)")) >= 0 || !common)
			{
				// we append HERE, so we get the exact
				// results we have in ret.list
				
				ret.results.append(*itr);
				ret.list.append(parse(*itr));
				++num;
			}
		}
	}

	ret.count = num;
	ret.outOf = fullNum;
	ret.common = common;
	return ret;
}

SearchResult Index::search(QRegExp regexp, const QString &text, bool common)
{
	QStringList results;
	for (QPtrListIterator<File> file(dictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	SearchResult res = scanResults(regexp, results, common);
	res.text = text;
	return res;
}

SearchResult Index::scanKanjiResults(QRegExp regexp, QStringList results, bool common)
{
	unsigned int num = 0;
	unsigned int fullNum = 0;
	const bool jmyCount = false; // don't count JinMeiYou as common
	SearchResult ret;
	ret.results = results;

	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT " || (*itr).left(8) == "HEADING ")
		{
			ret.list.append(kanjiParse(*itr));
			continue;
		}

		int found = regexp.search(*itr);

		if (found >= 0)
		{
			++fullNum;
			// common entries have G[1-8] (jouyou)
			QRegExp comregexp(jmyCount ? "G[1-9]" : "G[1-8]");
			if ((*itr).find(comregexp) >= 0 || !common)
			{
				ret.list.append(kanjiParse(*itr));
				++num;
			}
		}
	}

	ret.count = num;
	ret.outOf = fullNum;
	ret.common = common;
	return ret;
}

SearchResult Index::searchKanji(QRegExp regexp, const QString &text,  bool common)
{
	QStringList results;
	for (QPtrListIterator<File> file(kanjiDictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	SearchResult res = scanKanjiResults(regexp, results, common);
	res.text = text;
	return res;
}

SearchResult Index::searchPrevious(QRegExp regexp, const QString &text, SearchResult list, bool common)
{
	SearchResult res;

	if (firstEntry(list).extendedKanjiInfo())
		res = scanKanjiResults(regexp, list.results, common);
	else
		res = scanResults(regexp, list.results, common);

	res.text = text;
	return res;
}

QRegExp Dict::Index::createRegExp(SearchType type, const QString &text, DictionaryType dictionaryType, bool caseSensitive)
{
	QString regExp;
	switch (type)
	{
	case Search_Beginning:
		switch (textType(text))
		{
		case Dict::Text_Latin:
			regExp = "\\W%1";
			break;

		case Dict::Text_Kana:
			if (dictionaryType == Kanjidict)
				regExp = "\\W%1";
			else // edict
				regExp = "\\[%1";
			break;

		case Dict::Text_Kanji:
			regExp = "^%1";
		}
		break;
	
	case Search_FullWord:
		switch (textType(text))
		{
		case Dict::Text_Latin:
			regExp = "\\W%1\\W";
			break;

		case Dict::Text_Kana:
			if (dictionaryType == Kanjidict)
				regExp = " %1 ";
			else // edict
				regExp = "\\[%1\\]";
			break;

		case Dict::Text_Kanji:
			regExp = "^%1\\W";
		}
		break;
	
	case Search_Anywhere:
		regExp = "%1";
	}

	return QRegExp(regExp.arg(text), caseSensitive);
}

int Index::stringCompare(File &file, int index, QCString str)
{
	return eucStringCompare(file.lookup(index), str);
}

// effectively does a strnicmp on two "strings" 
// except it will make katakana and hiragana match (EUC A4 & A5)
int Dict::eucStringCompare(const char *str, const char *str2)
{
	for (unsigned i = 0; ; ++i)
	{
		unsigned char c = static_cast<unsigned char>(str[i]);
		unsigned char c2 = static_cast<unsigned char>(str2[i]);
		if ((c2 == '\0') || (c == '\0'))
			return 0;

		if ((i % 2) == 0)
		{
			if (c2 == 0xA5)
				c2 = 0xA4;

			if (c == 0xA5)
				c = 0xA4;
		}

		if ((c2 >= 'A') && (c2 <= 'Z')) c2 |= 0x20; /*fix ucase*/
		if ((c >= 'A') && (c <= 'Z')) c |= 0x20;

		if (c2 != c)
			return (int)c2 - (int)c;
	}

	return 0;
}

bool Dict::isEUC(unsigned char c)
{
	return (c & 0x80);
}

Entry Dict::parse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Entry(raw.right(length - 5));
	if (raw.left(8) == "HEADING ")
		return Entry(raw.right(length - 8), true);

	QString reading;
	QString kanji;
	QStringList meanings;
	QString curmeaning;
	bool firstmeaning = true;
	QCString parsemode("kanji");

	unsigned int i;
	for (i = 0; i < length; i++)
	{
		QChar ichar(raw.at(i));

		if (ichar == '[')
		{
			parsemode = "reading";
		}
		else if (ichar == ']')
		{
			// do nothing
		}
		else if (ichar == '/')
		{
			if (!firstmeaning)
			{
				meanings.append(curmeaning);
				curmeaning = "";
			}
			else
			{
				firstmeaning = false;
				parsemode = "meaning";
			}
		}
		else if (ichar == ' ')
		{
			if (parsemode == "meaning") // only one that needs the space
				curmeaning += ' ';
		}
		else if (parsemode == "kanji")
		{
			kanji += ichar;
		}
		else if (parsemode == "meaning")
		{
			curmeaning += ichar;
		}
		else if (parsemode == "reading")
		{
			reading += ichar;
		}
	}

	return (Entry(kanji, reading, meanings));
}

Entry Dict::kanjiParse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Entry(raw.right(length - 5));
	if (raw.left(8) == "HEADING ")
		return Entry(raw.right(length - 8), true);

	QStringList readings;
	QString kanji;
	QStringList meanings;
	QString curmeaning;
	QString curreading;

	QString strfreq;
	QString strgrade;
	QString strstrokes;
	QString strmiscount = "";

	bool prevwasspace = true;
	QChar detailname;
	QCString parsemode("kanji");

	// if there are two S entries, second is common miscount
	bool strokesset = false;

	unsigned int i;
	QChar ichar;
	for (i = 0; i < length; i++)
	{
		ichar = raw.at(i);

		if (ichar == ' ')
		{
			if (parsemode == "reading")
			{
				readings.append(curreading);
				curreading = "";
			}
			else if (parsemode == "kanji")
			{
				parsemode = "misc";
			}
			else if (parsemode == "detail")
			{
				if (detailname == 'S')
					strokesset = true;

				parsemode = "misc";
			}
			else if (parsemode == "meaning")
			{
				curmeaning += ichar;
			}
			prevwasspace = true;
		}
		else if (ichar == '{')
		{
			parsemode = "meaning";
		}
		else if (ichar == '}')
		{
			meanings.append(curmeaning);
			curmeaning = "";
		}
		else if (parsemode == "detail")
		{
			if (detailname == 'G')
			{
				strgrade += ichar;
			}
			else if (detailname == 'F')
			{
				strfreq += ichar;
			}
			else if (detailname == 'S')
			{
				if (strokesset)
					strmiscount += ichar;
				else
					strstrokes += ichar;
			}
			prevwasspace = false;
		}
		else if (parsemode == "kanji")
		{
			kanji += ichar;
		}
		else if (parsemode == "meaning")
		{
			curmeaning += ichar;
		}
		else if (parsemode == "reading")
		{
			curreading += ichar;
		}
		else if (parsemode == "misc" && prevwasspace)
		{
			if (QRegExp("[A-Za-z0-9]").search(QString(ichar)) >= 0)
				   // is non-japanese?
			{
				detailname = ichar;
				parsemode = "detail";
			}
			else
			{
				curreading = QString(ichar);
				parsemode = "reading";
			}
		}
	}

	return (Entry(kanji, readings, meanings, strgrade.toUInt(), strfreq.toUInt(), strstrokes.toUInt(), strmiscount.toUInt()));
}

QString Dict::prettyMeaning(QStringList Meanings)
{
	QString meanings;
	QStringList::Iterator it;
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
		meanings.append((*it).stripWhiteSpace()).append("; ");

	meanings.truncate(meanings.length() - 2);
	return meanings;
}

QString Dict::prettyKanjiReading(QStringList Readings)
{
	QStringList::Iterator it;
	QString html;

	for (it = Readings.begin(); it != Readings.end(); ++it)
	{
		if ((*it) == "T1")
			html += i18n("In names: ");
		else
		{
			if ((*it) == "T2")
				html += i18n("As radical: ");
			else
			{
				html += (*it).stripWhiteSpace();
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); // get rid of last ,

	return html;
}

Dict::Entry Dict::firstEntry(Dict::SearchResult result)
{
	for (QValueListIterator<Dict::Entry> it = result.list.begin(); it != result.list.end(); ++it)
	{
		if ((*it).dictName() == "__NOTSET" && (*it).header() == "__NOTSET")
			return (*it);
	}

	return Dict::Entry("__NOTHING");
}

QString Dict::firstEntryText(Dict::SearchResult result)
{
	for (QStringList::Iterator it = result.results.begin(); it != result.results.end(); ++it)
	{
		if ((*it).left(5) != "DICT " && (*it).left(7) != "HEADER ")
			return (*it);
	}

	return QString("NONE ");
}

///////////////////////////////////////////////////////////////

Entry::Entry(const QString & kanji, const QString & reading, const QStringList &meanings)
	: DictName(QString::fromLatin1("__NOTSET"))
	, Header(QString::fromLatin1("__NOTSET"))
	, Meanings(meanings)
	, Kanji(kanji)
	, KanaOnly(reading.isEmpty())
	, Readings(KanaOnly ? kanji : reading)
	, ExtendedKanjiInfo(false)
	, Grade(0)
	, Strokes(0)
	, Miscount(0)
	, Freq(0)
{
}

Entry::Entry(const QString &kanji, QStringList &readings, QStringList &meanings, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount)
	: DictName(QString::fromLatin1("__NOTSET"))
	, Header(QString::fromLatin1("__NOTSET"))
	, Meanings(meanings)
	, Kanji(kanji)
	, KanaOnly(false)
	, Readings(readings)
	, ExtendedKanjiInfo(true)
	, Grade(grade)
	, Strokes(strokes)
	, Miscount(miscount)
	, Freq(freq)
{
}

Entry::Entry(const QString &dictname)
	: KanaOnly(true)
	, ExtendedKanjiInfo(false)
{
	DictName = dictname;
}

Entry::Entry(const QString &headername, bool)
	: DictName(QString::fromLatin1("__NOTSET"))
	, Header(headername)
	, KanaOnly(true)
	, ExtendedKanjiInfo(false)
{
}

QString Entry::dictName()
{
	return DictName;
}

QString Entry::header()
{
	return Header;
}

bool Entry::kanaOnly()
{
	return KanaOnly;
}

QString Entry::kanji()
{
	return Kanji;
}

QStringList Entry::readings()
{
	return Readings;
}

QString Entry::firstReading()
{
	return *Readings.at(0);
}

QStringList Entry::meanings()
{
	return Meanings;
}

unsigned int Entry::grade()
{
	return Grade;
}

unsigned int Entry::freq()
{
	return Freq;
}

unsigned int Entry::miscount()
{
	return Miscount;
}

unsigned int Entry::strokes()
{
	return Strokes;
}

bool Entry::extendedKanjiInfo()
{
	return ExtendedKanjiInfo;
}

#include "dict.moc"
