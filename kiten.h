/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KITEN_H
#define KITEN_H


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
	virtual bool queryClose();

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
	QString filteredClipboardText();
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
