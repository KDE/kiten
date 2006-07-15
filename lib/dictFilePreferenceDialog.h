/***************************************************************************
 *   Copyright (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//This is currently the only file in libkiten dict handlers to have moc code included...

#ifndef DICTFILEDISPLAYPREFERENCES__H_
#define DICTFILEDISPLAYPREFERENCES__H_


class QStringList;
class QString;
#include <qwidget.h>

class DictionaryPreferenceDialog : public QWidget {
	Q_OBJECT
	public:
		DictionaryPreferenceDialog(QWidget *parent, const char *name) : QWidget(parent,name){}
		virtual ~DictionaryPreferenceDialog() {}

	public slots:
		virtual void updateWidgets() = 0;
		virtual void updateWidgetsDefault() = 0;
		virtual void updateSettings() = 0;
	signals:
		void widgetChanged();
};

class KConfigSkeleton;
class KActionSelector;
class Q3ListBox;
//This is the default one used by edict and kanjidic
class dictFileFieldSelector : public DictionaryPreferenceDialog {
	Q_OBJECT
	public:	
		dictFileFieldSelector(KConfigSkeleton*,const QString &dictionaryTypeName, QWidget *parent, const char *name);
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
