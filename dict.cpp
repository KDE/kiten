// jason@katzbrown.com

#include "dict.h"
#include <kdebug.h>
#include <qfile.h>
#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <qtextstream.h> 
#include <qfileinfo.h> 
#include <iostream.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qcstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qstrlist.h>

#include <cassert>

// TODO: check which of these C headers are stll needed
#include <unistd.h> 
#include <stdio.h> 
#include <sys/mman.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>

namespace
{
void msgerr(const QString &msg, const QString &dict = QString::null)
{
	QString output = msg;
	if(dict != QString::null) output = msg.arg(dict);
	KMessageBox::error(0, output);
}
}

using namespace Dict;

File::File(QString path, QString n)
	: myName(n)
	, dictFile(path)
	, dictPtr((const unsigned char *)MAP_FAILED)
	, indexFile(KGlobal::dirs()->saveLocation("appdata", "xjdx/", true) + QFileInfo(path).baseName() + ".xjdx")
	, indexPtr((const uint32_t *)MAP_FAILED)
	, valid(false)
{
	if(!indexFile.exists())
	{
		// find the index generator executable
		KProcess proc;
		proc << KStandardDirs::findExe("kitengen") << path << indexFile.name();
		// TODO: put up a status dialog and event loop instead of blocking
		proc.start(KProcess::Block, KProcess::NoCommunication);
	}

	if(!dictFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open dictionary %1."), path);
		return;
	}

	dictPtr = (const unsigned char *)mmap(0, dictFile.size(), PROT_READ, MAP_SHARED, dictFile.handle(), 0);
	if(dictPtr == MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1."), path);
		return;
	}

	if(!indexFile.open(IO_ReadOnly))
	{
		msgerr(i18n("Could not open index for dictionary %1."), path);
		return;
	}

	indexPtr = (const uint32_t*)mmap(0, indexFile.size(), PROT_READ, MAP_SHARED, indexFile.handle(), 0);
	if(indexPtr == MAP_FAILED)
	{
		msgerr(i18n("Memory error when loading dictionary %1's index file."), path);
		return;
	}

	valid = true;
}

File::~File(void)
{
	if(dictPtr != MAP_FAILED)
		munmap((void *)dictPtr, dictFile.size());
	dictFile.close();

	if(indexPtr != MAP_FAILED)
		munmap((void *)indexPtr, indexFile.size());
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
	if(pos > dictFile.size()) return 10;
	return dictPtr[pos];
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
		if(f->isValid())
			fileList.append(f);
		else
			delete f;
	}
}

QStringList Index::doSearch(File &file, QString text)
{
	// Do a binary search to find an entry that matches text
	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QCString eucString = codec.fromUnicode(text);

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

		if(comp < 0)
			hi = cur - 1;
		else if(comp > 0)
			lo = cur + 1;
	}
	while(hi >= lo && comp != 0);
	QStringList results;
	// A match?
	if(comp == 0)
	{
		// wheel back to make sure we get the first matching entry
		while(cur - 1 && 0 == stringCompare(file, cur - 1, eucString))
			--cur;

		// output every matching entry
		while(cur < index.size() && 0 == stringCompare(file, cur, eucString))
		{
			// because the index doesn't point to the start of the line, find the
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
			results.append(codec.toUnicode(bytes));
			results.append("\n");
			++cur;
		}
	}

	// return all the entries found, or null if no match
	return results;
}

SearchResult Index::scanResults(QRegExp regexp, QStringList results, unsigned int &num, unsigned int &fullNum, bool common)
{
	SearchResult ret;
	ret.results = results;
	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT ")
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
				ret.list.append(parse(*itr));
				++num;
			}
		}
	}
	return ret;
}

