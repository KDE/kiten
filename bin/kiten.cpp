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

#include <QtCore/QList>
#include <QtCore/QPair>

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <ktoolbar.h>
#include <kglobalaccel.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <ktoggleaction.h>

#include <qclipboard.h>
#include <qtextcodec.h>
#include <qtimer.h>

#include "kiten.h"
#include "resultsView.h"
#include "kitenEdit.h"
/* Separting Learn */
//#include "learn.h"
#include "kitenconfig.h"
#include "configuredialog.h"
#include "historyPtrList.h"

kiten::kiten(QWidget *parent, const char *name) 
	: KMainWindow(parent)  
{
	setStandardToolBarMenuEnabled(true);
	setObjectName(QLatin1String(name)); /* Set up the config */

	config = KitenConfigSkeleton::self();
	config->readConfig();
	
	/* Set up hot keys */
	//KDE4 TODO
/*	Accel = new KGlobalAccel(this);
	(void) Accel->insert("Lookup English/Japanese word", i18n("Lookup English/Japanese Word"), i18n("Looks up current text on clipboard in the same way as if you used Kiten's regular search."), Qt::CTRL + Qt::ALT + Qt::Key_S, Qt::CTRL + Qt::ALT + Qt::Key_S, this, SLOT(searchClipboardContents()));
	Accel->readSettings(KGlobal::config());
	Accel->updateConnections();
*/
	/* Make the ResultView object */
	mainView = new ResultView(this, "mainView");
	setCentralWidget(mainView);

	setupActions();

	createGUI();

	StatusBar = statusBar();
	optionDialog = 0;

	/* Set things as they were (as told in the config) */
	comCB->setChecked(config->com());
	autoSearchToggle->setChecked(config->autosearch());
//	deinfCB->setChecked(config->deinf());
	updateConfiguration();
	applyMainWindowSettings(KGlobal::config(), "kitenWindow");

	/* What happens when links are clicked or things are selected in the clipboard */
	connect(mainView, SIGNAL(urlClick(const QString &)), SLOT(searchText(const QString &)));
	connect(kapp->clipboard(), SIGNAL(selectionChanged()), this, SLOT(searchClipboard()));

	/* See below for what else needs to be done */
	QTimer::singleShot(10, this, SLOT(finishInit()));
}

/** destructor to clean up the little bits */
kiten::~kiten()
{
    delete optionDialog;
    optionDialog = 0;
}

void kiten::setupActions() {

	/* Add the basic quit/print/prefs actions, use the gui factory for keybindings */
	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::print(this, SLOT(print()), actionCollection());
	(void) KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	//KDE4 FIXME (const QObject*) cast
	KStdAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());
	
	/* Setup the Go-to-learn-mode actions */
//	(void) new KAction(i18n("&Learn"), "pencil", CTRL+Key_L, this, SLOT(createLearn()), actionCollection(), "file_learn");
	/* TODO: put back when Dictionary Editor is reorganised */
//	(void) new KAction(i18n("&Dictionary Editor..."), "edit", 0, this, SLOT(createEEdit()), actionCollection(), "dict_editor");
	/* TODO: Replace with the new radical search launching system */
//	(void) new KAction(i18n("Ra&dical Search..."), "gear", CTRL+Key_R, this, SLOT(radicalSearch()), actionCollection(), "search_radical");
	//Create the edit box, linking to our searchMethod in the constructor.

	/* Setup the Search Actions and our custom Edit Box */
	Edit = new KitenEdit(actionCollection(), this);
	KAction *EditToolbarWidget = new KAction(actionCollection(), "EditToolbarWidget");
	EditToolbarWidget->setDefaultWidget(Edit);
