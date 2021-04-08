/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CONFIGDICTIONARYSELECTOR_H
#define CONFIGDICTIONARYSELECTOR_H

#include "ui_configdictselect.h" //From the UI file

#include <QWidget> //For the WindowFlags on the constructor

class KConfigSkeleton;
class QString;

class ConfigDictionarySelector : public QWidget, public Ui::configDictSelect
{
  Q_OBJECT

  public:
    explicit ConfigDictionarySelector(  const QString &dictionaryName
                                      , QWidget *parent = nullptr
                                      , KConfigSkeleton *iconfig = nullptr
                                      , Qt::WindowFlags f = {} );

  public slots:
    void addDictSlot();
    void deleteDictSlot();
    bool hasChanged();
    bool isDefault();
    void updateSettings();
    void updateWidgets();
    void updateWidgetsDefault();

  signals:
    void widgetChanged();

  private:
    QString          _dictName;
    KConfigSkeleton *_config = nullptr;
};

#endif
