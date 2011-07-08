/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#ifndef KITEN_H
#define KITEN_H

#include <KXmlGuiWindow>

#include "dictquery.h"
#include "dictionarymanager.h"
#include "entry.h"
#include "historyptrlist.h"

class KAction;
class KGlobalAccel;
class KListAction;
class KProcess;
class KStatusBar;
class KSystemTrayIcon;
class KToggleAction;
class KitenConfigSkeleton;
class KitenEdit;

class QDockWidget;

class ConfigureDialog;
class DictionaryUpdateManager;
class EntryListView;
class ResultsView;
class SearchStringInput;

class Kiten : public KXmlGuiWindow
{
  Q_OBJECT

  //Constructors and other setup/takedown related methods
  public:
    explicit Kiten( QWidget *parent = 0, const char *name = 0 );
            ~Kiten();

    KitenConfigSkeleton *getConfig();

  // The following will be available via dbus.
  public slots:
    void searchTextAndRaise( const QString &str );
    void addExportListEntry( int index );

  protected:
    void setupActions();
    void setupExportListDock();
    virtual bool queryClose(); //overridden from KXmlGuiWindow (called@shutdown)

  private slots:
    void finishInit();
    void focusResultsView();

    //Searching related methods
    void searchFromEdit();
    void searchText( const QString& );
    void searchClipboard();
    void searchAndDisplay( const DictQuery& );
    void searchInResults();
    void displayResults( EntryList* );
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
    void loadDictionaries();
    void loadDictConfig( const QString& );

    //Other
    void print();

    //History related methods
    void back();
    void forward();
    void goInHistory( int );
    void displayHistoryItem();
    void addHistory( EntryList* );
    void enableHistoryButtons();
    void setCurrentScrollValue( int value );

  //	void createEEdit();
  //	void kanjiDictChange();
  //	void slotLearnConfigure();
  //signals:
  //	void saveLists();
  //	void add( Entry* );


  private:
    KStatusBar          *_statusBar;
    DictionaryManager    _dictionaryManager;
    DictionaryUpdateManager *_dictionaryUpdateManager;
    SearchStringInput   *_inputManager;
    ResultsView         *_mainView;

    KToggleAction       *_autoSearchToggle;
    KListAction         *_historyAction;
    KAction             *_irAction;
    KAction             *_backAction;
    KAction             *_forwardAction;
    KProcess            *_radselect_proc;

    //TODO: this should probably be a standardaction
    QAction             *_globalShortcutsAction;

    //ResultsView *detachedView;
    KAction             *_globalSearchAction;

    KSystemTrayIcon     *_sysTrayIcon;

    //Export list related:
    QDockWidget         *_exportListDock;
    QWidget             *_exportListDockContents;
    EntryListView       *_exportList;

    KGlobalAccel        *_accel;
    ConfigureDialog     *_optionDialog;
    KitenConfigSkeleton *_config;

    HistoryPtrList       _historyList;
    QString              _personalDict;
};

#endif
