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

#ifndef _RADSELECT_H_
#define _RADSELECT_H_

#include <kmainwindow.h>
class radselectView;

//From libkiten
#include <dictQuery.h>
class KitenEdit;

//QT and kdelibs
class KToggleAction;
class QDBusInterface;

/**
 * This class serves as the main window for radselect.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Joseph Kerian <jkerian@gmail.com>
 * @version 0.1
 */
class radselect : public KMainWindow
{
	Q_OBJECT
public:
	radselect();
	virtual ~radselect();

	/**
	 * This loads a string for a given query into the UI
	 */
	void loadSearchString(const QString &);

protected:
	// Overridden virtuals for Qt drag 'n drop (XDND)
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

protected:
	/**
	 * This function is called when it is time for the app to save its
	 * properties for session management purposes.
	 */
	void saveProperties(KConfigGroup &);

	/**
	 * This function is called when this app is restored.  The KConfig
	 * object points to the session management config file that was saved
	 * with @ref saveProperties
	 */
	void readProperties(const KConfigGroup &);


private slots:
	void optionsPreferences();
	void configureToolBars();

	void changeStatusbar(const QString& text);

	void showFullSearchString();
	void search();
	void clear();

	void sendSearch(const QStringList&);

private:
	void setupAccel();
	void setupActions();

private:
	QDBusInterface *dbusInterface;
	radselectView *m_view;
	dictQuery m_currentQuery;

	KitenEdit *Edit;
	KToggleAction *showAll;

};

#endif // _RADSELECT_H_

