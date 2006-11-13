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

#include "deinf.h"
#include "dictQuery.h"
#include "entry.h"
#include "dictionary.h"
#include "historyPtrList.h"


class KitenConfigSkeleton;
class ConfigureDialog;
class KitenEdit;
class KConfig;
class KGlobalAccel;
class KStatusBar;
class KSystemTrayIcon;
class KToggleAction;
class KListAction;
class ResultView;
class QDockWidget;
class QTableWidget;

class kiten : public KMainWindow {
	Q_OBJECT

	// The following will be available via dbus.
public slots:
	void searchTextAndRaise(const QString);
	void addExportListEntry(const  QString& dict, const QString& word, const QStringList& readings, const QStringList& meanings);

		//Constructors and other setup/takedown related methods
public:
	kiten(QWidget *parent = 0, const char *name = 0);
    ~kiten();
protected:
	void setupActions();
	void setupExportListDock();
	virtual bool queryClose(); //overridden from KMainWindow (called@shutdown)
private slots:
	void finishInit();

		//Searching related methods
	void searchFromEdit();
	void searchText(const QString);
	void searchClipboard();
	void searchAndDisplay(const dictQuery&);
	void searchInResults();
	void displayResults(EntryList *);
	//void searchOnTheSpot();
		
		//Configuration related slots
	void slotConfigure();
	void slotConfigureHide();
	void slotConfigureDestroy();
	void configureToolBars();
	void configureGlobalKeys();
	void newToolBarConfig();
	void updateConfiguration();
	void loadDictConfig(const QString); 

		//Other
	void print();

		//History related methods
	void back();
	void forward();
	void goInHistory(int);
	void displayHistoryItem();
	void addHistory(EntryList*);
	void enableHistoryButtons();


//	void createEEdit();
//	void kanjiDictChange();
//	void toggleCom();
//	void slotLearnConfigure();
//signals:
//	void saveLists();
//	void add(Entry*);


private:
	KStatusBar *StatusBar;
	dictionary dictionaryManager;
	
	ResultView *mainView;
	
	KToggleAction *autoSearchToggle;
	KToggleAction *deinfCB;
	KListAction *historyAction;
	KAction *irAction;
	KToggleAction *comCB;
	KAction *backAction;
	KAction *forwardAction;

	//TODO: this should probably be a standardaction
	KAction *globalShortcutsAction; 

	//ResultView *detachedView;
	KAction *globalSearchAction;

	KSystemTrayIcon *sysTrayIcon;

	//Export list related:
	QDockWidget *exportListDock;
	QWidget *exportListDockContents;
	QTableWidget *exportList;

	KGlobalAccel *Accel;
	ConfigureDialog *optionDialog;
	KitenConfigSkeleton* config;

	KitenEdit *Edit;

	historyPtrList historyList;
	QString personalDict;


};

#endif
