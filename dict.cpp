// jason@katzbrown.com

#include "dict.h"
#include <kdebug.h>
#include <qfile.h>
#include <kapp.h>
#include <kglobal.h>
#include <kprocess.h>
#include <kstandarddirs.h>
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

#include <unistd.h> 
#include <stdio.h> 
#include <sys/mman.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>

Dict::Dict() : QObject()
{
	TheDict = new QStringList;
	TheKanjiDict = new QStringList;

	com = false;
}

Dict::~Dict()
{
}

bool Dict::init(bool kanjidict)
{
	KStandardDirs *dirs = KGlobal::dirs();

	//kdDebug() << "Dict:init(" << kanjidict << ")\n";
	QStringList *theDictList = 0;

	QString indexdir = dirs->saveLocation("appdata", "xjdx/", true);
	QString index;

	if (kanjidict)
		theDictList = &KanjiDictList;
	else
		theDictList = &DictList;

	// check if we have a dict
	
	int numDicts = theDictList->size();
	if (numDicts < 1)
	{
		//kdDebug() << "No dictionaries!\n";
		return false;
	}

	QStringList::Iterator it;
	for (it = theDictList->begin(); it != theDictList->end(); ++it)
	{
		// check if the dicts exist
		if (!QFile::exists(*it))
		{
			//kdDebug() << *it << " does not exist!\n";
			return false;
		}

		index = QString(indexdir).append(QFileInfo(*it).fileName()).append(".xjdx"); // the index file

		if (!QFile::exists(index))
		{
			//kdDebug() << index << " does not exist, will make now\n";

			// find the index generator executable
			KProcess proc;
			proc << KStandardDirs::findExe("kitengen") << *it << index;
			// TODO: put up a status dialog and event loop instead of blocking
			proc.start(KProcess::Block, KProcess::NoCommunication);

			if (!QFile::exists(index))
			{
				//kdDebug() << index << " still does not exist\n";
				return false;
			}
		}
	}

	struct stat buf;

	int i = kanjidict? DictList.size() : 0; // dict number in array

	for (it = theDictList->begin(); it != theDictList->end(); ++it)
	{
		//kdDebug() << "starting on " << (*it).latin1() << " now\n";
		index = QString(indexdir).append(QFileInfo(*it).fileName()).append(".xjdx"); // the index file
		
  		DictFiles[i] = open((*it).latin1(), O_RDONLY);
		//kdDebug() << "DictFiles[" << i << "] = " << DictFiles[i] << endl;

		stat((*it).latin1(), &buf);
		DictLength[i] = buf.st_size + 1;
		//kdDebug() << "buf.st_size " << buf.st_size << endl;

		if ((DictMap[i] = mmap(0, DictLength[i], PROT_READ, MAP_FILE | MAP_SHARED, DictFiles[i], 0)) == MAP_FAILED)
		{
			kapp->quit();
		}

  		IndexFiles[i] = open(index.latin1(), O_RDONLY);
		stat(index.latin1(), &buf);
		IndexLength[i] = buf.st_size + 1;
		
		indptrt[i] = IndexLength[i] / sizeof(int32_t) - 1;

		//kdDebug() << "IndexLength " << IndexLength[i] << endl;
		if ((DictIndexMap[i] = (int32_t *) mmap(0, IndexLength[i], PROT_READ, MAP_FILE | MAP_SHARED, IndexFiles[i], 0)) == MAP_FAILED)
		{
			kapp->quit();
		}

		//kdDebug() << "Dict #" << i << " is now mmapped!\n";

		i++;
	}
	return true;
}

