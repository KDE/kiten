/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2005 Paul Temple <paul.temple@gmx.net>
			  (C) 2006 Joseph Kerian <jkerian@gmail.com>

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

#include <kconfigdialog.h>

class QWidget;
class KitenConfigSkeleton;

class ConfigureDialog : public KConfigDialog
{
	Q_OBJECT
public:
	ConfigureDialog(QWidget *parent=0, KitenConfigSkeleton *config=NULL);
	virtual ~ConfigureDialog();
signals:
	void SIG_updateWidgets();
	void SIG_updateWidgetsDefault();
	void SIG_updateSettings();
	void settingsChanged();

private slots:
	void updateWidgets();
	void updateWidgetsDefault();
	void updateSettings();
	void updateButtons();

private:
	QWidget *makeDictionaryPreferencesPage(QWidget *, KitenConfigSkeleton*);
	QWidget *makeDictionaryFileSelectionPage(QWidget *, KitenConfigSkeleton*);
	bool hasChangedMarker;
	bool hasChanged();
	bool isDefault();
};

#endif
