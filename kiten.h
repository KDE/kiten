#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>

class Dict;
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

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void search();
	void readingSearch();
	void readingSearchAccel();
	void kanjiSearch();
	void kanjiSearchAccel();
	void jpWordAccel();
	void engWordAccel();
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
	void toggleIR();

	void createLearn();

	private:
	Dict * _Dict;
	ResultView *_ResultView;
	KToggleAction *kanjiCB;
	KToggleAction *irCB;
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
	QRegExp kanjiSearchItems();

	EditAction *Edit;
};

#endif
