/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
