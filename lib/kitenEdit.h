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

#ifndef KITENEDIT_H
#define KITENEDIT_H

#include "libkitenexport.h"
#include <kaction.h>
#include <kcombobox.h>

class KComboBox;
class KCompletion;

class LIBKITEN_EXPORT KitenEdit : public KHistoryCombo
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
	
	private:
		KComboBox *comboBox;
		KCompletion *completion;
};

#endif
