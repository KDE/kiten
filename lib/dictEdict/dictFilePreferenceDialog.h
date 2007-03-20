/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

//This is currently the only file in libkiten dict handlers to have moc code included...

#ifndef DICTFILEFIELDSELECTOR_H_
#define DICTFILEFIELDSELECTOR_H_


class QStringList;
class QString;
#include <qwidget.h>
#include "DictionaryPreferenceDialog.h"

class KConfigSkeleton;
class KActionSelector;
//This is the default one used by edict and kanjidic
class dictFileFieldSelector : public DictionaryPreferenceDialog {
	Q_OBJECT
	public:
		dictFileFieldSelector(KConfigSkeleton*,const QString &dictionaryTypeName, QWidget *parent);
		virtual ~dictFileFieldSelector();

	public slots:
		void setAvailable(const QStringList &list);
		void addAvailable(const QStringList &list);
		void setDefaultList(const QStringList &list);
		void setDefaultFull(const QStringList &list);
		void readFromPrefs();
		void writeToPrefs();

		void updateWidgets();
		void updateWidgetsDefault();
		void updateSettings();
		void settingChanged();
	signals:
		void widgetChanged();

	private:
		QStringList fetchItemFromPreferences(const QString &dictName, const QString &type);
		QStringList extractList(KActionSelector*);

		QStringList completeList, defaultList,defaultFull;
		QString dictName;
		KActionSelector *ListView, *FullView;
		KConfigSkeleton *config;

};
#endif
