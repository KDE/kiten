#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include "dict.h"
#include "rad.h"
#include "deinf.h"

class ConfigureDialog;
class EditAction;
class KGlobalAccel;
class KStatusBar;
class KToggleAction;
class KListAction;
class Learn;
class ResultView;

class TopLevel : public KMainWindow
{
	Q_OBJECT

public:
	TopLevel(QWidget *parent = 0, const char *name = 0);
    ~TopLevel();

signals:
	void saveLists();
	void add(Dict::Entry);
	void quizConfChanged();

protected:
	void closeEvent(QCloseEvent *);

private slots:
	void search(bool inResults = false);
	void ressearch(const QString&);
	void searchBeginning();
	void searchAnywhere();
	void resultSearch();
	void searchAccel();
	void autoSearch();
	void kanjiSearchAccel();
	void strokeSearch();
	void gradeSearch();
	void back();
	void forward();
	void goInHistory(int);
	void historySpotChanged();
	void slotKeyBindings();
	void slotConfigure();
	void slotLearnConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void slotUpdateConfiguration();
	void kanjiDictChange();
	void toggleCom();
	void addToList();
	RadWidget *radicalSearch();
	void radSearch(const QStringList &, unsigned int, unsigned int);

	void print();

	void createLearn();
	void learnDestroyed(Learn *);
	void createEEdit();
	void configureToolBars();
	void newToolBarConfig();

	void finishInit();

private:
	KStatusBar *StatusBar;

	Dict::Index _Index;
	Rad _Rad;
	Deinf::Index _DeinfIndex;
	ResultView *_ResultView;
	KToggleAction *kanjiCB;
	KToggleAction *autoSearchToggle;
	KToggleAction *deinfCB;
	KListAction *historyAction;
	KAction *irAction;
	KAction *addAction;
	KToggleAction *comCB;
	KAction *backAction;
	KAction *forwardAction;

	bool wholeWord;
	bool caseSensitive;

	KGlobalAccel *Accel;

	void doSearch(QString text, QRegExp regexp);
	void doSearchInResults(QString text, QRegExp regexp);
	void handleSearchResult(Dict::SearchResult);
	QString clipBoardText();

	ConfigureDialog *optionDialog;

	void setResults(unsigned int, unsigned int);

	QPtrList<Learn> learnList;
	bool autoCreateLearn;

	QRegExp searchItems();
	QRegExp readingSearchItems(bool);
	QRegExp kanjiSearchItems(bool = false);

	EditAction *Edit;

	bool readingSearch; // if this is true and no results, try with kanjiSearchItems
	bool beginningReadingSearch;
	bool edictUseGlobal;
	bool kanjidicUseGlobal;

	Dict::Entry toAddKanji;

	QValueList<Dict::SearchResult> resultHistory;
	QValueListIterator<Dict::SearchResult> currentResult;
	int currentResultIndex;
	void addHistory(Dict::SearchResult);
	void enableHistoryButtons();

	QString personalDict;
};

#endif
