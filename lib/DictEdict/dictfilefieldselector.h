/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

//This is currently the only file in libkiten dict handlers to have moc code included...

#ifndef KITEN_DICTFILEFIELDSELECTOR_H
#define KITEN_DICTFILEFIELDSELECTOR_H

#include "dictionarypreferencedialog.h"

#include <QWidget>

class KActionSelector;
class KConfigSkeleton;
class QString;
class QStringList;

//This is the default one used by edict and kanjidic
class DictFileFieldSelector : public DictionaryPreferenceDialog
{
  Q_OBJECT

  public:
             explicit DictFileFieldSelector( KConfigSkeleton *config
                                   , const QString &dictionaryTypeName
                                   , QWidget *parent );
    virtual ~DictFileFieldSelector();

  public slots:
    void     setAvailable( const QStringList &list );
    void     addAvailable( const QStringList &list );
    void     setDefaultList( const QStringList &list );
    void     readFromPrefs();
    void     writeToPrefs();

    void     updateWidgets() override;
    void     updateWidgetsDefault() override;
    void     updateSettings() override;
    void     settingChanged();

  signals:
    void     widgetChanged();

  private:
    QStringList      m_completeList;
    QStringList      m_defaultList;
    QString          m_dictName;
    KActionSelector *m_listView;
    KConfigSkeleton *m_config;
};

#endif
