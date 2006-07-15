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
#include "widgets.h"
#include "kitenEdit.h"
/* Separting Learn */
//#include "learn.h"
#include "kitenconfig.h"
#include "optiondialog.h"
#include "historyPtrList.h"

TopLevel::TopLevel(QWidget *parent, const char *name) 
	: KMainWindow(parent, name) /* KDE4 FIXME, 
	DCOPObject("kiten") */
{
#if KDE_VERSION > 305
	    setStandardToolBarMenuEnabled(true);
#endif
	/* Set up the config */
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
	kitenDCOPupdateConfiguration();
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");

/* Separating Learn
	// Starts learnmode on start
	if (config->startLearn())
		createLearn();
*/

	/* What happens when links are clicked or things are selected in the clipboard */
	connect(mainView, SIGNAL(linkClicked(const QString &)), SLOT(linkClicked(const QString &)));
	connect(kapp->clipboard(), SIGNAL(selectionChanged()), this, SLOT(autoSearch()));

	/* See below for what else needs to be done */
	QTimer::singleShot(10, this, SLOT(finishInit()));
}

/** destructor to clean up the little bits */
TopLevel::~TopLevel()
{
    delete optionDialog;
    optionDialog = 0;
}

void TopLevel::setupActions() {

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
//	toolBar()->addWidget(Edit->ComboBox());
	
	(void) new KAction(i18n("&Clear Search Bar"), actionCollection(), "clear_search");
	(void) new KAction(i18n("S&earch"), actionCollection(), "search");
	(void) new KAction(i18n("Search with &Beginning of Word"), actionCollection(), "search_beginning");
	(void) new KAction(i18n("Search &Anywhere"), actionCollection(), "search_anywhere");
	(void) new KAction(i18n("Stro&kes"), actionCollection(), "search_stroke");
	(void) new KAction(i18n("&Grade"), actionCollection(), "search_grade");

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
void TopLevel::finishInit()
{
	StatusBar->showMessage(i18n("Initialising Dictionaries"));
	
	// if it's the application's first time starting, 
	// the app group won't exist and we show demo
	//KDE4 CHANGE config ->sessionConfig
	if (!kapp->sessionConfig()->hasGroup("app"))
	{
		kitenDCOPsearch(QString::fromUtf8("辞書"));
	}

//	Edit->Completion()->clear(); // make sure the edit is focused initially
	StatusBar->showMessage(i18n("Welcome to Kiten"));
	setCaption(QString::null);	
}


/** This function is run on program window close.
    It saves the settings in the config. */
/* TODO: see about closing radselect and learnmode maybe DCOP */
bool TopLevel::queryClose()
{
	config->setCom(comCB->isChecked());
	config->setAutosearch(autoSearchToggle->isChecked());
//	config->setDeinf(deinfCB->isChecked());
	config->writeConfig();

	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// SEARCHING METHODS
//////////////////////////////////////////////////////////////////////////////


/** This function is called when a kanji is clicked in the result view */
void TopLevel::linkClicked(const QString &text)
{
	dictQuery query(text); //Pull the link into a query
	Edit->setText(text);   //Set it as our linked text
	searchAndDisplay(query);
}

/** This function searches for the contents of the Edit field in the mainwindow */
void TopLevel::localSearch()
{
	dictQuery query(Edit->text());
	searchAndDisplay(query);
}

/** This function searches for the contents of the clipboard */
/** filter out long selections and selections that are contained in our
	current search (alleviates problem where research occurs because of
	X's auto-add-to-clipboard-on-select feature */
void TopLevel::searchClipboardContents()
{
	QString text = kapp->clipboard()->text(QClipboard::Selection).simplified();
	
	if (text.length() < 80 && Edit->text().contains(text))
		kitenDCOPsearch(text);
}


/** This function will search things as they are put in the clipxoard.
	It checks each time to see if the autoSearchToggle is set.  */
void TopLevel::autoSearch()
{
	if (autoSearchToggle->isChecked())
		searchClipboardContents();
}


/* DCOP Interface here currently */
/** This should change the Edit text to be appropriate and then begin a search
	of the dictionaries' entries. */
void TopLevel::kitenDCOPsearch(const QString query)
{
	/* first, sync the search bar */
	Edit->setText(query);
	
	/* get the window as we'd like it */
	raise();

	/* now search */
	dictQuery theQuery(query);
	searchAndDisplay(theQuery);
}

/** This method performs the search and displays
  the result to the screen.. */
void TopLevel::searchAndDisplay(dictQuery &query)
{
	/* keep the user informed of what we are doing */
	StatusBar->showMessage(i18n("Searching..."));
	
	/* This gorgeous incantation is all that's necessary to fill a dictQuery
		with a query and an Entrylist with all of the results form all of the
		requested dictionaries */
	EntryList *results = dictionaryManager.doSearch(query);
	
	/* synchronise the history (and store this pointer there) */
	if(results->count() > 0)
		addHistory(results);
	
	/* suppose it's about time to show the users the results. */
	displayResults(results);
	
	//the history manager will take care of deleting this object
	//(it is a setAutoDelete(true) object)
}

/** Search in the previous results, identical to searchAndDisplay except for the one call */
void TopLevel::resultSearch()
{
	StatusBar->showMessage(i18n("Searching..."));
	
	dictQuery searchQuery(Edit->text());

	EntryList *results = dictionaryManager.doSearchInList(searchQuery,historyList.current());

	if(results->count() > 0)
		addHistory(results);

	displayResults(results);
}

/** given a set of Search Results items, this function does all that's needed
    to put the interface into an appropriate state for those searchResults */
void TopLevel::displayResults(EntryList *results)
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
		StatusBar->showMessage((i18n("No results found")));
	}

	/* synchronise the results window */
	mainView->setContents(results->toHTML());
}


