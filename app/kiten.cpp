/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kiten.h"

#include <kxmlgui_version.h>
#include <KActionCollection>
#include <KConfig>
#include <KConfigGui>
#include <KEditToolBar>
#include <KLocalizedString>
#include <KProcess>
#include <KShortcutsDialog>
#include <KStandardAction>
#include <KStandardGuiItem>
#include <KStatusNotifierItem>
#include <KToggleAction>
#include <KXMLGUIFactory>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDockWidget>
#include <QFile>
#include <QList>
#include <QPair>
#include <QScrollBar>
#include <QStatusBar>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>

#include "configuredialog.h"
#include "dictionaryupdatemanager.h"
#include "entrylist.h"
#include "entrylistmodel.h"
#include "entrylistview.h"
#include "kitenconfig.h"
#include "resultsview.h"
#include "searchstringinput.h"
/* Separating Learn */
//#include "learn.h"

Kiten::Kiten( QWidget *parent, const char *name )
: KXmlGuiWindow( parent )
, _lastQuery( DictQuery() )
, _radselect_proc( new KProcess( this ) )
, _kanjibrowser_proc( new KProcess( this ) )
{
  _radselect_proc->setProgram( QStandardPaths::findExecutable( QStringLiteral("kitenradselect") ) );
  _kanjibrowser_proc->setProgram( QStandardPaths::findExecutable( QStringLiteral("kitenkanjibrowser") ) );
  setStandardToolBarMenuEnabled( true );
  setObjectName( QLatin1String( name ) );

  /* Set up the config */
  _config = KitenConfigSkeleton::self();
  _config->load();

  /* Set up hot keys */
  //KDE4 TODO
/*	Accel = new KGlobalAccel( this );
  (void) Accel->insert(   "Lookup English/Japanese word"
                        , i18n("Lookup English/Japanese Word")
                        , i18n( "Looks up current text on clipboard in the same way as if you used Kiten's regular search." )
                        , Qt::CTRL | Qt::ALT | Qt::Key_S
                        , Qt::CTRL | Qt::ALT | Qt::Key_S
                        , this
                        , SLOT(searchClipboardContents()) );
  Accel->readSettings( KSharedConfig::openConfig() );
  Accel->updateConnections();
*/
  /* ResultsView is our main widget, displaying the results of a search */
  _mainView = new ResultsView( this, "mainView" );

  /* Create the export list */
//	setupExportListDock();

  /* TODO: have a look at this idea
  detachedView = new ResultsView( NULL, "detachedView" );
  */

  setCentralWidget( _mainView );

  setupActions();
  // Be sure to create this manager before creating the GUI
  // as it needs to add a KAction to it.
  _dictionaryUpdateManager = new DictionaryUpdateManager( this );

  createGUI();

  _statusBar = statusBar();
  _optionDialog = nullptr;

  /* Start the system tray icon. */
  _sysTrayIcon = new KStatusNotifierItem(this);
  _sysTrayIcon->setStandardActionsEnabled(true);
  _sysTrayIcon->setAssociatedWidget(this);
  _sysTrayIcon->setIconByName(QStringLiteral("kiten"));
  _sysTrayIcon->setStatus(KStatusNotifierItem::Active);

  /* Set things as they were (as told in the config) */
  _autoSearchToggle->setChecked( _config->autosearch() );
  _inputManager->setDefaultsFromConfig();
  updateConfiguration();
  applyMainWindowSettings( KSharedConfig::openConfig()->group( "kitenWindow" ) );

  /* What happens when links are clicked or things are selected in the clipboard */
  connect(_mainView, &ResultsView::urlClicked, this, &Kiten::searchText);
  connect( QApplication::clipboard(), &QClipboard::selectionChanged,
                                this,   &Kiten::searchClipboard );
  connect(_inputManager, &SearchStringInput::search, this, &Kiten::searchFromEdit);

  connect( _mainView->verticalScrollBar(), &QAbstractSlider::valueChanged,
                                            this,   &Kiten::setCurrentScrollValue );
  /* We need to know when to reload our dictionaries if the user updated them. */
  connect(_dictionaryUpdateManager, &DictionaryUpdateManager::updateFinished, this, &Kiten::loadDictionaries);

  /* See below for what else needs to be done */
  QTimer::singleShot( 10, this, &Kiten::finishInit );
}

