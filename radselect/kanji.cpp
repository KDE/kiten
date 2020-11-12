/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#include "kanji.h"

Kanji::Kanji()
: strokeCount( 0 )
{
}

Kanji::Kanji( const QString &kanji, const QSet<QString> &radicals )
: QString( kanji.at( 0 ) )
, strokeCount( 0 )
{
  components = radicals;
}

void Kanji::addRadical( const QString &it )
{
  components += it;
}

void Kanji::setStrokes( unsigned int strokes )
{
  strokeCount = strokes;
}

const QSet<QString>& Kanji::getRadicals() const
{
  return components;
}

unsigned int Kanji::strokes() const
{
  return strokeCount;
}

bool Kanji::operator<( const Kanji &other ) const
{
  return this->strokeCount < other.strokeCount;
}
