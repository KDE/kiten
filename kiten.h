#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>
#include <dict.h>

class Dict;
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
	void add(Kanji *);

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void search();
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
	void loadDict();
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
	Dict * _Dict;
	Rad * _Rad;
	ResultView *_ResultView;
	KToggleAction *kanjiCB;
	KAction *irAction;
	KAction *addAction;
	KToggleAction *comCB;

	bool noInit;
	bool noKanjiInit;
	bool wholeWord;
	bool caseSensitive;

	KGlobalAccel *Accel;

	QString regexp; // is searched for in dict
	QRegExp realregexp; // is searched on real dict strings
	                    // things that match dict in dictionary
	void doSearch();
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

	Kanji *toAddKanji;
};

#endif
