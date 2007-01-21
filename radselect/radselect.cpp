/* This file is part of kiten, a KDE Japanese Reference Tool
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "radselect.h"
#include "radselectview.h"
#include "kromajiedit.h"
#include "kitenEdit.h"
#include "radselectconfig.h"
#include "ui_radselectprefdialog.h"

#include <QtDBus/QtDBus>

#include <qpainter.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kconfigdialog.h>

#include <KStandardShortcut>
#include <kaction.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <kicon.h>

#include <kedittoolbar.h>

radselect::radselect() :
	KMainWindow(),
	m_view(new radselectView(this))
{
    // accept dnd
    setAcceptDrops(true);
    setCentralWidget(m_view);  //This is the main widget
    setObjectName(QLatin1String("radselect"));

    setupActions();

    // allow the view to change the statusbar
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));

	 if(!QDBusConnection::sessionBus().isConnected())
		 kDebug() << "Session Bus not found!!" <<endl;
	 else
		dbusInterface = new QDBusInterface("org.kde.kiten", "/", "",
				QDBusConnection::sessionBus());

    // connect the search signal from the m_view with our dcop routines
    connect(m_view, SIGNAL(kanjiSelected(const QStringList&)),
            this,   SLOT(sendSearch(const QStringList&)));

}

radselect::~radselect()
{
}

void radselect::loadSearchString(const QString &searchString) {
	m_currentQuery = searchString;
	changeStatusbar(searchString);
	//TODO: Parse the strokes
	QString strokeStr = m_currentQuery.getProperty("S");
	int min = 0;
	int max = 0;
	m_view->loadRadicals(m_currentQuery.getProperty("R"), min, max);
}

void radselect::setupActions()
{
    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    KStandardAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());

    Edit = new KitenEdit(actionCollection(), this);
    QAction *KitenEditAction =actionCollection()->addAction( "KitenEditWidget" );
    qobject_cast<KAction*>( KitenEditAction )->setDefaultWidget(Edit);

    QAction *kac = actionCollection()->addAction( "clear_search" );
    kac->setText(  i18n("&Clear Search Bar") );
    kac->setIcon( KIcon(BarIcon("locationbar_erase",16)) );
    connect( kac, SIGNAL(triggered()), Edit, SLOT(clear()) );
    qobject_cast<KAction*>(kac )->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));

    kac =actionCollection()->addAction( "search" );
    kac->setText( i18n("S&earch") );
    kac->setIcon( KIcon("key_enter") );
    connect( kac, SIGNAL(triggered()), this, SLOT(search()) );

    showAll = actionCollection()->add<KToggleAction>( "show_full_search" );
    showAll->setText( i18n("&Show Full Search String") );
    showAll->setIcon( KIcon("full_string") );
    showAll->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
    connect( showAll, SIGNAL(triggered()), this, SLOT(showFullSearchString()) );

    (void) KStandardAction::configureToolbars(this, SLOT(configureToolBars()),
													  actionCollection());

    statusBar()->show();

    // Apply the create the main window and ask the mainwindow to
    // automatically save settings if changed: window size, toolbar
    // position, icon size, etc.  Also to add actions for the statusbar
    // toolbar, and keybindings if necessary.
    setupGUI(Default,"radselectui.rc");
}

void radselect::saveProperties(KConfig *config) { //For suspend

    if (!m_currentQuery.isEmpty())
        config->writePathEntry("searchString", m_currentQuery);
}

void radselect::readProperties(KConfig *config) { //For resume
	loadSearchString(config->readPathEntry("searchString"));
}

void radselect::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
		event->acceptProposedAction();
}

void radselect::dropEvent(QDropEvent *event)
{
	QByteArray qba= event->encodedData("text/plain");
	if(qba.size() > 0)
		loadSearchString(qba);
}

void radselect::optionsPreferences()
{
	if(KConfigDialog::showDialog("settings"))
		return;
	KConfigDialog *dialog = new KConfigDialog(this,"settings", radselectConfigSkeleton::self());
	QWidget *preferences = new QWidget();
	Ui::radselectprefdialog layout;
	layout.setupUi(preferences);
	dialog->addPage(preferences, i18n("Settings"),"contents");
	connect(dialog,SIGNAL(settingsChanged(const QString&)), m_view, SLOT(loadSettings()));
	dialog->show();
}

void radselect::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->showMessage(text);
}

//This one is triggered if the search button is used (or the widget interface
//is in some other way given priority.
void radselect::sendSearch(const QStringList& kanji) {
	//This may need to be done differently for handling collisions
	m_currentQuery = Edit->currentText() + kanji.join("");

	changeStatusbar(m_currentQuery);

	if(dbusInterface->isValid()) {
		QDBusMessage reply = dbusInterface->call(
				QLatin1String("searchTextAndRaise"),
				m_currentQuery.toString());
		if(reply.type() == QDBusMessage::ErrorMessage)
			kDebug() << "QDBus Error: " << reply.signature() <<"<eoe>"<<endl;
	}
}


void radselect::clear() {
}

void radselect::search() {
}

void radselect::showFullSearchString(){
}

void radselect::configureToolBars()
{
	KEditToolbar dlg(actionCollection());
	if (dlg.exec())
		createGUI();

//	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
//	KEditToolbar dlg(actionCollection(), "kitenui.rc");
//	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
//	dlg.exec();
}

/*void radselect::newToolBarConfig()
{
	createGUI("kitenui.rc");
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");
}
*/

#include "radselect.moc"
