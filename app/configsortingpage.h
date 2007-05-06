/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright  (C) 2006 Joseph Kerian <jkerian@gmail.com>

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

#ifndef CONFIGSORTINGPAGE_H
#define CONFIGSORTINGPAGE_H

#include <QtGui/QWidget>

#include "ui_configsorting.h" //From the UI file

class KitenConfigSkeleton;
class QString;

class ConfigSortingPage : public QWidget, public Ui::configSorting
{
	Q_OBJECT
public:
	explicit ConfigSortingPage(QWidget *parent = 0, KitenConfigSkeleton *iconfig=NULL, Qt::WFlags f = 0);

public slots:	
	void updateWidgets();
	void updateWidgetsDefault();
	void updateSettings();
	bool hasChanged();
	bool isDefault();

signals:
	void widgetChanged();
private:
	KitenConfigSkeleton *m_config;
	QStringList m_dictNames;
	QStringList m_fields;
};

#endif
