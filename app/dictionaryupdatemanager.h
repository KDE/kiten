/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#ifndef DICTIONARYUPDATEMANAGER_H
#define DICTIONARYUPDATEMANAGER_H

#include <QDate>
#include <QFile>
#include <QStringList>

class KAction;
class KJob;
class Kiten;
class KitenConfigSkeleton;

/**
 * This class handles all the update feature for the
 * EDICT and KANJIDIC dictionaries.
 *
 * @author Daniel E. Moctezuma <democtezuma@gmail.com>
 */
class DictionaryUpdateManager : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor.
     *
     * @param parent pointer to the Kiten instance. This helps us to add
     *               KAction actions to the program.
     */
    explicit DictionaryUpdateManager( Kiten *parent );

  signals:
    /**
     * Emitted when all the updates for EDICT and KANJIDIC finished.
     */
    void     updateFinished();

  private slots:
    /**
     * Downloads an information file containing:
     *   Name of dictionary
     *   Copyright information
     *   Latest creation date
     *   Number of entries in the dictionary
     */
    void     checkForUpdates();
    /**
     * Compare the downloaded information file with our
     * dictionaries and check whether or not we need to
     * update, if so, this function triggers their download.
     *
     * @param job the job that downloaded the information file.
     *            This slot should be connected to the result() signal from KJob.
     */
    void     checkInfoFile( KJob *job );
    /**
     * Install the downloaded dictionary.
     *
     * @param job the job that downloaded a dictionary file.
     *            This slot should be connected to the result() signal from KJob.
     */
    void     installDictionary( KJob *job );
    /**
     * Show the update results as a KMessageBox.
     */
    void     showUpdateResults();

  private:
    /**
     * Check whether or not the update finished, if so,
     * emit an updateFinished signal.
     */
    void     checkIfUpdateFinished();
    /**
     * Return the creation date of a file.
     * Files could be:
     *   Information file
     *   EDICT
     *   KANJIDIC
     *
     * @param file file from which you want to get the creation date
     * @return creation date of the given file
     */
    QDate    getFileDate( QFile &file );
    /**
     * Download a dictionary.
     * Could be:
     *   EDICT
     *   KANJIDIC
     *
     * @param url url to the dictionary you want to download
     */
    void     downloadDictionary( const QString &url );

    /**
     * We need it to add a KAction action to the main toolbar.
     */
    Kiten                 *_parent;
    /**
     * Config file that we need to know the path to
     * the installed dictionaries.
     */
    KitenConfigSkeleton   *_config;
    /**
     * Update action.
     */
    KAction               *_actionUpdate;
    /**
     * List of dictionaries already up to date.
     */
    QStringList            _succeeded;
    /**
     * List of dictionaries that failed to be updated.
     */
    QStringList            _failed;
    /**
     * Counter to know how many dictionaries we are trying to
     * install (to be used inside the installDictionary private slot).
     */
    int                    _counter;
};

#endif
