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

#include "kiten.h"

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QScrollBar>

#include <kaction.h>
#include <kprocess.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <ktoolbar.h>
#include <kglobalaccel.h>
#include <kiconloader.h>
#include <kshortcutsdialog.h>
#include <klocale.h>
#include <kxmlguiwindow.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <kstandardguiitem.h>
#include <ksystemtrayicon.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <qclipboard.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include <qdockwidget.h>
#include <qtablewidget.h>
#include <qboxlayout.h>
#include <kapplication.h>
#include <kurl.h>
#include <kfiledialog.h>

#include "resultsView.h"
#include "DictQuery.h"
/* Separting Learn */
//#include "learn.h"
#include "kitenconfig.h"
#include "configuredialog.h"
#include "HistoryPtrList.h"
#include "entryListView.h"
#include "searchStringInput.h"

kiten::kiten(QWidget *parent, const char *name)
	: KXmlGuiWindow(parent)
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
	/* ResultView is our main widget, displaying the results of a search */
	mainView = new ResultView(this, "mainView");

	/* Create the export list */
//	setupExportListDock();

	/* TODO: have a look at this idea
	detachedView = new ResultView(NULL, "detachedView");
	*/

	setCentralWidget(mainView->widget());

	setupActions();

	createGUI();

	StatusBar = statusBar();
	optionDialog = 0;

	/* Start the system tray icon. */
	sysTrayIcon = new KSystemTrayIcon(windowIcon(), this);
	sysTrayIcon->show();

	/* Set things as they were (as told in the config) */
	autoSearchToggle->setChecked(config->autosearch());
	inputManager->setDefaultsFromConfig();
	updateConfiguration();
	applyMainWindowSettings( KGlobal::config()->group("kitenWindow") );

	/* What happens when links are clicked or things are selected in the clipboard */
	connect(mainView, SIGNAL(urlClicked(const QString &)), SLOT(searchText(const QString &)));
	connect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, SLOT(searchClipboard()));
	connect(inputManager, SIGNAL(search()), this, SLOT(searchFromEdit()));

	connect( mainView->view()->verticalScrollBar(),
			SIGNAL(valueChanged(int)),
			this,
			SLOT(setCurrentScrollValue(int))
		   );

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
	(void) KStandardAction::quit(this, SLOT(close()), actionCollection());
	//Why the heck is KSA:print adding it's own toolbar!?
	//	(void) KStandardAction::print(this, SLOT(print()), actionCollection());
	(void) KStandardAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	//old style cast seems needed here, (const QObject*)
	KStandardAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());

	/* Setup the Go-to-learn-mode actions */
	/* TODO: put back when Dictionary Editor is reorganised */
//	(void) new KAction(i18n("&Dictionary Editor..."), "document-properties", 0, this, SLOT(createEEdit()), actionCollection(), "dict_editor");
	KAction *radselect = actionCollection()->addAction("radselect");
	radselect->setText(i18n("Radical Selector"));
//	radselect->setIcon("edit-find");
	radselect->setShortcut(Qt::CTRL+Qt::Key_R);
	connect(radselect,SIGNAL(triggered()), this, SLOT(radicalSearch()));

	/* Setup the Search Actions and our custom Edit Box */
	inputManager = new searchStringInput(this);

	KAction *searchButton = actionCollection()->addAction( "search" );
	searchButton->setText( i18n("S&earch") );
	// Set the search button to search
	connect(searchButton, SIGNAL(triggered()), this, SLOT(searchFromEdit()));
	searchButton->setIcon(KStandardGuiItem::find().icon());

	// That's not it, that's "find as you type"...
	//connect(Edit, SIGNAL(completion(const QString &)), this, SLOT(searchFromEdit()));

	/* Setup our widgets that handle preferences */
	//deinfCB = new KToggleAction(i18n("&Deinflect Verbs in Regular Search"), 0, this, SLOT(kanjiDictChange()), actionCollection(), "deinf_toggle");

	autoSearchToggle = actionCollection()->add<KToggleAction>("autosearch_toggle");
   autoSearchToggle->setText(i18n("&Automatically Search Clipboard Selections"));

	irAction = actionCollection()->add<KAction>("search_in_results");
	irAction->setText(i18n("Search &in Results"));
	connect(irAction, SIGNAL(triggered()), this, SLOT(searchInResults()));


	(void) KStandardAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());

	//TODO: this should probably be a standardaction
	globalShortcutsAction = actionCollection()->addAction("options_configure_global_keybinding");
        globalShortcutsAction->setText(i18n("Configure &Global Shortcuts..."));
	connect(globalShortcutsAction, SIGNAL(triggered()), this, SLOT(configureGlobalKeys()));

	globalSearchAction = actionCollection()->add<KToggleAction>("search_on_the_spot");
        globalSearchAction->setText(i18n("On The Spo&t Search"));
	KAction *temp = qobject_cast<KAction*>( globalSearchAction );
	KShortcut shrt("Ctrl+Alt+S");
	//globalSearchAction->setGlobalShortcut(shrt);  //FIXME: Why does this take ~50 seconds to return!?
	connect(globalSearchAction, SIGNAL(triggered()), this, SLOT(searchOnTheSpot()));

	backAction = KStandardAction::back(this, SLOT(back()), actionCollection());
	forwardAction = KStandardAction::forward(this, SLOT(forward()), actionCollection());
	backAction->setEnabled(false);
	forwardAction->setEnabled(false);

}