SearchResult Index::search(QRegExp regexp, QString text, unsigned int &num, unsigned int &fullNum, bool common)
{
	fullNum = 0;
	num = 0;

	QStringList results;
	for(QPtrListIterator<File> file(dictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	return scanResults(regexp, results, num, fullNum, common);
}

SearchResult Index::searchPrevious(QRegExp regexp, SearchResult list, unsigned int &num, unsigned int &fullNum, bool common)
{
	num = 0;
	fullNum = 0;
	return scanResults(regexp, list.results, num, fullNum, common);
}

KanjiSearchResult Index::scanKanjiResults(QRegExp regexp, QStringList results, unsigned int &num, unsigned int &fullNum, bool common)
{
	const bool jmyCount = false; // don't count JinMeiYou as common
	KanjiSearchResult ret;
	ret.results = results;

	for (QStringList::Iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if ((*itr).left(5) == "DICT ")
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

	return ret;
}

KanjiSearchResult Index::searchKanji(QRegExp regexp, const QString &text, unsigned int &num, unsigned int &fullNum, bool common)
{
	num = 0;
	fullNum = 0;

	QStringList results;
	for(QPtrListIterator<File> file(kanjiDictFiles); *file; ++file)
	{
		results.append(QString("DICT ") + (*file)->name());

		results += doSearch(**file, text);
	}

	return scanKanjiResults(regexp, results, num, fullNum, common);
}

KanjiSearchResult Index::searchPreviousKanji(QRegExp regexp, KanjiSearchResult list, unsigned int &num, unsigned int &fullNum, bool common)
{
	num = 0;
	fullNum = 0;

	return scanKanjiResults(regexp, list.results, num, fullNum, common);
}

// effectively does a strnicmp on two "strings" 
// except it will make katakana and hiragana match (EUC A4 & A5)
int Index::stringCompare(File &file, int index, QCString str)
{
	for(unsigned i = 0; i < str.length(); ++i)
	{
		unsigned char c1 = static_cast<unsigned char>(str[i]);
		unsigned char c2 = file.lookup(index, i);
		if ((c1 == '\0') || (c2 == '\0'))
			return 0;

		if ((i % 2) == 0)
		{
			if (c1 == 0xA5)
				c1 = 0xA4;

			if (c2 == 0xA5)
				c2 = 0xA4;
		}

		if ((c1 >= 'A') && (c1 <= 'Z')) c1 |= 0x20; /*fix ucase*/
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 |= 0x20;

		if (c1 != c2)
			return (int)c1 - (int)c2;
	}

	return 0;
}

Entry Index::parse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Entry(raw.right(length - 5));

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
				meanings.prepend(curmeaning);
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

Kanji Index::kanjiParse(const QString &raw)
{
	unsigned int length = raw.length();
	if (raw.left(5) == "DICT ")
		return Kanji(raw.right(length - 5));

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
			meanings.prepend(curmeaning);
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

	return (Kanji(kanji, readings, meanings, strgrade.toUInt(), strfreq.toUInt(), strstrokes.toUInt(), strmiscount.toUInt()));
}

QString Dict::prettyMeaning(QStringList Meanings)
{
	QString meanings;
	QStringList::Iterator it;
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
		meanings.append(*it).append("; ");

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
				html += (*it);
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); // get rid of last ,

	return html;
}

///////////////////////////////////////////////////////////////

Entry::Entry(const QString & kanji, const QString & reading, const QStringList &meanings)
{
	Kanji = kanji;
	Meanings = meanings;

	if (reading.isEmpty()) // empty, because its kanaonly
	{
		KanaOnly = true;
		Reading = kanji;
	}
	else
	{
		KanaOnly = false;
		Reading = reading;
	}

	DictName = "__NOTSET";
}

Entry::Entry(const QString &dictname)
{
	DictName = dictname;
}

QString Entry::dictName()
{
	return DictName;
}

bool Entry::kanaOnly()
{
	return KanaOnly;
}

QString Entry::kanji()
{
	return Kanji;
}

QString Entry::reading()
{
	return Reading;
}

QStringList Entry::meanings()
{
	return Meanings;
}

///////////////////////////////////////////////////////////////

Kanji::Kanji(QString &kanji, QStringList &readings, QStringList &meanings, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount)
{
	//kdDebug() << "new kanji\n";
	TheKanji = kanji;
	Readings = readings;
	Meanings = meanings;
	Grade = grade;
	Freq = freq;
	Strokes = strokes;
	Miscount = miscount;

	DictName = "__NOTSET";
}

Kanji::Kanji(const QString &dictname)
{
	//kdDebug() << "new kanji header\n";
	DictName = dictname;
}

QString Kanji::dictName()
{
	return DictName;
}

QString Kanji::kanji()
{
	return TheKanji;
}

unsigned int Kanji::grade()
{
	return Grade;
}

unsigned int Kanji::freq()
{
	return Freq;
}

unsigned int Kanji::miscount()
{
	return Miscount;
}

unsigned int Kanji::strokes()
{
	return Strokes;
}

QStringList Kanji::readings()
{
	return Readings;
}

QStringList Kanji::meanings()
{
	return Meanings;
}

#include "dict.moc"
