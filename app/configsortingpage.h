/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CONFIGSORTINGPAGE_H
#define CONFIGSORTINGPAGE_H

#include "ui_configsorting.h" //From the UI file

#include <QWidget>

class KitenConfigSkeleton;
class QString;

class ConfigSortingPage : public QWidget, public Ui::configSorting
{
  Q_OBJECT

  public:
    explicit ConfigSortingPage(  QWidget *parent = nullptr
                               , KitenConfigSkeleton *config = nullptr
                               , Qt::WindowFlags f = {} );

  public slots:
    bool hasChanged();
    bool isDefault();
    void updateSettings();
    void updateWidgets();
    void updateWidgetsDefault();

  signals:
    void widgetChanged();

  private:
    KitenConfigSkeleton *_config;
    QStringList          _dictNames;
    QStringList          _fields;
};

#endif
