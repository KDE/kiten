/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>

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

#ifndef CONFIGDICTIONARIES_H
#define CONFIGDICTIONARIES_H

#include "kitenconfig.h"
#include "configdictionariesbase.h"

class ConfigDictionaries : public ConfigDictionariesBase
{
	Q_OBJECT
public:
	ConfigDictionaries(QWidget *parent = 0, const char* name = 0, WFlags f = 0);
	void readDictionaries();
        
	// KConfig stuff
	void updateWidgets();
	void updateWidgetsDefault();
	void updateSettings();
	bool hasChanged();
	bool isDefault();
	// KConfig stuff

signals:
	void widgetChanged();
public slots:
	void writeDictionaries();
private slots:
	void slotAddEdict();
	void slotAddKanjidic();
	void slotDelSelEdict();
	void slotDelSelKanjidic();
private:
	bool changed;
	Config* config;
	void readDictionaryList(const QString& group);
	void writeDictionaryList(const QString& group);
	void add(KListView* list);
	void delSel(KListView* list);
};

#endif
