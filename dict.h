// jason@katzbrown.com

#ifndef DICT_H
#define DICT_H

#include <qfile.h>
#include <qmemarray.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <sys/types.h>
#include <inttypes.h>

class QRegExp;

namespace Dict
{
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

	unsigned char lookup(unsigned i, int offset = 0);
private:
	QString myName;

	QFile dictFile;
	const unsigned char * dictPtr;

	QFile indexFile;
	const uint32_t * indexPtr;

	bool valid;
};

class Entry
{
public:
	// EDict ctor
	Entry(const QString &, const QString &, const QStringList &);
	// Kanjidic ctor
	Entry(QString &, QStringList &, QStringList &, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount);
	// default (for containers)
	Entry(const QString & = QString::null);

	QString dictName();
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

class Index : public QObject
{
Q_OBJECT

public:
	Index();
	virtual ~Index();

	void setDictList(const QStringList &files, const QStringList &names);
	void setKanjiDictList(const QStringList &files, const QStringList &names);

	SearchResult search(QRegExp, QString, bool common);
	SearchResult searchKanji(QRegExp, QString, bool common);
	SearchResult searchPrevious(QRegExp, QString, SearchResult, bool common);

private:
	QPtrList<File> dictFiles;
	QPtrList<File> kanjiDictFiles;

	void loadDictList(QPtrList<File> &fileList, const QStringList &dictList, const QStringList &dictNameList);

	QStringList doSearch(File &, QString);
	SearchResult scanResults(QRegExp regexp, QStringList results, bool common);
	SearchResult scanKanjiResults(QRegExp regexp, QStringList results, bool common);
	int stringCompare(File &, int index, QCString);

	Entry parse(const QString &);
	Entry kanjiParse(const QString &);
};

QString prettyKanjiReading(QStringList);
QString prettyMeaning(QStringList);
	
}

#endif