void Dict::doSearch(QString regexp)
{
	// TIME FOR SPAGHETTI CODE! :))
	// but it works.. yaya!
	// if you call munging the encoding until it's illegible "working" :-)

	bool firstTime = true; // if it isn't, then we look for next entry
	                       // otherwise we get our bearing in the file

	//our codec
	QTextCodec *codec = QTextCodec::codecForName("eucJP");

	//unsigned char *sch_str = (const unsigned char *)((const char *)codec->fromUnicode(regexp));
	unsigned char *sch_str = regexp.latin1();

	int sch_str_len = strlen(sch_str);
	//kdDebug() << "Dict::search(), sch_str = " << QString((char *)sch_str) << ", sch_str_len = " << sch_str_len << endl;

	int32_t lo, hi, itok, lo2, hi2, schix, schiy, index_posn = 0;
	int res = 0, i;

	QString ResultString; // final results

	int32_t dic_loc;
	int hit_posn;
	int32_t res_index = 0;
	int32_t oldres_index;

	// get down and da-ti-
	// this is modelled after Jim Breen's xjdic's xjdserver()
	// thanks Mr Breen!
	
	lo = 1;
	hi = indptrt[CurrentDict];
	
	int iterlimit = 40; // maximum binary search iterations

	while (true)
	{
		if (!iterlimit--)
			break; // break if we've done this 40 times

  		it = (lo + hi) / 2; // it is now average
		//kdDebug() << "it is " << it << endl;

		//kdDebug() << "Calling stringCompare- 1\n";
		res = stringCompare(sch_str_len, sch_str); // looks for the search string and len at it
		//kdDebug() << "stringCompare done, res = " << res << endl;

		if (res == 0) // if it was a perfect match
		{
			itok = it; // ok

			lo2 = lo; // backup
			hi2 = it; // backup
			while (true)
			{
				if (lo2 + 1 >= hi2) // if they are one different break
					break;

				it = (lo2+hi2)/2;

				//kdDebug() << "stringCompare- 1.5\n";
				res = stringCompare(sch_str_len, sch_str);

				if (res == 0)
				{
					hi2 = it;
					itok = it;
					continue;
				}
				else
				{
					lo2 = it + 1;
				}
			}
			it = itok;
			res = 0;
			break;
		}

		if (res < 0) // was less than
		{
			hi = it - 1;
		}
		else         // was greater than
			lo = it + 1;

		if (lo > hi)
		{
			//kdDebug() << "lo = " << lo << ", hi = " << hi << endl;
			break;
		}

		//kdDebug() << "looping again\n";
	}

	if (res != 0) // never got a match
	{
		return;
	}

	// as the above sometimes misses the first matching entry, step back to the first
	while (true)
	{

		//kdDebug() << "stringCompare- 2\n";
		if (stringCompare(sch_str_len, sch_str) == 0) // if its a match now
		{
			it--; // lets try again going backwards (ie, to first)
			if (it == 0)
			{
				it = 1;
				break;
			}
			continue;
		}
		else // go forwards looking for match
		{
			it++;
			break;
		}
	}

	//kdDebug() << "firsttime is now false\n";

	// start of code we do if its first time or not
	Common:
	//kdDebug() << "starting common again\n";

	oldres_index = res_index;

	// if its not firstTime, we get next entry
	if (!firstTime)
	{
		index_posn++;
		hi = indptrt[CurrentDict];
		
		//kdDebug() << "looking for ANOTHER entry\n";
		res = 0;

		if (index_posn > hi)
		{
			return;
		}
		it = index_posn;
	
		//kdDebug() << "calling stringCompare- 3\n";
		res = stringCompare(sch_str_len, sch_str);
		//kdDebug() << "res = " << res << endl;
	
		if (res != 0)
		{
			return;
		}
	}

	schix = IndexLookup(it);
	schiy = schix;

	res_index = it;

	// back off to the start of this line
	while ((DictLookup(schix) != 0x0a) && (schix >= 0)) // while its not the last line or blah..
		schix--;

	schix++; // now points to first char of our line

	hit_posn = schiy - schix;
	dic_loc = schix;

	QByteArray bytes(0);
	for (i = 0; DictLookup(schix + i) != 0x0a; i++) // get to end of our line
	{
		if (i == 512)
		{
			kdWarning() << "Reply over-run!\n";
			kapp->quit();
		}

		const char eucchar = static_cast<char>(DictLookup(schix + i));
		bytes.resize(bytes.size() + 1);
		bytes[bytes.size() - 1] = eucchar;

		//kdDebug() << "eucchar = " << eucchar << endl;
		//kdDebug() << "appendChar = " << QString(appendChar) << endl;

		//kdDebug() << "end of this iteration\n";
	}
	ResultString.append(codec->toUnicode(bytes)); // add to our match result

	if (oldres_index != res_index) // no repeats
	{
		//kdDebug() << "ResultString = " << ResultString << endl;
		ResultString.append("\n"); // add a newline
	
		s.append(ResultString);
		ResultString = "";
	}

	//kdDebug() << "One Entry Done\n";
	//kdDebug() << "going to Common:\n";
	
	if (firstTime)
	{
		index_posn = res_index;
		firstTime = false;
	}
	
	goto Common; // search for more 
}

