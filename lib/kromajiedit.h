/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2006  Joseph Kerian <jkerian@gmail.com>

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

#ifndef KROMAJIEDIT_H
#define KROMAJIEDIT_H

#include <QtCore/QMap>

#include <klineedit.h>

class QKeyEvent;
class QWidget;
class QMenu;
class QByteArray;

class KRomajiEdit : public KLineEdit
{
	Q_OBJECT
	
	public:
	KRomajiEdit(QWidget *parent, const char *name);
	~KRomajiEdit();

	public slots:
	void setKana(QAction*);

	protected:
	void keyPressEvent(QKeyEvent *e);
	QMenu *createPopupMenu();
	
	private:
	QMap<QString, QString> hiragana;
	QMap<QString, QString> katakana;

	QByteArray kana;
};

#endif