void kiten::setupExportListDock()
{
	exportListDock = new QDockWidget(i18n("Export List"), this);
	exportListDockContents = new QWidget(exportListDock);
	exportListDock->setWidget(exportListDockContents);
	addDockWidget(Qt::RightDockWidgetArea, exportListDock);


	QVBoxLayout *layout = new QVBoxLayout(exportListDockContents);
	exportListDockContents->setLayout(layout);

	exportList = new EntryListView(exportListDockContents);

	layout->addWidget(exportList);

	exportList->setModel(new EntryListModel(EntryList()));

}

void kiten::addExportListEntry(int index)
{
	EntryListModel *model = qobject_cast<EntryListModel*>(exportList->model());
	if (!model) return;

	EntryList list = model->entryList();

	list << historyList.current()->at(index)->clone();
	model->setEntryList(list);
}

/** This is the latter part of the initialisation. */
void kiten::finishInit()
{
	StatusBar->showMessage(i18n("Initialising Dictionaries"));

	// if it's the application's first time starting,
	// the app group won't exist and we show demo
	if (config->initialSearch())
		if (!kapp->sessionConfig()->hasGroup("app"))
			searchTextAndRaise(QString::fromUtf8("辞書"));
			//Note to future tinkerers... DO NOT EDIT OR TRANSLATE THAT
			//it's an embedded unicode search string to find "dictionary" in japanese

//	Edit->Completion()->clear(); // make sure the edit is focused initially
	StatusBar->showMessage(i18n("Welcome to Kiten"));
	setCaption(QString());
}


/** This function is run on program window close.
    It saves the settings in the config. */
bool kiten::queryClose()
{
	config->setAutosearch(autoSearchToggle->isChecked());
	config->writeConfig();

	saveMainWindowSettings( KGlobal::config()->group( "kitenWindow") );
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// SEARCHING METHODS
//////////////////////////////////////////////////////////////////////////////

/** This function searches for the contents of the Edit field in the mainwindow.
 * Any gui choices will also be included here. */
void kiten::searchFromEdit()
{
	kDebug() << "SEARCH FROM EDIT CALLED";
	DictQuery query = inputManager->getSearchQuery();
	searchAndDisplay(query);
}

/** This function is called when a kanji is clicked in the result view
	or any other time in which we want to search for something that didn't
	come from the input box  */
void kiten::searchText(const QString &text)
{
	searchAndDisplay(DictQuery(text));
}

/** This should change the Edit text to be appropriate and then begin a search
	of the dictionaries' entries. */
void kiten::searchTextAndRaise(const QString &str)
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
		QString clipboard = QApplication::clipboard()->text(QClipboard::Selection).simplified();

		//Only search if the clipboard selection was less than 20 characters wide
		if (clipboard.length() < 20) {
			DictQuery potentialQuery(clipboard);
			//Make sure that we're not looking for a substring of the current string (needed?)
			if(!(potentialQuery < inputManager->getSearchQuery()))
				searchTextAndRaise(clipboard);
			kDebug() << "Clipboard item is a substring (rejected)";
		} else
			kDebug() << "Clipboard entry too long";
	}
}

/** This method performs the search and displays
  the result to the screen.. */
void kiten::searchAndDisplay(const DictQuery &query)
{
	/* keep the user informed of what we are doing */
	StatusBar->showMessage(i18n("Searching..."));

	/* This gorgeous incantation is all that's necessary to fill a DictQuery
		with a query and an Entrylist with all of the results form all of the
		requested dictionaries */
	EntryList *results = dictionaryManager.doSearch(query);

	/* synchronize the history (and store this pointer there) */
	addHistory(results);

	/* Add the current search to our drop down list */
	inputManager->setSearchQuery(query);

	/* suppose it's about time to show the users the results. */
	displayResults(results);
}

/** Search in the previous results, identical to searchAndDisplay except for the one call */
void kiten::searchInResults()
{
	StatusBar->showMessage(i18n("Searching..."));

	DictQuery searchQuery = inputManager->getSearchQuery();
	EntryList *results = dictionaryManager.doSearchInList(searchQuery,historyList.current());

	addHistory(results);
	inputManager->setSearchQuery(searchQuery);
	displayResults(results);
}

