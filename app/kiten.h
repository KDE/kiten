/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>
			 (C) 2006 Joseph Kerian <jkerian@gmail.com>
			 (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef KITEN_H
#define KITEN_H

#include "DictQuery.h"
#include "Entry.h"
#include "DictionaryManager.h"
#include "HistoryPtrList.h"

#include <kxmlguiwindow.h>

class KAction;
class KitenConfigSkeleton;
class ConfigureDialog; class KitenEdit;
class searchStringInput;
class KGlobalAccel;
class KStatusBar;
class KSystemTrayIcon;
class KToggleAction;
class KListAction;
class KProcess;
class ResultView;
class QDockWidget;
class EntryListView;

class kiten : public KXmlGuiWindow {
	Q_OBJECT

	// The following will be available via dbus.
public slots:
	void searchTextAndRaise(const QString&);
	void addExportListEntry(int index);

		//Constructors and other setup/takedown related methods
public:
	explicit kiten(QWidget *parent = 0, const char *name = 0);
    ~kiten();
protected:
	void setupActions();
	void setupExportListDock();
	virtual bool queryClose(); //overridden from KXmlGuiWindow (called@shutdown)
private slots:
	void finishInit();
	void focusResultView();

		//Searching related methods
	void searchFromEdit();
	void searchText(const QString&);
	void searchClipboard();
	void searchAndDisplay(const DictQuery&);
	void searchInResults();
	void displayResults(EntryList *);
	void radicalSearch();
	//void searchOnTheSpot();

		//Configuration related slots
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void configureToolBars();
	void configureGlobalKeys();
	void newToolBarConfig();
	void updateConfiguration();
	void loadDictConfig(const QString&);

		//Other
	void print();

		//History related methods
	void back();
	void forward();
	void goInHistory(int);
	void displayHistoryItem();
	void addHistory(EntryList*);
	void enableHistoryButtons();
	void setCurrentScrollValue(int value);

//	void createEEdit();
//	void kanjiDictChange();
//	void slotLearnConfigure();
//signals:
//	void saveLists();
//	void add(Entry*);


private:
	KStatusBar *StatusBar;
	DictionaryManager dictionaryManager;

	searchStringInput *inputManager;
	ResultView *mainView;

	KToggleAction *autoSearchToggle;
	KListAction *historyAction;
	KAction *irAction;
	KAction *backAction;
	KAction *forwardAction;
	KProcess *radselect_proc;

	//TODO: this should probably be a standardaction
	QAction *globalShortcutsAction;

	//ResultView *detachedView;
	KAction *globalSearchAction;

	KSystemTrayIcon *sysTrayIcon;

	//Export list related:
	QDockWidget *exportListDock;
	QWidget *exportListDockContents;
	EntryListView *exportList;

	KGlobalAccel *Accel;
	ConfigureDialog *optionDialog;
	KitenConfigSkeleton* config;

	HistoryPtrList historyList;
	QString personalDict;

};

#endif
