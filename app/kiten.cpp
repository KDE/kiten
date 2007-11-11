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

#include <kaction.h>
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
#include <ksystemtrayicon.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <qclipboard.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include <qdockwidget.h>
#include <qtablewidget.h>
#include <qboxlayout.h>
#include <QApplication>
#include <kapplication.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kurl.h>
#include <kfiledialog.h>

#include "resultsView.h"
#include "kitenEdit.h"
#include "wordType.h"
/* Separting Learn */
//#include "learn.h"
#include "kitenconfig.h"
#include "configuredialog.h"
#include "HistoryPtrList.h"
#include "entryListView.h"

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
	/* Make the ResultView object */
	mainView = new ResultView(this, "mainView");

	/* Create the export list */
	setupExportListDock();

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
	comCB->setChecked(config->com());
	autoSearchToggle->setChecked(config->autosearch());
//	deinfCB->setChecked(config->deinf());
	updateConfiguration();
	applyMainWindowSettings( KGlobal::config()->group( "kitenWindow") );

	/* What happens when links are clicked or things are selected in the clipboard */
	connect(mainView, SIGNAL(urlClicked(const QString &)), SLOT(searchText(const QString &)));

	connect(mainView, SIGNAL(entrySpecifiedForExport(int)), this, SLOT(addExportListEntry(int)));

	connect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, SLOT(searchClipboard()));

	/* See below for what else needs to be done */
	QTimer::singleShot(10, this, SLOT(finishInit()));
}

/** destructor to clean up the little bits */
kiten::~kiten()
{
    delete optionDialog;
    optionDialog = 0;
}

#include <QFile>

void kiten::saveAs()
{
	//TODO: remember last path used
	QString filename = KFileDialog::getSaveFileName(KUrl(), i18n("Kvtml files (*.kvtml)"), this, i18n("Choose file to export to"));
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	if (!file.isOpen())
	{
		kDebug() << "ERROR: File not opened properly";
		return;
	}
	EntryListModel *model = qobject_cast<EntryListModel*>(exportList->model());
	EntryList list = model->entryList();
	file.write(list.toKVTML(0, list.size()).toUtf8());
	file.close();
}

void kiten::setupActions() {

	/* Add the basic quit/print/prefs actions, use the gui factory for keybindings */
	(void) KStandardAction::quit(this, SLOT(close()), actionCollection());
	(void) KStandardAction::print(this, SLOT(print()), actionCollection());
	(void) KStandardAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	(void) KStandardAction::saveAs(this, SLOT(saveAs()), actionCollection());
	//KDE4 FIXME (const QObject*) cast
	KStandardAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());

	/* Setup the Go-to-learn-mode actions */
//	(void) new KAction(i18n("&Learn"), "pencil", CTRL+Key_L, this, SLOT(createLearn()), actionCollection(), "file_learn");
	/* TODO: put back when Dictionary Editor is reorganised */
//	(void) new KAction(i18n("&Dictionary Editor..."), "edit", 0, this, SLOT(createEEdit()), actionCollection(), "dict_editor");
	/* TODO: Replace with the new radical search launching system */
//	(void) new KAction(i18n("Ra&dical Search..."), "gear", CTRL+Key_R, this, SLOT(radicalSearch()), actionCollection(), "search_radical");
	//Create the edit box, linking to our searchMethod in the constructor.

	/* Setup the Search Actions and our custom Edit Box */
	Edit = new KitenEdit(actionCollection(), this);
        QAction *EditToolbarWidget = actionCollection()->addAction( "EditToolbarWidget" );
        EditToolbarWidget->setText( i18n("Search t&ext") );
	qobject_cast<KAction*>( EditToolbarWidget )->setDefaultWidget(Edit);
	Edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QAction *searchButton = actionCollection()->addAction( "search" );
        searchButton->setText( i18n("S&earch") );

        QAction *a= actionCollection()->addAction( "search_beginning");
        a->setText( i18n("Search with &Beginning of Word") );

        a= actionCollection()->addAction( "search_anywhere");
        a->setText(i18n("Search &Anywhere") );

        a=actionCollection()->addAction( "search_stroke");
        a->setText( i18n("Stro&kes") );

        a=actionCollection()->addAction( "search_grade");
        a->setText( i18n("&Grade") );

	// Set the search button to search
	connect(searchButton, SIGNAL(triggered()), this, SLOT(searchFromEdit()));
	// and the enter key from Edit
	connect(Edit, SIGNAL(returnPressed()), this, SLOT(searchFromEdit()));
	// and the selection of an item from history
	connect(Edit, SIGNAL(activated(const QString &)), this, SLOT(searchFromEdit()));


	/* Extra search options */
	wordType = new WordType(this);
	QAction *WordTypeAction = actionCollection()->addAction("WordType");
        WordTypeAction->setText(i18n("Word type"));
	qobject_cast<KAction*>( WordTypeAction )->setDefaultWidget(wordType);


	// That's not it, that's "find as you type"...
	//connect(Edit, SIGNAL(completion(const QString &)), this, SLOT(searchFromEdit()));

	/* Setup our widgets that handle preferences */
	//deinfCB = new KToggleAction(i18n("&Deinflect Verbs in Regular Search"), 0, this, SLOT(kanjiDictChange()), actionCollection(), "deinf_toggle");
