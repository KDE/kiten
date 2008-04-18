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

#ifndef KITEN_ENTRYEDICT_H
#define KITEN_ENTRYEDICT_H

#include "Entry.h"
#include <QtCore/QHash>
#include <QtCore/QList>

class QString;
class EDICT_formatting;

class /* NO_EXPORT */ EntryEDICT : public Entry {
	public:
//		EntryEDICT(const EntryEDICT &x) : Entry(x) {} //No special members to copy in this one
		EntryEDICT(const QString &dict);
		EntryEDICT(const QString &dict, const QString &entry);
		Entry *clone() const;

		virtual QString toHTML() const;

		virtual bool loadEntry(const QString &);
		virtual QString dumpEntry() const;

		virtual QString HTMLWord() const;
		virtual QString Common() const;

	protected:
		virtual QString kanjiLinkify(const QString &) const;

	private:
		//Field of Application goes into the hash
		QList<QString> m_typeList;
		QList<QString> m_miscMarkings;

		/* This "private static class" provides various pieces of information
		 * about the EDICT format that we need to parse it properly. */
		static const EDICT_formatting &format();
		static EDICT_formatting *m_format;
	};

#endif
