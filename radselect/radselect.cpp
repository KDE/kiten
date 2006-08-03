/* This file is part of kiten, a KDE Japanese Reference Tool
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "radselect.h"
#include "radselectpref.h"
#include "kromajiedit.h"
#include "kitenEdit.h"

#include <qpainter.h>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kconfig.h>

#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>
#include <ktoggleaction.h>

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

    // connect the search signal from the m_view with our dcop routines
//    connect(m_view, SIGNAL(searchTrigger(const QStringList&, const QString&, const QString&)),
//            this,   SLOT(sendSearch(const QStringList&, const QString&, const QString&)));

}

radselect::~radselect()
{
}
void radselect::loadSearchString(QString searchString) {
	currentQuery = searchString;
	changeStatusbar(searchString);
	m_view->load(currentQuery.getProperty("R"),
		currentQuery.getProperty("G"), currentQuery.getProperty("S"));
}

void radselect::setupActions()
{
	
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    KStdAction::keyBindings((const QObject*)guiFactory(), SLOT(configureShortcuts()), actionCollection());

    Edit = new KitenEdit(actionCollection(), this);
	 
	 KAction *kac =  new KAction(KIcon(BarIcon("locationbar_erase",16)), 
			 i18n("&Clear Search Bar"), actionCollection(),"clear_search");
	 connect( kac, SIGNAL(triggered()), Edit, SLOT(clear()) );
	 kac->setShortcut(QString("Qt::CTRL;Qt::Key_N"));

	 kac=new KAction(KIcon("key_enter"),i18n("S&earch"),actionCollection(),"search");
	 connect( kac, SIGNAL(triggered()), this, SLOT(search()) );

    showAll = new KToggleAction(KIcon("full_string"),
			 i18n("&Show Full Search String"),actionCollection(), "show_full_search");
	 showAll->setShortcut(QString("Qt::CTRL;Qt::Key_S"));
	 connect( showAll, SIGNAL(triggered()), this, SLOT(showFullSearchString()) );

    (void) KStdAction::configureToolbars(this, SLOT(configureToolBars()),
													  actionCollection());

    statusBar()->show();

    // Apply the create the main window and ask the mainwindow to
    // automatically save settings if changed: window size, toolbar
    // position, icon size, etc.  Also to add actions for the statusbar
    // toolbar, and keybindings if necessary.
    setupGUI();
	 
}

void radselect::saveProperties(KConfig *config) { //For suspend

    if (!currentQuery.isEmpty())
        config->writePathEntry("searchString", currentQuery);
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
    // popup some sort of preference dialog, here
    radselectPreferences dlg;
    if (dlg.exec())
    {
        // redo your settings
    }
}

void radselect::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->showMessage(text);
}

//This one is triggered if the search button is used (or the widget interface
//is in some other way given priority.
void radselect::sendSearch(const QStringList& radicals, const QString& strokes, const QString& grade){
	dictQuery editBox (Edit->currentText());

	dictQuery dialogParts;
	if(radicals.count() > 0)
		dialogParts.setProperty("R",radicals.join(""));
	else
		editBox.removeProperty("R");
	if(strokes.length() > 0) 
		dialogParts.setProperty("S",strokes);
	else
		editBox.removeProperty("S");
	if(grade.length() > 0)
		dialogParts.setProperty("G",grade);
	else
		editBox.removeProperty("G");

	currentQuery = (dialogParts + editBox) + dialogParts;
		//A bit odd... but this is for proper ordering

	changeStatusbar(currentQuery);
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