//	comCB = new KToggleAction(i18n("&Filter Rare"), "view-filter", CTRL+Key_F, this, SLOT(toggleCom()), actionCollection(), "common");
	comCB = actionCollection()->add<KToggleAction>("common");
        comCB->setText(i18n("&Filter Rare"));
//autoSearchToggle = new KToggleAction(i18n("&Automatically Search Clipboard Selections"), "edit-find", 0, this, SLOT(kanjiDictChange()), actionCollection(), "autosearch_toggle");
	autoSearchToggle = actionCollection()->add<KToggleAction>("autosearch_toggle");
        autoSearchToggle->setText(i18n("&Automatically Search Clipboard Selections"));
	irAction = actionCollection()->add<KToggleAction>("search_in_results");
        irAction->setText(i18n("Search &in Results"));
	(void) KStandardAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());

	//TODO: this should probably be a standardaction
	globalShortcutsAction = actionCollection()->addAction("options_configure_global_keybinding");
        globalShortcutsAction->setText(i18n("Configure &Global Shortcuts..."));
	connect(globalShortcutsAction, SIGNAL(triggered()), this, SLOT(configureGlobalKeys()));

	globalSearchAction = actionCollection()->add<KToggleAction>("search_on_the_spot");
        globalSearchAction->setText(i18n("On The Spo&t Search"));
	qobject_cast<KAction*>( globalSearchAction )->setGlobalShortcut(KShortcut("Ctrl+Alt+S"));
	connect(globalSearchAction, SIGNAL(triggered()), this, SLOT(searchOnTheSpot()));

	/* Set up history interface management */
	//KDE4 TODO
/*	historyAction = new KListAction(i18n("&History"), 0, 0, 0, actionCollection(), "history");
	connect(historyAction, SIGNAL(activated(int)), this, SLOT(goInHistory(int)));
	*/
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

//	Edit->Completion()->clear(); // make sure the edit is focused initially
	StatusBar->showMessage(i18n("Welcome to Kiten"));
	setCaption(QString());
}


/** This function is run on program window close.
    It saves the settings in the config. */
bool kiten::queryClose()
{
	config->setCom(comCB->isChecked());
	config->setAutosearch(autoSearchToggle->isChecked());
//	config->setDeinf(deinfCB->isChecked());
	config->writeConfig();

	saveMainWindowSettings( KGlobal::config()->group( "kitenWindow") );
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// SEARCHING METHODS
//////////////////////////////////////////////////////////////////////////////

/**
 * Extracts options from the gui and applies them to the query
 */
void kiten::getOptionsFromGui( DictQuery& query)
{
	if (wordType->currentIndex())
	{
		query.setProperty("type", wordType->currentText());
	}
}

/** This function searches for the contents of the Edit field in the mainwindow.
 * Any gui choices will also be included here. */
void kiten::searchFromEdit()
{
	DictQuery query;

	query = Edit->currentText();
	getOptionsFromGui(query);

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

		if (clipboard.length() < 40 && Edit->currentText() != clipboard)
			if(!(Edit->currentText().contains(clipboard)
					&& historyList.current()->count() > 0))
				searchTextAndRaise(clipboard);
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
	Edit->setCurrentItem(query.toString(), true);

	/* suppose it's about time to show the users the results. */
	displayResults(results);
}

/** Search in the previous results, identical to searchAndDisplay except for the one call */
void kiten::searchInResults()
{
	StatusBar->showMessage(i18n("Searching..."));

	DictQuery searchQuery(Edit->currentText());
	EntryList *results = dictionaryManager.doSearchInList(searchQuery,historyList.current());

	addHistory(results);
	Edit->setCurrentItem(searchQuery.toString(), true);
	displayResults(results);
}

/** given a set of Search Results items, this function does all that's needed
    to put the interface into an appropriate state for those searchResults */
void kiten::displayResults(EntryList *results)
{
	/* synchronize the statusbar */
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
	QStringList dictSort;
	QStringList fieldSort = config->field_sortlist();
	if(config->dictionary_enable()=="true")
		dictSort = config->dictionary_sortlist();
	results->sort(fieldSort,dictSort);
	/* synchronize the results window */
	mainView->setContents(results->toHTML());

	/* to print the html results to file:
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
	mainView->updateFont();

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
