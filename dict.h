// jason@katzbrown.com

#ifndef DICT_H
#define DICT_H

#include <qptrlist.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qobject.h>
#include <qregexp.h>

#include <sys/types.h>
#include <inttypes.h>

class QRegExp;
class Entry;
class Kanji;

class Dict : public QObject
{
	Q_OBJECT

	public:
	Dict();
	~Dict();
	bool init(bool);
	void search(QString);
	QPtrList<Entry> search(QRegExp, QString, unsigned int &, unsigned int &);
	QPtrList<Kanji> kanjiSearch(QRegExp, const QString &, unsigned int &, unsigned int &);
	void setDictList(const QStringList &);
	void setDictNameList(const QStringList &);
	void setKanjiDictList(const QStringList &);
	void setKanjiDictNameList(const QStringList &);

	bool isir() { return ir; };
	bool iscom() { return com; };
	
	public slots:
	void toggleCom(bool);
	void toggleIR(bool);

	private:
	char *DictMap[40];
	uint32_t *DictIndexMap[40];
	int DictFiles[40];
	int IndexFiles[40];
	uint32_t DictLength[40];
	uint32_t IndexLength[40];
	uint32_t indptrt[40];

	void msgerr(const QString &, const QString &);
	void msgerr(const QString &);

	QStringList *results;

	void doSearch(QString);
	QStringList s; // doSearch() stores it here

	// should this be uint or int :(
	uint32_t it; // it marks the spot in the dict buffer we are currently at!

	int CurrentDict; // dict in all arrays that all functions use for searching

	int stringCompare(int klen, unsigned char *str1);
	unsigned char DictLookup(uint32_t xit);
	uint32_t IndexLookup(uint32_t xit);

	QStringList DictList;
	QStringList DictNameList;
	QStringList KanjiDictList;
	QStringList KanjiDictNameList;
	QString dictName();

	bool com;
	bool ir;

	Entry *parse(const QString &);
	Kanji *kanjiParse(const QString &);
};

class Entry
{
	public:
	Entry(const QString &, const QString &, const QStringList &);
	Entry(const QString &);

	bool kanaOnly();
	QString kanji();
	QString reading();
	QStringList meanings();
	QString dictName();
	
	private:
	QString Kanji;
	QString Reading;
	bool KanaOnly;

	QStringList Meanings;

	QString DictName;
};

class Kanji
{
	public:
	Kanji(QString &, QStringList &, QStringList &, unsigned int grade, unsigned int freq, unsigned int strokes, unsigned int miscount);
	Kanji(const QString &);

	QString kanji();
	unsigned int grade();
	unsigned int strokes();
	unsigned int miscount();
	unsigned int freq();
	QStringList readings();
	QStringList meanings();

	QString dictName();

	private:
	QString TheKanji;
	unsigned int Grade;
	unsigned int Strokes;
	unsigned int Miscount;
	unsigned int Freq;
	QStringList Readings;
	QStringList Meanings;

	QString DictName;
};

#endif
