#ifndef KITEN_H
#define KITEN_H

#include <kmainwindow.h>
#include <qstring.h>

#include <qregexp.h>

class Dict;
class Entry;
class SearchForm;
class QCheckBox;
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
	void kanjiSearch();
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void slotUpdateConfiguration();
	void loadDict();

	private:
	Dict * _Dict;
	SearchForm *_SearchForm;
	ResultView *_ResultView;
	QCheckBox *kanjiCB;
	QCheckBox *comCB;
	bool noInit;
	bool noKanjiInit;

	KGlobalAccel *Accel;

	QString regexp; // is searched for in dict
	QRegExp realregexp; // is searched on real dict strings
	                    // things that match dict in dictionary
	void doSearch();

	ConfigureDialog *optionDialog;

	void setResults(unsigned int, unsigned int);
};

#endif
