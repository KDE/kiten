#ifndef WIDGETS_H
#define WIDGETS_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qptrlist.h>

class KLineEdit;
class KCompletion;
class Entry;
class Kanji;
class QString;
class QTabWidget;
class QListViewItem;
class KListView;
class QRegExp;
class KPushButton;
class KStatusBar;
class QSpinBox;
class Dict;
class LearnList;
class QToolButton;

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
	void setLineText(const QString&);
	void setFocusNow();

	private:
	QToolButton *ClearButton;
	KLineEdit *LineEdit;
	KCompletion *CompletionObj;
	KPushButton *SearchDictButton;
	KPushButton *SearchReadingButton;
	KPushButton *SearchKanjiButton;

	bool wholeWord;
	bool caseSensitive;

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

	void addHeader(const QString &);
};

class Learn : public QWidget
{
	Q_OBJECT
	
	public:
	Learn(Dict *, QWidget *parent = 0, const char *name = 0);
	~Learn();

	public slots:
	void showKanji(QListViewItem *);
	void writeConfiguration();
	void listLoaded();
	void slotAdd();

	private slots:
	void next();
	void prev();
	void updateGrade();
	void random();
	void gradeChange(int);
	void showList();

	private:
	KPushButton *UpdateGrade;
	QSpinBox *GradeSpin;
	ResultView *View;
	KPushButton *Next;
	KPushButton *Prev;
	KPushButton *Rand;
	QPtrList<Kanji> list;
	KStatusBar *StatusBar;

	KPushButton *Add;
	KPushButton *Show;

	Dict *dict;

	void readConfiguration();

	void update(Kanji *curKanji= 0);

	LearnList *_LearnList;

	void add();
	bool addNew;
};

class LearnList : public QWidget
{
	Q_OBJECT
	
	public:
	LearnList(QWidget *parent = 0, const char *name = 0);
	~LearnList();

	KPushButton *save();
	KListView *list();
	
	private slots:
	void del();
	void updateCaption(QListViewItem *);

	signals:
	void showKanji(const QString&);
	void initDone();

	private:
	KListView *List;
	KPushButton *Save;
	KPushButton *Del;

	QTabWidget *Tabs;
};

#endif
