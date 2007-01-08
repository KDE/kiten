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

#ifndef ENTRYDEINFLECT_H_
#define ENTRYDEINFLECT_H_

#include "entry.h"

class QString;
class dictQuery;

class /* NO_EXPORT */ entryDeinflect : public Entry {

	public:
		entryDeinflect(const QString &original, const QString &correction);
		entryDeinflect *clone() const { return new entryDeinflect(*this); }

		virtual QString toHTML(printType=printAuto) const;

		virtual bool loadEntry(const QString &);
		virtual QString dumpEntry() const;

		virtual inline QString HTMLWord() const;
		virtual inline QString Common() const;
	protected:
		virtual inline QString kanjiLinkify(const QString &) const;

		bool common;
};

#endif
