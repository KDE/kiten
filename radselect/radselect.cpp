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
#include "radselectconfig.h"
#include "ui_radselectprefdialog.h"

#include <QtDBus/QtDBus>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <kactioncollection.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kapplication.h>

#include <KStandardShortcut>
#include <kaction.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <kicon.h>

#include <kedittoolbar.h>
#include <khistorycombobox.h>

radselect::radselect() :
	KXmlGuiWindow(),
	m_view(new radselectView(this))
{
    // accept dnd
    setAcceptDrops(true);
    setCentralWidget(m_view);  //This is the main widget
    setObjectName(QLatin1String("radselect"));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
    KStandardAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());
    statusBar()->show();

    // Apply the create the main window and ask the mainwindow to
    // automatically save settings if changed: window size, toolbar
    // position, icon size, etc.  Also to add actions for the statusbar
    // toolbar, and keybindings if necessary.
    setupGUI(Default,"radselectui.rc");

    // allow the view to change the statusbar
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));

	 if(!QDBusConnection::sessionBus().isConnected())
         {
		 kDebug() << "Session Bus not found!!";
                 dbusInterface =0;
         }
	 else
		dbusInterface = new QDBusInterface("org.kde.kiten", "/", "",
				QDBusConnection::sessionBus());

    // connect the search signal from the m_view with our dcop routines
    connect(m_view, SIGNAL(kanjiSelected(const QStringList&)),
            this,   SLOT(sendSearch(const QStringList&)));

}

radselect::~radselect()
{
        delete dbusInterface;
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

void radselect::saveProperties(KConfigGroup &config) { //For suspend
    if (!m_currentQuery.isEmpty())
        config.writePathEntry("searchString", m_currentQuery);
}

void radselect::readProperties(const KConfigGroup &config) { //For resume
	loadSearchString(config.readPathEntry("searchString", QString()));
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
	dialog->addPage(preferences, i18n("Settings"),"help-contents");
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
	if(kanji.size() == 0) return;

	//This may need to be done differently for handling collisions
	m_currentQuery = kanji.at(0);

	changeStatusbar(m_currentQuery);

	if(dbusInterface && dbusInterface->isValid()) {
		QDBusMessage reply = dbusInterface->call(
				QLatin1String("searchTextAndRaise"),
				m_currentQuery.toString());
		if(reply.type() == QDBusMessage::ErrorMessage) {
			kDebug() << "QDBus Error: " << reply.signature() <<"<eoe>";
		}
	}
}

#include "radselect.moc"