//	toolBar()->addWidget(Edit->ComboBox());
	
	(void) new KAction(i18n("&Clear Search Bar"), actionCollection(), "clear_search");
	KAction *searchButton = new KAction(i18n("S&earch"), actionCollection(), "search");
	(void) new KAction(i18n("Search with &Beginning of Word"), actionCollection(), "search_beginning");
	(void) new KAction(i18n("Search &Anywhere"), actionCollection(), "search_anywhere");
	(void) new KAction(i18n("Stro&kes"), actionCollection(), "search_stroke");
	(void) new KAction(i18n("&Grade"), actionCollection(), "search_grade");

	// Set the search button to search
	connect(searchButton, SIGNAL(triggered()), this, SLOT(searchFromEdit()));
	// and the enter key from Edit
	connect(Edit, SIGNAL(returnPressed()), this, SLOT(searchFromEdit()));
	// and the selection of an item from history
	connect(Edit, SIGNAL(activated(const QString &)), this, SLOT(searchFromEdit()));


	// That's not it, that's "find as you type"...
	//connect(Edit, SIGNAL(completion(const QString &)), this, SLOT(searchFromEdit()));

	/* Setup our widgets that handle preferences */
	//deinfCB = new KToggleAction(i18n("&Deinflect Verbs in Regular Search"), 0, this, SLOT(kanjiDictChange()), actionCollection(), "deinf_toggle");
//	comCB = new KToggleAction(i18n("&Filter Rare"), "filter", CTRL+Key_F, this, SLOT(toggleCom()), actionCollection(), "common");
	comCB = new KToggleAction(i18n("&Filter Rare"), actionCollection(), "common");
//autoSearchToggle = new KToggleAction(i18n("&Automatically Search Clipboard Selections"), "find", 0, this, SLOT(kanjiDictChange()), actionCollection(), "autosearch_toggle");
	autoSearchToggle = new KToggleAction(i18n("&Automatically Search Clipboard Selections"), actionCollection(), "autosearch_toggle");
	irAction =  new KAction(i18n("Search &in Results"), actionCollection(), "search_in_results");
	(void) KStdAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());
	(void) new KAction(i18n("Configure &Global Shortcuts..."), actionCollection(), "options_configure_keybinding");

	/* Set up history interface management */
	//KDE4 TODO
/*	historyAction = new KListAction(i18n("&History"), 0, 0, 0, actionCollection(), "history");
	connect(historyAction, SIGNAL(activated(int)), this, SLOT(goInHistory(int)));
	*/
	backAction = KStdAction::back(this, SLOT(back()), actionCollection());
	forwardAction = KStdAction::forward(this, SLOT(forward()), actionCollection());
	backAction->setEnabled(false);
	forwardAction->setEnabled(false);
}

/** This is the latter part of the initialisation. */
void kiten::finishInit()
{
	StatusBar->showMessage(i18n("Initialising Dictionaries"));
	
	// if it's the application's first time starting, 
	// the app group won't exist and we show demo
	if (!kapp->sessionConfig()->hasGroup("app"))
	{
		searchTextAndRaise(QString::fromUtf8("辞書"));
	}

//	Edit->Completion()->clear(); // make sure the edit is focused initially
	StatusBar->showMessage(i18n("Welcome to Kiten"));
	setCaption(QString::null);	
}


/** This function is run on program window close.
    It saves the settings in the config. */