//////////////////////////////////////////////////////////////////////////////
// PREFERENCES RELATED METHODS
//////////////////////////////////////////////////////////////////////////////

void TopLevel::slotConfigure()
{
	if (ConfigureDialog::showDialog("settings"))
		return;
	
	//ConfigureDialog didn't find an instance of this dialog, so lets create it :
	optionDialog = new ConfigureDialog(this, "settings");
	connect(optionDialog, SIGNAL(hidden()),this,SLOT(slotConfigureHide()));
	connect(optionDialog, SIGNAL(settingsChanged()), this, SLOT(kitenDCOPupdateConfiguration()));
//	connect(optionDialog, SIGNAL(valueChanged()), this, SIGNAL(quizConfChanged()));
	optionDialog->show();

	return;
}

/** This function just queues up slotConfigureDestroy() to get around the 
    SIGSEGV if you try to delete yourself if you are in the stack */
void TopLevel::slotConfigureHide()
{
	QTimer::singleShot(0, this, SLOT(slotConfigureDestroy()));
}

/** This function actually tears down the optionDialog */
void TopLevel::slotConfigureDestroy()
{
	if (optionDialog != 0 && optionDialog->isVisible() == 0)
	{
		delete optionDialog;
		optionDialog = 0;
	}
}

/* TODO: reimplement something very much like this 
void TopLevel::createEEdit()
{
	eEdit *_eEdit = new eEdit(PERSONALDictionaryLocation("data"), this);
	_eEdit->show();
}
*/

void TopLevel::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
	KEditToolbar dlg(actionCollection());
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void TopLevel::newToolBarConfig()
{
	createGUI("kitenui.rc");
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");
}

/** Opens the dialogue for configuring the global accelerator keys. */
void TopLevel::configureGlobalKeys()
{
	//KDE4 TODO
//	KKeyDialog::configure(Accel, this);
}

/** This function, as the name says, updates the configuration file.  It should
    be called in EVERY case where the configuration files change. */
void TopLevel::kitenDCOPupdateConfiguration()
{
	QStringList dictList = config->dictionary_list();
	for(QStringList::Iterator it=dictList.begin(); it != dictList.end(); ++it)
		loadDictConfig(*it);

	mainView->updateFont();
//	dictionaryManager.loadDisplaySettings();
}

/** This function loads the dictionaries from the config file for the program
  to use via the dictionaryManager object */
void TopLevel::loadDictConfig(const QString dictType) 
{
	KStandardDirs *dirs = KGlobal::dirs();
	KConfig localConfig(dirs->findResource("config","kitenrc"));
	
	QString globalDictName = KGlobal::dirs()->findResource("data",QString("kiten/")+dictType.toLower());

	localConfig.setGroup("dicts_"+dictType.toLower());								//Set the preference group

	bool useGlobal = localConfig.readEntry("__useGlobal", true); //Do we use the built in dict?

	QStringList dictNames = localConfig.readEntry("__NAMES", (QStringList)"");
	QStringList loadedDicts = dictionaryManager.listDictionaries();

	if (!globalDictName.isNull() && useGlobal && 
			!loadedDicts.contains(globalDictName))
		dictionaryManager.addDictionary(globalDictName,dictType,dictType.toLower());

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
	for(QStringList::const_iterator it = dictNames.constBegin(); it != dictNames.constEnd(); ++it) {
		if(!loadedDicts.contains(*it)) { //If it's already in the list, don't load
			QString dictPath = localConfig.readEntry(*it,QString());
			dictionaryManager.addDictionary(dictPath,*it,dictType.toLower());
		}
	}
}



/** This function allows one to print out the currently displayed result. */
void TopLevel::print()
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
void TopLevel::addHistory(EntryList *result)
{
	/* KDE4 FIXME
	historyList.addItem(result);
	historyAction->setItems(historyList.toStringList());
	historyAction->setCurrentItem(historyList.count()-1);
	enableHistoryButtons();
	*/
}

/** This goes back one item in the history and displays */
void TopLevel::back(void)
{
	historyList.prev();
	displayHistoryItem();
}

/** This goes forward one item in the history and displays */
void TopLevel::forward(void)
{
	historyList.next();
	displayHistoryItem();
}

/** This method just sets the current element in the list and triggers the display. */
void TopLevel::goInHistory(int index)
{
	historyList.at(index);
	displayHistoryItem();
}

void TopLevel::displayHistoryItem() {
	//Do not display history items that gave no results
	if(historyList.current()->count() < 1)
		; //Set the statusbar to an appropriate message
	else
		displayResults( historyList.current() );
	
//KDE4 FIXME	historyAction->setCurrentItem(historyList.at());
	Edit->setText(historyList.current()->getQuery().toString());
	enableHistoryButtons();
}

/** This function determines whether the forward and back buttons should be
	enabled.  It is currently done independently of what action has just 
	occurred. */
void TopLevel::enableHistoryButtons()
{
	backAction->setEnabled(historyList.at() > 0);
	forwardAction->setEnabled(historyList.at()+1 < historyList.count());
}

#include "kiten.moc"
