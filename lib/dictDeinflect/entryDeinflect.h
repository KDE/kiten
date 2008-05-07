/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2006  Joseph Kerian <jkerian@gmail.com>
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

#ifndef KITEN_ENTRYDEINFLECT_H
#define KITEN_ENTRYDEINFLECT_H

#include "Entry.h"
#include <QtCore/QString>
#include "kdebug.h"

class /* NO_EXPORT */ entryDeinflect : public Entry {

	public:
		entryDeinflect(const QString &correction, const QString &type, int index, const QString &dictName)
			: Entry(dictName),m_correction(correction),m_type(type),m_index(index) {}
		entryDeinflect(const entryDeinflect &old) : Entry(old), m_correction(old.m_correction),
			m_type(old.m_type), m_index(old.m_index) {}
		entryDeinflect *clone() const { return new entryDeinflect(*this); }

		virtual QString toHTML() const {
			return QString("<div class=\"Deinflect\">Possible de-conjugation ") +
				makeLink(m_correction) + QString(" as ") + m_type;
		}

		virtual bool loadEntry(const QString &) {return true;}
		virtual QString dumpEntry() const {return QString();}
		virtual bool matchesQuery(const DictQuery&) const {return true;}

	private:
		QString m_correction;
		QString m_type;
		int m_index;
};

#endif