bool kiten::queryClose()
{
	config->setCom(comCB->isChecked());
	config->setAutosearch(autoSearchToggle->isChecked());
//	config->setDeinf(deinfCB->isChecked());
	config->writeConfig();

	saveMainWindowSettings(KGlobal::config(), "kitenWindow");
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// SEARCHING METHODS
//////////////////////////////////////////////////////////////////////////////

/** This function searches for the contents of the Edit field in the mainwindow */
void kiten::searchFromEdit()
{
	searchAndDisplay(dictQuery(Edit->currentText()));
}

/** This function is called when a kanji is clicked in the result view 
	or any other time in which we want to search for something that didn't
	come from the input box  */
void kiten::searchText(const QString text)
{
	searchAndDisplay(dictQuery(text));
}

/** This should change the Edit text to be appropriate and then begin a search
	of the dictionaries' entries. */
void kiten::searchTextAndRaise(const QString str)
{
	/* Do the search */
	searchText(str);
	
	/* get the window as we'd like it */
	raise();
}

/** This function will search things as they are put in the clipboard.
	It checks each time to see if the autoSearchToggle is set.
		This function searches for the contents of the clipboard
	filter out long selections and selections that are contained in our
	current search (alleviates problem where research occurs because of
	X's auto-add-to-clipboard-on-select feature */
void kiten::searchClipboard()
{
	if (autoSearchToggle->isChecked()) {
		QString clipboard = kapp->clipboard()->text(QClipboard::Selection).simplified();
		
		if (clipboard.length() < 40 && Edit->currentText() != clipboard)
			if(!(Edit->currentText().contains(clipboard) 
					&& historyList.current()->count() > 0))
				searchTextAndRaise(clipboard);
	}
}

/** This method performs the search and displays
  the result to the screen.. */
void kiten::searchAndDisplay(const dictQuery &query)
{
	/* keep the user informed of what we are doing */
	StatusBar->showMessage(i18n("Searching..."));
	
	/* This gorgeous incantation is all that's necessary to fill a dictQuery
		with a query and an Entrylist with all of the results form all of the
		requested dictionaries */
	EntryList *results = dictionaryManager.doSearch(query);
	
	/* synchronise the history (and store this pointer there) */
	addHistory(results);
	
	/* Add the current search to our drop down list */
	Edit->setCurrentItem(query.toString(), true);
	
	/* suppose it's about time to show the users the results. */
	displayResults(results);
}

/** Search in the previous results, identical to searchAndDisplay except for the one call */
void kiten::searchInResults()
{
	StatusBar->showMessage(i18n("Searching..."));
	
	dictQuery searchQuery(Edit->currentText());
	EntryList *results = dictionaryManager.doSearchInList(searchQuery,historyList.current());

	addHistory(results);
	Edit->setCurrentItem(searchQuery.toString(), true);
	displayResults(results);
}

/** given a set of Search Results items, this function does all that's needed
    to put the interface into an appropriate state for those searchResults */
void kiten::displayResults(EntryList *results)
{
	/* synchronise the statusbar */
	if(results->count() > 0)
	{
		QString str;
		str = i18n("Found ") + QString::number(results->count()) + i18n(" results");
		
		StatusBar->showMessage(str);
		setCaption(str);
	}
	else
	{
		QString noneFound = i18n("No results found");
		StatusBar->showMessage(noneFound);
		setCaption(noneFound);
	}
	/* sort the results */
	QStringList fred;
	results->sort(fred);
	/* synchronise the results window */
	mainView->setContents(results->toHTML());
}


//////////////////////////////////////////////////////////////////////////////
// PREFERENCES RELATED METHODS
//////////////////////////////////////////////////////////////////////////////
void kiten::slotConfigure()
{
	if (ConfigureDialog::showDialog("settings"))
		return;
	
	//ConfigureDialog didn't find an instance of this dialog, so lets create it :
	optionDialog = new ConfigureDialog(this, config);
	connect(optionDialog, SIGNAL(hidden()),this,SLOT(slotConfigureHide()));
	connect(optionDialog, SIGNAL(settingsChanged()), this, SLOT(updateConfiguration()));
	
	optionDialog->show();
}

/** This function just queues up slotConfigureDestroy() to get around the 
    SIGSEGV if you try to delete yourself if you are in the stack */
void kiten::slotConfigureHide()
{
	QTimer::singleShot(0, this, SLOT(slotConfigureDestroy()));
}

/** This function actually tears down the optionDialog */
void kiten::slotConfigureDestroy()
{
	if (optionDialog != 0 && optionDialog->isVisible() == 0)
	{
		delete optionDialog;
		optionDialog = 0;
	}
}

/* TODO: reimplement something very much like this 
void kiten::createEEdit()
{
	eEdit *_eEdit = new eEdit(PERSONALDictionaryLocation("data"), this);
	_eEdit->show();
}
*/

void kiten::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "kitenWindow");
	KEditToolbar dlg(actionCollection());
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void kiten::newToolBarConfig()
{
	createGUI("kitenui.rc");
	applyMainWindowSettings(KGlobal::config(), "kitenWindow");
}

/** Opens the dialogue for configuring the global accelerator keys. */
void kiten::configureGlobalKeys()
{
	//KDE4 TODO
//	KKeyDialog::configure(Accel, this);
}

/** This function, as the name says, updates the configuration file.  It should
    be called in EVERY case where the configuration files change. */
