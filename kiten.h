#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>
#include "dict.h"
#include "rad.h"

class Rad;
class Entry;
class EditAction;
class QCheckBox;
class QPushButton;
class ResultView;
class ConfigureDialog;
class KGlobalAccel;
class KToggleAction;

class TopLevel : public KMainWindow
{
	Q_OBJECT

	public:
	TopLevel(QWidget *parent = 0, const char *name = 0);

	signals:
	void updateLists();
	void saveLists();
	void add(Dict::Kanji);

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void search();
	void search(bool inResults);
	void searchBeginning();
	void searchEnd();
	void resultSearch();
	void searchAccel();
	void kanjiSearchAccel();
	void strokeSearch();
	void gradeSearch();
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void slotUpdateConfiguration();
	void kanjiDictChange();
	void globalListChanged();
	void globalListDirty();
	void toggleCom();
	void addToList();
	void radicalSearch();
	void radSearch(QString &);

	void createLearn();
	void configureToolBars();
	void newToolBarConfig();

	private:
	Dict::Index _Index;
	Rad _Rad;
	ResultView *_ResultView;
	KToggleAction *kanjiCB;
	KAction *irAction;
	KAction *addAction;
	KToggleAction *comCB;

	bool wholeWord;
	bool caseSensitive;

	KGlobalAccel *Accel;

	void doSearch(QString text, QRegExp regexp, bool inResults = false);
	QString clipBoardText();

	ConfigureDialog *optionDialog;

	void setResults(unsigned int, unsigned int);

	bool autoCreateLearn;
	bool isListMod;

	QRegExp searchItems();
	QRegExp readingSearchItems(bool);
	QRegExp kanjiSearchItems(bool = false);

	EditAction *Edit;

	bool readingSearch; // if this is true and no results, try with kanjiSearchItems
	bool beginningReadingSearch;

	Dict::Kanji toAddKanji;

	QValueList<Dict::SearchResult> resultHistory;
	QValueList<Dict::KanjiSearchResult> kanjiResultHistory;
};

#endif
