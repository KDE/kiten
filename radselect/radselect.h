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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qpointer.h>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <kapplication.h>
#include <kmainwindow.h>

#include <kaction.h>
#include <klineedit.h>

#include "radselectview.h"
#include "dictQuery.h"

class KitenEdit;
class KToggleAction;

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
     * Use this method to load whatever file/URL you have
     */
//    void load(const char *searchString);
    void loadSearchString(QString searchString);

protected:
    // Overridden virtuals for Qt drag 'n drop (XDND)
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);


private slots:
    void optionsPreferences();
    void configureToolBars();

    void changeStatusbar(const QString& text);

    void showFullSearchString();
    void search();
    void clear();

    void sendSearch(const QStringList& radicals, const QString& strokes, const QString& grade);

private:
    void setupAccel();
    void setupActions();

private:
    radselectView *m_view;
    dictQuery currentQuery;

    KitenEdit *Edit;
    KToggleAction *showAll;

};

#endif // _RADSELECT_H_

