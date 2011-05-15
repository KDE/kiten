/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#ifndef KITEN_ENTRYDEINFLECT_H
#define KITEN_ENTRYDEINFLECT_H

#include "../entry.h"

class /* NO_EXPORT */ EntryDeinflect : public Entry
{
  public:
                    EntryDeinflect (  const QString &correction
                                    , const QString &type
                                    , int index
                                    , const QString &matchedEnding );
                    EntryDeinflect( const EntryDeinflect &old );

    EntryDeinflect *clone() const;
    virtual QString dumpEntry() const;
    virtual bool    loadEntry( const QString &entry );
    virtual bool    matchesQuery( const DictQuery &query ) const;
    virtual QString toHTML() const;

  private:
    QString m_correction;
    QString m_matchedEnding;
    QString m_type;
    int     m_index;
};

#endif
