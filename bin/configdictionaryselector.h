/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>
           (C) 2006 Joseph Kerian <jkerian@gmail.com>
			  (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef CONFIGDICTIONARYSELECTOR_H
#define CONFIGDICTIONARYSELECTOR_H

#include <qwidget.h> //For the WFlags on the constructor

#include "kitenconfig.h"

#include "ui_configdictselect.h" //From the UI file

class QTabWidget;
class KListView;
class QString;
class QWidget;

class ConfigDictionarySelector : public QWidget, public Ui::configDictSelect
{
	Q_OBJECT
public:
	ConfigDictionarySelector(const QString &dictionaryName, QWidget *parent = 0, const char* name = 0, Qt::WFlags f = 0);

public slots:	
	void updateWidgets();
	void updateWidgetsDefault();
	void updateSettings();
	bool hasChanged();
	bool isDefault();

	void addDictSLOT();
	void deleteDictSLOT();

signals:
	void widgetChanged();
private:
	QString dictName;
	KConfigSkeleton *config;
};

#endif