// Destructor to clean up the little bits
Kiten::~Kiten()
{
  if( _radselect_proc->state() != QProcess::NotRunning )
  {
    _radselect_proc->kill();
  }
  if( _kanjibrowser_proc->state() != QProcess::NotRunning )
  {
    _kanjibrowser_proc->kill();
  }
  _dictionaryManager.removeAllDictionaries();
  delete _optionDialog;
  _optionDialog = nullptr;
}

KitenConfigSkeleton* Kiten::getConfig()
{
  return _config;
}

void Kiten::setupActions()
{
  /* Add the basic quit/print/prefs actions, use the gui factory for keybindings */
  (void) KStandardAction::quit( this, SLOT(close()), actionCollection() );
  //Why the heck is KSA:print adding it's own toolbar!?
  //	(void) KStandardAction::print(this, SLOT(print()), actionCollection());
  (void) KStandardAction::preferences( this, SLOT(slotConfigure()), actionCollection() );
  //old style cast seems needed here, (const QObject*)
#if KXMLGUI_VERSION >= QT_VERSION_CHECK(5, 84, 0)
  KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
#else
  KStandardAction::keyBindings(   (const QObject*)guiFactory()
                                , SLOT(configureShortcuts())
                                , actionCollection() );
#endif

  /* Setup the Go-to-learn-mode actions */
  /* TODO: put back when Dictionary Editor is reorganised */
// 	(void) new KAction(   i18n( "&Dictionary Editor..." )
//                             , "document-properties"
//                             , 0
//                             , this
//                             , SLOT(createEEdit())
//                             , actionCollection()
//                             , "dict_editor");
  QAction *radselect = actionCollection()->addAction( QStringLiteral("radselect") );
  radselect->setText( i18n( "Radical Selector" ) );
//	radselect->setIcon( "edit-find" );
  actionCollection()->setDefaultShortcut(radselect, Qt::CTRL+Qt::Key_R );
  connect(radselect, &QAction::triggered, this, &Kiten::radicalSearch);

  QAction *kanjibrowser = actionCollection()->addAction( QStringLiteral("kanjibrowser") );
  kanjibrowser->setText( i18n( "Kanji Browser" ) );
  actionCollection()->setDefaultShortcut(kanjibrowser, Qt::CTRL+Qt::Key_K );
  connect(kanjibrowser, &QAction::triggered, this, &Kiten::kanjiBrowserSearch);

  /* Setup the Search Actions and our custom Edit Box */
  _inputManager = new SearchStringInput( this );

  QAction *searchButton = actionCollection()->addAction( QStringLiteral("search") );
  searchButton->setText( i18n( "S&earch" ) );
  // Set the search button to search
  connect(searchButton, &QAction::triggered, this, &Kiten::searchFromEdit);
  searchButton->setIcon( KStandardGuiItem::find().icon() );

  // That's not it, that's "find as you type"...
//   connect( Edit, SIGNAL(completion(QString)),
//            this,   SLOT(searchFromEdit()) );

  /* Setup our widgets that handle preferences */
//   deinfCB = new KToggleAction(   i18n( "&Deinflect Verbs in Regular Search" )
//                                 , 0
//                                 , this
//                                 , SLOT(kanjiDictChange())
//                                 , actionCollection()
//                                 , "deinf_toggle" );

  _autoSearchToggle = actionCollection()->add<KToggleAction>( QStringLiteral("autosearch_toggle") );
  _autoSearchToggle->setText( i18n( "&Automatically Search Clipboard Selections" ) );

  _irAction = actionCollection()->add<QAction>( QStringLiteral("search_in_results") );
  _irAction->setText( i18n( "Search &in Results" ) );
  connect(_irAction, &QAction::triggered, this, &Kiten::searchInResults);


  QAction *actionFocusResultsView;
  actionFocusResultsView = actionCollection()->addAction(  QStringLiteral("focusresultview")
                                                         , this
                                                         , SLOT(focusResultsView()) );
  actionCollection()->setDefaultShortcut(actionFocusResultsView, Qt::Key_Escape );
  actionFocusResultsView->setText( i18n( "Focus result view" ) );


  (void) KStandardAction::configureToolbars(   this
                                             , SLOT(configureToolBars())
                                             , actionCollection() );

  //TODO: this should probably be a standard action
  /*
  globalShortcutsAction = actionCollection()->addAction( "options_configure_global_keybinding" );
  globalShortcutsAction->setText( i18n( "Configure &Global Shortcuts..." ) );
  connect( globalShortcutsAction, SIGNAL(triggered()), this, SLOT(configureGlobalKeys()) );
  */

  //TODO: implement this
  //_globalSearchAction = actionCollection()->add<KToggleAction>( "search_on_the_spot" );
  //_globalSearchAction->setText( i18n( "On The Spo&t Search" ) );
  //KAction *temp = qobject_cast<KAction*>( _globalSearchAction );
  //KShortcut shrt( "Ctrl+Alt+S" );
  //globalSearchAction->setGlobalShortcut(shrt);  //FIXME: Why does this take ~50 seconds to return!?
  //connect(globalSearchAction, SIGNAL(triggered()), this, SLOT(searchOnTheSpot()));

  _backAction = KStandardAction::back( this, SLOT(back()), actionCollection() );
  _forwardAction = KStandardAction::forward( this, SLOT(forward()), actionCollection() );
  _backAction->setEnabled( false );
  _forwardAction->setEnabled( false );
}

