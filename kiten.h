#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>

class Dict;
class Entry;
class SearchForm;
class QCheckBox;
class QPushButton;
class ResultView;
class ConfigureDialog;
class KGlobalAccel;

class TopLevel : public KMainWindow
{
	Q_OBJECT

	public:
	TopLevel(QWidget *parent = 0, const char *name = 0);

	private slots:
	void close();
	void search();
	void readingSearch();
	void readingSearchAccel();
	void kanjiSearch();
	void kanjiSearchAccel();
	void jpWordAccel();
	void engWordAccel();
	void strokeSearch();
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void slotUpdateConfiguration();
	void loadDict();

	void createLearn();

	private:
	Dict * _Dict;
	SearchForm *_SearchForm;
	ResultView *_ResultView;
	QCheckBox *kanjiCB;
	QPushButton *strokeButton;
	QCheckBox *comCB;
	QCheckBox *irCB;
	bool noInit;
	bool noKanjiInit;

	KGlobalAccel *Accel;

	QString regexp; // is searched for in dict
	QRegExp realregexp; // is searched on real dict strings
	                    // things that match dict in dictionary
	void doSearch();
	QString clipBoardText();

	ConfigureDialog *optionDialog;

	void setResults(unsigned int, unsigned int);
};

#endif
