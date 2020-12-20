/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
