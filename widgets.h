#ifndef WIDGETS_H
#define WIDGETS_H

#include <qwidget.h>
#include <qtextedit.h>

class KLineEdit;
class KCompletion;
class Entry;
class Kanji;
class QString;
class QRegExp;
class KPushButton;

class SearchForm : public QWidget
{
	Q_OBJECT
	public:
	SearchForm(QWidget *parent = 0, const char *name = 0);
	~SearchForm();

	QRegExp searchItems();
	QRegExp readingSearchItems(bool);
	QRegExp kanjiSearchItems();

	QString lineText();

	private:
	KLineEdit *LineEdit;
	KCompletion *CompletionObj;
	KPushButton *SearchDictButton;
	KPushButton *SearchReadingButton;
	KPushButton *SearchKanjiButton;

	bool wholeWord;
	bool caseSensitive;
	bool alreadyOnce;

	//bool isEnglish(QString);

	QString goodText();

	private slots:
	void doSearch();
	void doReadingSearch();
	void doKanjiSearch();
	void returnPressed();

	public slots:
	void slotUpdateConfiguration();
	
	signals:
	void search();
	void readingSearch();
	void kanjiSearch();
};

class ResultView : public QTextEdit
{
	Q_OBJECT
	
	public:
	ResultView(QWidget *parent = 0, const char *name = 0);

	void addResult(Entry *, bool = false);
	void addKanjiResult(Kanji *);
};

#endif
