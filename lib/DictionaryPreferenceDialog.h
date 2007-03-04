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

#ifndef DICTIONARYPREFERENCEDIALOG__H_
#define DICTIONARYPREFERENCEDIALOG__H_

#include <qwidget.h>

class DictionaryPreferenceDialog : public QWidget {
	Q_OBJECT
	public:
		DictionaryPreferenceDialog(QWidget *parent, const char *name);
		virtual ~DictionaryPreferenceDialog();

	public slots:
		virtual void updateWidgets() = 0;
		virtual void updateWidgetsDefault() = 0;
		virtual void updateSettings() = 0;
	signals:
		void widgetChanged();
};

#endif
