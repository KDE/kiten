/***************************************************************************
 *  Copyright  (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
 *             (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com>          *
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

#ifndef ENTRYEDICT_H_
#define ENTRYEDICT_H_

#include "entry.h"

class QString;
class dictQuery;

class KDE_EXPORT EntryEDICT : public Entry {
	public:
		EntryEDICT() : Entry() {}
		EntryEDICT(const EntryEDICT &x) : Entry(x) {} //No special members to copy in this one
		EntryEDICT(const QString &dict) : Entry(dict) {}
		EntryEDICT(const QString &dict, const QString &entry) : Entry(dict) {loadEntry(entry);}
		Entry *clone() const { return new EntryEDICT(*this); }
		

		virtual QString toBriefHTML() const;
		virtual QString toVerboseHTML() const;
		
		virtual bool loadEntry(const QString &);
		virtual QString dumpEntry() const;

		virtual inline QString HTMLWord() const;
	protected:
		virtual inline QString kanjiLinkify(const QString &) const;
};

#endif