void kiten::updateConfiguration()
{
	//Load the dictionaries of each type that we can adjust in prefs
	foreach(QString it, config->dictionary_list())
		loadDictConfig(it);

	//Load settings for each dictionary type
	foreach(QString it, dictionaryManager.listDictFileTypes())
		dictionaryManager.loadDictSettings(it,config);

	//Update the HTML/CSS for our fonts
	mainView->updateFont();

	//Update general options for the display manager (sorting by dict, etc)
	dictionaryManager.loadSettings(*config->config());
}

/** This function loads the dictionaries from the config file for the program
  to use via the dictionaryManager object */
void kiten::loadDictConfig(const QString dictType) 
{
	KStandardDirs *dirs = KGlobal::dirs();
	KConfig *localConfig = config->config();
	localConfig->setGroup("dicts_"+dictType.toLower());	//Set the preference group

	QList< QPair<QString,QString> > dictionariesToLoad;

	if(localConfig->readEntry("__useGlobal", true)) //If we need to load the global
		dictionariesToLoad.append( qMakePair(dictType,
				dirs->findResource("data", QString("kiten/")+dictType.toLower())));
	
	QStringList dictNames = localConfig->readEntry<QStringList>("__NAMES", QStringList());
	foreach( QString name, dictNames ) {
			QString dictPath = localConfig->readEntry(name,QString());
			if(!dictPath.isEmpty() && !name.isEmpty())
				dictionariesToLoad.append( qMakePair(name,dictPath) );
	}
		
	QStringList loadedDictionaries =
		dictionaryManager.listDictionariesOfType(dictType.toLower());

	typedef QPair<QString,QString> __dictName_t; //Can't have commas in a foreach
	foreach( __dictName_t it, dictionariesToLoad) {
		//Remove from the loadedDictionaries list all the dicts that we are supposed to load
		//This will leave only those that need to be unloaded at the end
		if(loadedDictionaries.removeAll(it.first) == 0 )
			dictionaryManager.addDictionary(it.second,it.first,dictType.toLower());
	}

	foreach(QString it, loadedDictionaries)
		dictionaryManager.removeDictionary(it);
	
/*	
#define PERSONALDictionaryLocation( __X ) KGlobal::dirs()->saveLocation(__X, \
		"kiten/dictionaries/",true).append("personal")
	if(!isKanji) { //Don't load personal dicts as kanji dicts
		QString personalDict(PERSONALDictionaryLocation("data"));
		if (QFile::exists(personalDict) && 
					loadedDicts.find(personalDict) == loadedDicts.end())
		{
			dictionaryManager.addDictionary(personalDict,"Personal",dictType.lower());
		}
	}
*/	
}



/** This function allows one to print out the currently displayed result. */
void kiten::print()
{
//	_ResultView->toHTML();
}

/******************************************************************************
  HISTORY HANDLING FUNCTIONS
******************************************************************************/

/** given one Search Result, it addes it to the history list the logic in it
   exists to maintain the history list of a certain size.  Note that this method
   does not display the EntryList it is given... so you can add something to the
   history and display it seperately. */
void kiten::addHistory(EntryList *result)
{
	historyList.addItem(result);
	enableHistoryButtons();
}

/** This goes back one item in the history and displays */
void kiten::back(void)
{
	historyList.prev();
	displayHistoryItem();
}

/** This goes forward one item in the history and displays */
void kiten::forward(void)
{
	historyList.next();
	displayHistoryItem();
}

/** This method just sets the current element in the list and triggers the display. */
void kiten::goInHistory(int index)
{
	historyList.setCurrent(index);
	displayHistoryItem();
}

void kiten::displayHistoryItem() {
	Edit->setCurrentItem(historyList.current()->getQuery().toString(), true);
	enableHistoryButtons();

	displayResults( historyList.current() );
}

/** This function determines whether the forward and back buttons should be
	enabled.  It is currently done independently of what action has just 
	occurred. */
void kiten::enableHistoryButtons()
{
	backAction->setEnabled(historyList.index() > 0);
	forwardAction->setEnabled(historyList.index()+1 < historyList.count());
}

#include "kiten.moc"