void Kiten::setupExportListDock()
{
  _exportListDock = new QDockWidget( i18n( "Export List" ), this );
  _exportListDockContents = new QWidget( _exportListDock );
  _exportListDock->setWidget( _exportListDockContents );
  addDockWidget( Qt::RightDockWidgetArea, _exportListDock );


  QVBoxLayout *layout = new QVBoxLayout( _exportListDockContents );
  _exportListDockContents->setLayout( layout );

  _exportList = new EntryListView( _exportListDockContents );

  layout->addWidget( _exportList );

  _exportList->setModel( new EntryListModel( EntryList() ) );
}

void Kiten::addExportListEntry( int index )
{
  EntryListModel *model = qobject_cast<EntryListModel*>( _exportList->model() );
  if ( ! model )
    return;

  EntryList list = model->entryList();

  list << _historyList.current()->at( index )->clone();
  model->setEntryList( list );
}

// This is the latter part of the initialization
void Kiten::finishInit()
{
  _statusBar->showMessage( i18n( "Initializing Dictionaries" ) );

  // if it's the application's first time starting,
  // the app group won't exist and we show demo
  if ( _config->initialSearch() )
  {
    if ( KConfigGui::hasSessionConfig() && ! KConfigGui::sessionConfig()->hasGroup( "app" ) )
    {
      searchTextAndRaise( QStringLiteral( "辞書" ) );
      //Note to future tinkerers... DO NOT EDIT OR TRANSLATE THAT
      //it's an embedded unicode search string to find "dictionary" in japanese
    }
  }

  _inputManager->focusInput();
  _statusBar->showMessage( i18n( "Welcome to Kiten" ) );
  setCaption( QString() );
}

void Kiten::focusResultsView()
{
  _mainView->verticalScrollBar()->setFocus();
}