QPtrList<Entry> Dict::search(QRegExp realregexp, QString regexp, unsigned int &num, unsigned int &fullNum)
{
	fullNum = 0;
	num = 0;
	QPtrList<Entry> ret;

	QStringList::iterator itr;

	for (CurrentDict = 0; CurrentDict < DictList.size(); CurrentDict++)
	{
		it = 0;

		// add seperator
		ret.append(new Entry(*(DictNameList.at(CurrentDict))));
		num--;
		fullNum--;

		//kdDebug() << "curdic = " << CurrentDict << endl;
		//kdDebug() << "name = " << *(DictNameList.at(CurrentDict)) << endl;
		
		doSearch(regexp); // make our s

		for (itr = s.begin(); itr != s.end(); ++itr)
		{
			//kdDebug() << "Processing: " << *itr << endl;
			int found = realregexp.search(*itr);
	
			if (found < 0)
				continue;
	
			if (((*itr).find(QString("(P)")) < 0) && com) // common entries have (P)
			{
				//kdDebug() << "Not Common\n";
				fullNum++;
				continue;
			}
			ret.append(parse(*itr));
		}

		s.clear();
	}

	fullNum += ret.count();
	num += ret.count();

	return ret;
}

QPtrList<Kanji> Dict::kanjiSearch(QRegExp realregexp, const QString &regexp, unsigned int &num, unsigned int &fullNum)
{
	num = 0;
	fullNum = 0;
	QPtrList<Kanji> ret;

	for (CurrentDict = DictList.size(); CurrentDict < (DictList.size() + KanjiDictList.size()); CurrentDict++)
	{
		ret.append(new Kanji(*KanjiDictNameList.at(CurrentDict - DictList.size())));
		num--;
		fullNum--;
		doSearch(regexp);
	}

	QStringList::iterator itr;
	for (itr = s.begin(); itr != s.end(); ++itr)
	{
		//kdDebug() << "Processing: " << *itr << endl;
		int found = realregexp.search(*itr);

		if (found < 0)
			continue;

		// if this is true, then its NOT G[1-8]
		if (((*itr).find(QRegExp("G[1-8]")) < 0) && com) // common entries
		                                            // have G[1-8] 
		                                            // (jouyou)
		{
			fullNum++;
			continue;
		}

		ret.append(kanjiParse(*itr));
	}

	fullNum += ret.count();
	num += ret.count();

	return ret;
}

// algorithms by Jim Breen for xjdic

// looks up str1's equiv at it in the xjdx file
int Dict::stringCompare(int klen, unsigned char *str1)
{
	//kdDebug() << "Dict::stringCompare(" << klen << ", " << str1 << ")\n";
	unsigned c1,c2;
	int i,rc1,rc2;

/* effectively does a strnicmp on two "strings" 
   except it will make katakana and hiragana match (EUC A4 & A5) */

//  The strcmp() function compares the two strings s1 and s2. It returns an integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than, to match, or be greater than s2. 
//  okay.. ;)

	for (i = 0; i < klen ; i++)
	{
		c1 = str1[i];

		//kdDebug() << "DictLookup(IndexLookup(" << it << ") + " << i << ")\n";
		c2 = DictLookup(IndexLookup(it) + i);

		//kdDebug() << "c2 = " << c2 << endl;

		if ((c1 == '\0') || (c2 == '\0'))
			return (0);

		if ((i % 2) ==0)
		{
			if (c1 == 0xA5)
				c1 = 0xA4;

			if (c2 == 0xA5)
				c2 = 0xA4;
		}

		if ((c1 >= 'A') && (c1 <= 'Z')) c1 |= 0x20; /*fix ucase*/
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 |= 0x20;

		if (c1 != c2 ) 
		{
			rc1 = c1;
			rc2 = c2;
			return (rc1-rc2);
		}
	}

	return (0);
}

// returns specified character from a dictionary
unsigned char Dict::DictLookup(uint32_t xit)
{
	//kdDebug() << "Dict::DictLookup(" << xit << ")\n";

	uint32_t it2;
	it2 = xit-1;
	//kdDebug() << "length is " << DictLength[CurrentDict] << ", it2 is " << it2 << endl;

	if ((it2 < 0) || (it2 > DictLength[CurrentDict]))
		return (10);

	return (DictMap[CurrentDict][it2]);
}

// returns specified entry from .xjdx file
uint32_t Dict::IndexLookup(uint32_t xit)
{
	//kdDebug() << "CurrentDict " << CurrentDict << endl;
	//kdDebug() << "Dict::IndexLookup(" << xit << ")\n";

	//int32_t it2;
	//it2 = xit-1;
	//if ((it2 < 0) || (it2 > DictLength[CurrentDict]))
		//return;

	return (DictIndexMap[CurrentDict][xit]);
}
// end

void Dict::setDictList(const QStringList &list)
{
	DictList = list;
}

