/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2006  Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

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

#ifndef KITEN_KITENEDIT_H
#define KITEN_KITENEDIT_H

#include "libkitenexport.h"
#include <QtCore/QSize>
#include <kaction.h>
#include <khistorycombobox.h>
#include <kactioncollection.h>
#define MINIMUMWIDTH 150

class KComboBox;
class KCompletion;

class KITEN_EXPORT KitenEdit : public KHistoryComboBox
{
	Q_OBJECT
	public:
		KitenEdit(KActionCollection *parent, QWidget *bar);
		~KitenEdit();
		
		/* access and mutate contents */
		KComboBox *ComboBox();
		KCompletion *Completion();
		QString text();
		void setText(const QString &text);
		virtual QSize minimumSizeHint() const { return QSize(MINIMUMWIDTH, 0); }
	
	private:
		KComboBox *comboBox;
		KCompletion *completion;
};

#undef MINIMUMWIDTH

#endif