// This function is run on program window close.
// It saves the settings in the config.
bool Kiten::queryClose()
{
  _config->setAutosearch( _autoSearchToggle->isChecked() );
  _config->save();

  KConfigGroup configGroup = KSharedConfig::openConfig()->group( "kitenWindow" );
  saveMainWindowSettings( configGroup );
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// SEARCHING METHODS
//////////////////////////////////////////////////////////////////////////////

/**
 * This function searches for the contents of the Edit field in the mainwindow.
 * Any gui choices will also be included here.
 */
void Kiten::searchFromEdit()
{
  qDebug() << "SEARCH FROM EDIT CALLED";
  DictQuery query = _inputManager->getSearchQuery();
  if( query != _lastQuery )
  {
    _lastQuery = query;
    searchAndDisplay( query );
  }
}

/**
 * This function is called when a kanji is clicked in the result view
 * or any other time in which we want to search for something that didn't
 * come from the input box.
 */
void Kiten::searchText( const QString &text )
{
  searchAndDisplay( DictQuery( text ) );
}

/**
 * This should change the Edit text to be appropriate and then begin a search
 * of the dictionaries' entries.
 */
void Kiten::searchTextAndRaise( const QString &str )
{
  /* Do the search */
  searchText( str );
  /* get the window as we'd like it */
  raise();
}

/**
 * This function will search things as they are put in the clipboard.
 * It checks each time to see if the autoSearchToggle is set.
 * This function searches for the contents of the clipboard
 * filter out long selections and selections that are contained in our
 * current search (alleviates problem where research occurs because of
 * X's auto-add-to-clipboard-on-select feature.
 */
void Kiten::searchClipboard()
{
  if ( _autoSearchToggle->isChecked() )
  {
    QString clipboard = QApplication::clipboard()->text( QClipboard::Selection ).simplified();

    //Only search if the clipboard selection was less than 20 characters wide
    if ( clipboard.length() < 20 )
    {
      DictQuery potentialQuery( clipboard );
      //Make sure that we're not looking for a substring of the current string (needed?)
      if( ! ( potentialQuery < _inputManager->getSearchQuery() ) )
      {
        searchTextAndRaise(clipboard);
      }

      qDebug() << "Clipboard item is a substring (rejected)";
    }
    else
    {
      qDebug() << "Clipboard entry too long";
    }
  }
}

/**
 * This method performs the search and displays
 * the result to the screen.
 */
void Kiten::searchAndDisplay( const DictQuery &query )
{
  /* keep the user informed of what we are doing */
  _statusBar->showMessage( i18n( "Searching..." ) );

  /* This gorgeous incantation is all that's necessary to fill a DictQuery
    with a query and an Entrylist with all of the results form all of the
    requested dictionaries */
  EntryList *results = _dictionaryManager.doSearch( query );

  /* if there are no results */
  if ( results->size() == 0 ) //TODO: check here if the user actually prefers this
  {
    //create a modifiable copy of the original query
    DictQuery newQuery( query );

    bool tryAgain = false;

    do
    {
      //by default we don't try again
      tryAgain = false;

      //but if the matchtype is changed we try again
      if ( newQuery.getMatchType() == DictQuery::Exact )
      {
        newQuery.setMatchType( DictQuery::Beginning );
        tryAgain = true;
      }
      else if ( newQuery.getMatchType() == DictQuery::Beginning )
      {
        newQuery.setMatchType( DictQuery::Anywhere );
        tryAgain = true;
      }


      //try another search
      if ( tryAgain )
      {
        delete results;
        results = _dictionaryManager.doSearch( newQuery );

        //results means all is ok; don't try again
        if ( results->size() > 0 )
        {
          tryAgain = false;
        }
      }
    } while ( tryAgain );
  }

  /* synchronize the history (and store this pointer there) */
  addHistory( results );

  /* Add the current search to our drop down list */
  _inputManager->setSearchQuery( results->getQuery() );

  /* suppose it's about time to show the users the results. */
  displayResults( results );
}

/**
 * Search in the previous results, identical to
 * searchAndDisplay except for the one call.
 */
void Kiten::searchInResults()
{
  _statusBar->showMessage( i18n( "Searching..." ) );

  DictQuery searchQuery = _inputManager->getSearchQuery();
  EntryList *results = _dictionaryManager.doSearchInList(  searchQuery
                                                         ,_historyList.current() );

  addHistory( results );
  _inputManager->setSearchQuery( searchQuery );
  displayResults( results );
}

/**
 * Given a set of Search Results items, this function does all that's needed
 * to put the interface into an appropriate state for those searchResults.
 */
void Kiten::displayResults( EntryList *results )
{
  QString infoStr;
  /* synchronize the statusbar */
  if( results->count() > 0 )
  {
    infoStr = i18np( "Found 1 result", "Found %1 results", results->count() );
  }
  else
  {
    infoStr = i18n( "No results found" );
  }
  _statusBar->showMessage( infoStr );
  setCaption( infoStr );

  /* sort the results */
  /* synchronize the results window */
  if( results->count() > 0 )
  {
    QStringList dictSort;
    QStringList fieldSort = _config->field_sortlist();
    if( _config->dictionary_enable() == QLatin1String("true") )
    {
      dictSort = _config->dictionary_sortlist();
    }
    results->sort( fieldSort, dictSort );
    _mainView->setContents( results->toHTML() );
  }
  else
  {
    _mainView->setContents( "<html><body>" + infoStr + "</body></html>" );
  }

  _mainView->setLaterScrollValue( results->scrollValue() );


  /* //Debuggery: to print the html results to file:
  QFile file( "/tmp/lala" );
  file.open( QIODevice::WriteOnly );
  file.write( results->toHTML().toUtf8() );
  file.close();
  */
}

/*
void Kiten::searchOnTheSpot()
{
  qDebug() << "On the spot search!\n";
}
*/

void Kiten::radicalSearch()
{
  // Radselect is a KUniqueApplication, so we don't
  // need to worry about opening more than one copy
  _radselect_proc->start();
}

void Kiten::kanjiBrowserSearch()
{
  // KanjiBrowser is a KUniqueApplication, so we don't
  // need to worry about opening more than one copy
  _kanjibrowser_proc->start();
}

//////////////////////////////////////////////////////////////////////////////
// PREFERENCES RELATED METHODS
//////////////////////////////////////////////////////////////////////////////
void Kiten::slotConfigure()
{
  //If the settings dialog is open and obscured/hidden/cached, show it
  if ( ConfigureDialog::showDialog( QStringLiteral("settings") ) )
  {
    return;
  }

  //ConfigureDialog didn't find an instance of this dialog, so lets create it :
  _optionDialog = new ConfigureDialog( this, _config );
  connect( _optionDialog, SIGNAL(hidden()),
                   this,   SLOT(slotConfigureHide()) );
  connect(_optionDialog, &ConfigureDialog::settingsChanged, this, &Kiten::updateConfiguration);

  _optionDialog->show();
}

/**
 * This function just queues up slotConfigureDestroy() to get around the
 * SIGSEGV if you try to delete yourself if you are in the stack.
 */
void Kiten::slotConfigureHide()
{
  QTimer::singleShot(0, this, &Kiten::slotConfigureDestroy);
}

/**
 * This function actually tears down the optionDialog
 */
void Kiten::slotConfigureDestroy()
{
  if ( _optionDialog != nullptr && _optionDialog->isVisible() == 0 )
  {
    delete _optionDialog;
    _optionDialog = nullptr;
  }
}

/* TODO: reimplement something very much like this
void Kiten::createEEdit()
{
  eEdit *_eEdit = new eEdit( PERSONALDictionaryLocation( "data" ), this );
  _eEdit->show();
}
*/

void Kiten::configureToolBars()
{
  KConfigGroup configGroup = KSharedConfig::openConfig()->group( "kitenWindow" );
  saveMainWindowSettings( configGroup );
  KEditToolBar dlg( actionCollection() );
  connect(&dlg, &KEditToolBar::newToolBarConfig, this, &Kiten::newToolBarConfig);
  dlg.exec();
}

void Kiten::newToolBarConfig()
{
  createGUI( QStringLiteral("kitenui.rc") );
  KConfigGroup configGroup = KSharedConfig::openConfig()->group( "kitenWindow" );
  applyMainWindowSettings( configGroup );
}

/** Opens the dialog for configuring the global accelerator keys. */
void Kiten::configureGlobalKeys()
{
#if KXMLGUI_VERSION >= QT_VERSION_CHECK(5, 84, 0)
  KShortcutsDialog::showDialog(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this);
#else
  KShortcutsDialog::configure(  actionCollection()
                              , KShortcutsEditor::LetterShortcutsAllowed
                              , this );
#endif
}

/**
 * This function, as the name says, updates the configuration file.
 * It should be called in EVERY case where the configuration files change.
 */
void Kiten::updateConfiguration()
{
  loadDictionaries();

  //Update the HTML/CSS for our fonts
  displayHistoryItem();

  _inputManager->updateFontFromConfig();

  // Reset the content of the last query. This is because in case the user adds
  // new dictionaries and wants to execute the same last search, the output
  // will contain results of the new dictionary/dictionaries added.
  _lastQuery = DictQuery();

  /*: TODO: have a look at this as well
  detachedView->updateFont();
  */
}

/**
 * Loads the dictionaries, their settings and updates general
 * options for the display manager.
 */
void Kiten::loadDictionaries()
{
  // Avoid duplicates (this makes it easy when we need to reload the dictionaries).
  _dictionaryManager.removeAllDictionaries();

  //Load the dictionaries of each type that we can adjust in prefs
  foreach( const QString &it, _config->dictionary_list() )
  {
    loadDictConfig( it );
  }

  //Load settings for each dictionary type
  foreach( const QString &it, _dictionaryManager.listDictFileTypes() )
  {
    _dictionaryManager.loadDictSettings( it, _config );
  }

  //Update general options for the display manager (sorting by dict, etc)
  _dictionaryManager.loadSettings( *_config->config() );
  qDebug() << "Dictionaries loaded!";
}

/**
 * This function loads the dictionaries from the config file for the program
 * to use via the dictionaryManager object.
 */
void Kiten::loadDictConfig( const QString &dictType )
{
  KConfigGroup group = _config->config()->group( "dicts_" + dictType.toLower() );

  //A list of QPair's Name->Path
  QList< QPair<QString,QString> > dictionariesToLoad;

  //If we need to load the global
  if( group.readEntry( "__useGlobal", true ) )
  {
    QString dictionary = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral( "kiten/" ) + dictType.toLower());
    dictionariesToLoad.append( qMakePair( dictType, dictionary ) );
  }

  QStringList dictNames = group.readEntry<QStringList>( "__NAMES", QStringList() );
  foreach( const QString &name, dictNames )
  {
    QString dictPath = group.readEntry( name,QString() );
    if( ! dictPath.isEmpty() && ! name.isEmpty() )
    {
      dictionariesToLoad.append( qMakePair( name, dictPath ) );
    }
  }

  QStringList loadedDictionaries =
            _dictionaryManager.listDictionariesOfType( dictType.toLower() );

  typedef QPair<QString,QString> __dictName_t; //Can't have commas in a foreach
  foreach( const __dictName_t &it, dictionariesToLoad )
  {
    //Remove from the loadedDictionaries list all the dicts that we are supposed to load
    //This will leave only those that need to be unloaded at the end
    if( loadedDictionaries.removeAll( it.first ) == 0 )
    {
      _dictionaryManager.addDictionary( it.second, it.first, dictType.toLower() );
    }
  }

  foreach( const QString &it, loadedDictionaries )
  {
    _dictionaryManager.removeDictionary( it );
  }

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


/**
 * This function allows one to print out the currently displayed result
 */
void Kiten::print()
{
//	_ResultsView->toHTML();
}

/******************************************************************************
  HISTORY HANDLING FUNCTIONS
******************************************************************************/

/**
 * Given one Search Result, it adds it to the history list the logic in it
 * exists to maintain the history list of a certain size.  Note that this method
 * does not display the EntryList it is given... so you can add something to the
 * history and display it separately.
 */
void Kiten::addHistory( EntryList *result )
{
  _historyList.addItem( result );
  enableHistoryButtons();
}

/**
 * This goes back one item in the history and displays
 */
void Kiten::back( void )
{
  _historyList.prev();
  displayHistoryItem();
}

/**
 * This goes forward one item in the history and displays
 */
void Kiten::forward( void )
{
  _historyList.next();
  displayHistoryItem();
}

/**
 * This method just sets the current element in the list and triggers the display
 */
void Kiten::goInHistory( int index )
{
  _historyList.setCurrent( index );
  displayHistoryItem();
}

void Kiten::displayHistoryItem()
{
  if ( _historyList.current() == nullptr )
    return;

  _inputManager->setSearchQuery( _historyList.current()->getQuery() );
  enableHistoryButtons();

  displayResults( _historyList.current() );
}

/**
 * This function determines whether the forward and back buttons should be enabled.
 * It is currently done independently of what action has just occurred.
 */
void Kiten::enableHistoryButtons()
{
  _backAction->setEnabled( _historyList.index() > 0 );
  _forwardAction->setEnabled( _historyList.index() + 1 < _historyList.count() );
}

void Kiten::setCurrentScrollValue( int value )
{
  if ( _historyList.current() == nullptr )
    return;

  _historyList.current()->setScrollValue( value );
}


