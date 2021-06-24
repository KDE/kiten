/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "radselect.h"

#include "radselectconfig.h"
#include "radselectview.h"

#include "ui_radselectprefdialog.h"

#include <kxmlgui_version.h>
#include <KActionCollection>
#include <KConfig>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KStandardAction>
#include <KStandardShortcut>
#include <KXMLGUIFactory>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QStatusBar>

RadSelect::RadSelect()
: KXmlGuiWindow()
, m_view( new RadSelectView( this ) )
{
  // accept dnd
  setAcceptDrops( true );
  setCentralWidget( m_view );  //This is the main widget
  setObjectName( QStringLiteral( "radselect" ) );

  KStandardAction::quit( this, SLOT(close()), actionCollection() );
  KStandardAction::preferences( this, SLOT(optionsPreferences()), actionCollection() );

#if KXMLGUI_VERSION >= QT_VERSION_CHECK(5, 84, 0)
  KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
#else
  KStandardAction::keyBindings( (const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection() );
#endif

  statusBar()->show();

  // Apply the create the main window and ask the mainwindow to
  // automatically save settings if changed: window size, toolbar
  // position, icon size, etc.  Also to add actions for the statusbar
  // toolbar, and keybindings if necessary.
  setupGUI( Default, QStringLiteral("radselectui.rc") );

  // allow the view to change the statusbar
  connect( m_view, &RadSelectView::signalChangeStatusbar,
             this,   &RadSelect::changeStatusbar );

  if( ! QDBusConnection::sessionBus().isConnected() )
  {
    qDebug() << "Session Bus not found!!";
    m_dbusInterface = nullptr;
  }
  else
  {
    m_dbusInterface = new QDBusInterface(   QStringLiteral("org.kde.kiten"), QStringLiteral("/"), QLatin1String("")
                                          , QDBusConnection::sessionBus() );
  }

  // connect the search signal from the m_view with our dbus routines
  connect( m_view, &RadSelectView::kanjiSelected,
             this,   &RadSelect::sendSearch );
}

RadSelect::~RadSelect()
{
  delete m_dbusInterface;
}

void RadSelect::changeStatusbar( const QString& text )
{
  // display the text on the statusbar
  statusBar()->showMessage( text );
}

void RadSelect::dragEnterEvent( QDragEnterEvent *event )
{
  if ( event->mimeData()->hasFormat( QStringLiteral("text/plain") ) )
  {
    event->acceptProposedAction();
  }
}

void RadSelect::dropEvent( QDropEvent *event )
{
  QByteArray qba = event->mimeData()->data(QStringLiteral("text/plain"));
  if ( qba.size() > 0 )
  {
    loadSearchString( qba );
  }
}

void RadSelect::loadSearchString( const QString &searchString )
{
  m_currentQuery = searchString;
  changeStatusbar( searchString );
  //TODO: Parse the strokes
  QString strokeStr = m_currentQuery.getProperty( QStringLiteral("S") );
  int min = 0;
  int max = 0;
  m_view->loadRadicals( m_currentQuery.getProperty( QStringLiteral("R") ), min, max );
}

void RadSelect::optionsPreferences()
{
  if( KConfigDialog::showDialog( QStringLiteral("settings") ) )
  {
    return;
  }

  KConfigDialog *dialog = new KConfigDialog( this,QStringLiteral("settings"), RadSelectConfigSkeleton::self() );
  QWidget *preferences = new QWidget();
  Ui::radselectprefdialog layout;
  layout.setupUi( preferences );
  dialog->addPage( preferences, i18n( "Settings" ), QStringLiteral("help-contents") );
  connect( dialog, SIGNAL(settingsChanged(QString)),
           m_view,   SLOT(loadSettings()) );
  dialog->show();
}

void RadSelect::readProperties( const KConfigGroup &config )
{
  //For resume
  loadSearchString( config.readPathEntry( "searchString", QString() ) );
}

void RadSelect::saveProperties( KConfigGroup &config )
{
  //For suspend
  if ( ! m_currentQuery.isEmpty() )
  {
    config.writePathEntry( "searchString", m_currentQuery );
  }
}

//This one is triggered if the search button is used (or the widget interface
//is in some other way given priority.
void RadSelect::sendSearch( const QStringList& kanji )
{
  if( kanji.size() == 0 )
  {
    return;
  }

  //This may need to be done differently for handling collisions
  m_currentQuery = kanji.at( 0 );

  changeStatusbar( m_currentQuery );

  if( m_dbusInterface && m_dbusInterface->isValid() )
  {
    QDBusMessage reply = m_dbusInterface->call(  QStringLiteral( "searchTextAndRaise" )
                                               , m_currentQuery.toString() );
    if( reply.type() == QDBusMessage::ErrorMessage )
    {
      qDebug() << "QDBus Error: " << reply.signature() << "<eoe>";
    }
  }
}


