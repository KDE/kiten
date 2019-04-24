/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
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

#ifndef CONFIGDICTIONARYSELECTOR_H
#define CONFIGDICTIONARYSELECTOR_H

#include "ui_configdictselect.h" //From the UI file

#include <QWidget> //For the WindowFlags on the constructor

class KConfigSkeleton;
class QString;
class QWidget;

class ConfigDictionarySelector : public QWidget, public Ui::configDictSelect
{
  Q_OBJECT

  public:
    explicit ConfigDictionarySelector(  const QString &dictionaryName
                                      , QWidget *parent = nullptr
                                      , KConfigSkeleton *iconfig = NULL
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
    KConfigSkeleton *_config;
};

#endif
