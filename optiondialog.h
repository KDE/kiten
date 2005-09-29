/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>

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

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

class KListView;
class QString;
class KFontChooser;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class DictList;

#include <kconfigdialog.h>
#include <kglobalaccel.h>
#include <kkeydialog.h>

#include "kitenconfig.h"
#include "configdictionaries.h"

class ConfigureDialog : public KConfigDialog
{
	Q_OBJECT
public:
	ConfigureDialog(QWidget *parent=0, const char *name=0);
	~ConfigureDialog();
signals:
	void settingsUpdated();

private slots:
	void updateWidgets();
	void updateWidgetsDefault();
	void updateSettings();

private:
	bool hasChanged();
	bool isDefault();
	ConfigDictionaries* configDic;
};

#endif
