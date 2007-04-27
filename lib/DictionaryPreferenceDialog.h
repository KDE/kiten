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

#ifndef DICTIONARYPREFERENCEDIALOG__H_
#define DICTIONARYPREFERENCEDIALOG__H_

#include <QtGui/QWidget>
#include "libkitenexport.h"

class KITEN_EXPORT DictionaryPreferenceDialog : public QWidget {
	Q_OBJECT
	public:
		DictionaryPreferenceDialog(QWidget *parent, const QString &name);
		virtual ~DictionaryPreferenceDialog();
		const QString &name() const;
	public slots:
		virtual void updateWidgets() = 0;
		virtual void updateWidgetsDefault() = 0;
		virtual void updateSettings() = 0;
	signals:
		void widgetChanged();

	private:
		QString m_name;
};

#endif
