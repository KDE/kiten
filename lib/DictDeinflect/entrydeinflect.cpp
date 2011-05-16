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

#include "entrydeinflect.h"

#include <KLocale>

#include <QString>

EntryDeinflect::EntryDeinflect(  const QString &correction
                               , const QString &type
                               , int index
                               , const QString &matchedEnding )
: Entry( QString( "Deinflect" ) )
, m_correction( correction )
, m_matchedEnding( matchedEnding )
, m_type( type )
, m_index( index )
{
}

EntryDeinflect::EntryDeinflect( const EntryDeinflect &old )
: Entry( old )
, m_correction( old.m_correction )
, m_type( old.m_type )
, m_index( old.m_index )
{
}

EntryDeinflect* EntryDeinflect::clone() const
{
  return new EntryDeinflect( *this );
}

QString EntryDeinflect::dumpEntry() const
{
  return QString();
}

bool EntryDeinflect::loadEntry( const QString &entry )
{
  return true;
}

bool EntryDeinflect::matchesQuery( const DictQuery &query ) const
{
  return query.toString().contains( m_matchedEnding );
}

QString EntryDeinflect::toHTML() const
{
      return QString( "<div class=\"Deinflect\">" )
              + i18n( "Possible de-conjugation %1 as %2", makeLink( m_correction ), m_type )
              + "</div>";
}
