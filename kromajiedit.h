/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KROMAJIEDIT_H
#define KROMAJIEDIT_H

#include <klineedit.h>
#include <qcstring.h>

class QKeyEvent;
class QWidget;
class QPopupMenu;

class KRomajiEdit : public KLineEdit
{
	Q_OBJECT
	
	public:
	KRomajiEdit(QWidget *parent, const char *name);
	~KRomajiEdit();

	public slots:
	void setKana(int);

	protected:
	void keyPressEvent(QKeyEvent *e);
	QPopupMenu *createPopupMenu();
	
	private:
	QMap<QString, QString> hiragana;
	QMap<QString, QString> katakana;

	QCString kana;
};

#endif