void Dict::setDictNameList(const QStringList &list)
{
	DictNameList = list;
}

void Dict::setKanjiDictList(const QStringList &list)
{
	KanjiDictList = list;
}

void Dict::setKanjiDictNameList(const QStringList &list)
{
	KanjiDictNameList = list;
}

void Dict::toggleCom(bool on)
{
	com = on; // toggle
}

Entry *Dict::parse(const QString &raw)
{
	unsigned int length = raw.length();
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
		else
		{
			if (ichar == ']')
			{
			}
			else
			{
				if (ichar == '/')
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
				else
				{
					if (ichar == ' ')
					{
						if (parsemode == "meaning") // only one that needs the space
							curmeaning += ' ';
					}
					else
					{
						if (parsemode == "kanji")
						{
							kanji += ichar;
						}
						else
						{
							if (parsemode == "meaning")
							{
								curmeaning += ichar;
							}
							else
							{
								if (parsemode == "reading")
								{
									reading += ichar;
								}
							}
						}
					}
				}
			}
		}
	}

	return (new Entry(kanji, reading, meanings));
}

Kanji *Dict::kanjiParse(const QString &raw)
{
	unsigned int length = raw.length();
	QStringList readings;
	QString kanji;
	QStringList meanings;
	QString curmeaning;
	QString curreading;

	QString strfreq;
	QString strgrade;
	QString strstrokes;

	bool prevwasspace = true;
	QChar detailname;
	QCString parsemode("kanji");

	unsigned int i;
	QChar ichar;
	for (i = 0; i < length; i++)
	{
		ichar = raw.at(i);
		//kdDebug() << "char: " << QString(ichar) << "parsemode: " << QString(parsemode)  << "prevwasspace: " << prevwasspace << endl;

		if (ichar == ' ')
		{
			if (parsemode == "reading")
			{
				readings.append(curreading);
				curreading = "";
			}
			else
			{
				if (parsemode == "kanji")
				{
					parsemode = "misc";
				}
				else
				{
					if (parsemode == "detail")
					{
						parsemode = "misc";
					}
					else
					{
						if (parsemode == "meaning")
						{
							curmeaning += ichar;
						}
					}
				}
			}
			prevwasspace = true;
		}
		else
		{
			if (ichar == '{')
			{
					parsemode = "meaning";
			}
			else
			{
				if (ichar == '}')
				{
					meanings.prepend(curmeaning);
					curmeaning = "";
				}
				else
				{
					if (parsemode == "detail")
					{
						if (detailname == 'G')
						{
							strgrade += ichar;
						}
						else
						{
							if (detailname == 'F')
							{
								strfreq += ichar;
							}
							else
							{
								if (detailname == 'S')
								{
									strstrokes += ichar;
								}
							}
						}
						prevwasspace = false;
					}
					else
					{
						if (parsemode == "kanji")
						{
							kanji += ichar;
						}
						else
						{
							if (parsemode == "meaning")
							{
								curmeaning += ichar;
							}
							else
							{
								if (parsemode == "reading")
								{
									curreading += ichar;
								}
								else
								{
									if (parsemode == "misc")
									{
										if (prevwasspace)
										{
											if (QRegExp("[A-Za-z0-9]").search(QString(ichar)) >= 0)
												   // is non-japanese?
											{
												detailname = ichar;
												parsemode = "detail";
											}
											else
											{
												//kdDebug() << QString(ichar) << " isn't a letter\n";
												curreading = QString(ichar);
												parsemode = "reading";
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return (new Kanji(kanji, readings, meanings, strgrade.toUInt(), strfreq.toUInt(), strstrokes.toUInt()));
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

Kanji::Kanji(QString &kanji, QStringList &readings, QStringList &meanings, unsigned int grade, unsigned int freq, unsigned int strokes)
{
	TheKanji = kanji;
	Readings = readings;
	Meanings = meanings;
	Grade = grade;
	Freq = freq;
	Strokes = strokes;

	DictName = "__NOTSET";
}

Kanji::Kanji(const QString &dictname)
{
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
/*
	// load file by file, line by line
	for (it = theDictList->begin(); it != theDictList->end(); ++it)
	{
		QFile f(*it);
		if (f.open(IO_ReadOnly))
		{
			QTextStream t(&f);
			t.readLine(); // skip first line

			QString s;
			unsigned int n = 1;
			while (!t.eof())
			{
				theTheDict->append(t.readLine());
				n++;
				kapp->processEvents();
			}
			f.close();
		}
	}
*/

#include "dict.moc"
