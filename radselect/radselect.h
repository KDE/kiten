/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADSELECT_H
#define RADSELECT_H

#include "dictquery.h"

#include <KXmlGuiWindow>

class QDBusInterface;
class RadSelectView;

/**
 * This class serves as the main window for radselect.
 * It handles the menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Joseph Kerian <jkerian@gmail.com>
 * @version 0.1
 */
class RadSelect : public KXmlGuiWindow
{
  Q_OBJECT

  public:
    RadSelect();
    virtual ~RadSelect();

   /**
    * This loads a string for a given query into the UI
    */
    void loadSearchString( const QString &searchString );

  protected:
    // Overridden virtuals for Qt drag 'n drop (XDND)
    void dragEnterEvent( QDragEnterEvent *event ) override;
    void dropEvent( QDropEvent *event ) override;

  protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties( KConfigGroup &config ) override;

    /**
     * This function is called when this app is restored. The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties( const KConfigGroup &config ) override;


  private slots:
    void optionsPreferences();

    void changeStatusbar( const QString &text );

    void sendSearch( const QStringList &kanji );

  private:
    QDBusInterface *m_dbusInterface;
    RadSelectView  *m_view;
    DictQuery       m_currentQuery;
};

#endif
