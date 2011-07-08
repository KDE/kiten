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

class DictionaryUpdateManager : public QObject
{
  Q_OBJECT

  public:
    explicit DictionaryUpdateManager( Kiten *parent );

  signals:
    void     updateFinished();

  private slots:
    void     checkForUpdates();
    void     checkInfoFile( KJob *job );
    void     installDictionary( KJob *job );
    void     showUpdateResults();

  private:
    void     checkIfUpdateFinished();
    QDate    getFileDate( QFile &file );
    void     downloadDictionary( const QString &url );

    Kiten                 *_parent;
    KitenConfigSkeleton   *_config;
    KAction               *_actionUpdate;
    QStringList            _succeeded;
    QStringList            _failed;
    int                    _counter;
};

#endif