/** given a set of Search Results items, this function does all that's needed
    to put the interface into an appropriate state for those searchResults */
void kiten::displayResults(EntryList *results)
{
	QString infoStr;
	/* synchronize the statusbar */
	if(results->count() > 0)
		infoStr = i18np("Found 1 result", "Found %1 results", results->count());
	else
		infoStr = i18n("No results found");
	StatusBar->showMessage(infoStr);
	setCaption(infoStr);

	/* sort the results */
	/* synchronize the results window */
	if(results->count() > 0) {
		QStringList dictSort;
		QStringList fieldSort = config->field_sortlist();
		if(config->dictionary_enable()=="true")
			dictSort = config->dictionary_sortlist();
		results->sort(fieldSort,dictSort);
		mainView->setContents(results->toHTML());
	} else
		mainView->setContents("<html><body>"+infoStr+"</body></html>");

	mainView->view()->verticalScrollBar()->setValue(results->scrollValue());

	/* //Debuggery: to print the html results to file:
	QFile file("/tmp/lala");
	file.open(QIODevice::WriteOnly);
	file.write(results->toHTML().toUtf8());
	file.close();
	*/

}

/*
void kiten::searchOnTheSpot()
{
	kDebug() << "On the spot search!\n";
}

*/

void kiten::radicalSearch() {
	KProcess *local_proc = new KProcess;
	local_proc->setProgram(KStandardDirs::findExe("kitenradselect"));
	local_proc->start();
}


//////////////////////////////////////////////////////////////////////////////
// PREFERENCES RELATED METHODS
//////////////////////////////////////////////////////////////////////////////
void kiten::slotConfigure()
{
	//If the settings dialog is open and obscured/hidden/cached, show it
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
	saveMainWindowSettings(KGlobal::config()->group( "kitenWindow") );
	KEditToolBar dlg(actionCollection());
	connect(&dlg, SIGNAL(newToolBarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void kiten::newToolBarConfig()
{
	createGUI("kitenui.rc");
	applyMainWindowSettings(KGlobal::config()->group( "kitenWindow") );
}

/** Opens the dialog for configuring the global accelerator keys. */
void kiten::configureGlobalKeys()
{
	//KDE4 TODO: done?
	KShortcutsDialog::configure(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this);
}

/** This function, as the name says, updates the configuration file.  It should
    be called in EVERY case where the configuration files change. */
void kiten::updateConfiguration()
{
	//Load the dictionaries of each type that we can adjust in prefs
	foreach(const QString &it, config->dictionary_list())
		loadDictConfig(it);

	//Load settings for each dictionary type
	foreach(const QString &it, dictionaryManager.listDictFileTypes())
		dictionaryManager.loadDictSettings(it,config);

	//Update the HTML/CSS for our fonts
	displayHistoryItem();

	/*: TODO: have a look at this as well
	detachedView->updateFont();

	*/

	//Update general options for the display manager (sorting by dict, etc)
	dictionaryManager.loadSettings(*config->config());
}

/** This function loads the dictionaries from the config file for the program
  to use via the dictionaryManager object */
void kiten::loadDictConfig(const QString &dictType)
{
	KStandardDirs *dirs = KGlobal::dirs();
	KConfigGroup group = config->config()->group("dicts_"+dictType.toLower());

	QList< QPair<QString,QString> > dictionariesToLoad;

	if(group.readEntry("__useGlobal", true)) //If we need to load the global
		dictionariesToLoad.append( qMakePair(dictType,
				dirs->findResource("data", QString("kiten/")+dictType.toLower())));

	QStringList dictNames = group.readEntry<QStringList>("__NAMES", QStringList());
	foreach( const QString &name, dictNames ) {
			QString dictPath = group.readEntry(name,QString());
			if(!dictPath.isEmpty() && !name.isEmpty())
				dictionariesToLoad.append( qMakePair(name,dictPath) );
	}

	QStringList loadedDictionaries =
		dictionaryManager.listDictionariesOfType(dictType.toLower());

	typedef QPair<QString,QString> __dictName_t; //Can't have commas in a foreach
	foreach( const __dictName_t &it, dictionariesToLoad) {
		//Remove from the loadedDictionaries list all the dicts that we are supposed to load
		//This will leave only those that need to be unloaded at the end
		if(loadedDictionaries.removeAll(it.first) == 0 )
			dictionaryManager.addDictionary(it.second,it.first,dictType.toLower());
	}

	foreach(const QString &it, loadedDictionaries)
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
   history and display it separately. */
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
	if (historyList.current() == NULL) return;
	inputManager->setSearchQuery(historyList.current()->getQuery());
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

void kiten::setCurrentScrollValue(int value)
{
	if (historyList.current() == NULL) return;

	historyList.current()->setScrollValue(value);
}

#include "kiten.moc"
