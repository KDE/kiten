/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "radical.h"

Radical::Radical()
: strokeCount( 0 )
{
}

Radical::Radical( const QString &irad, unsigned int strokes )
: QString( irad.at( 0 ) )
, strokeCount( strokes )
{
}

void Radical::addKanji( const QSet<QString> &newKanji )
{
  kanji += newKanji;
}

const QSet<QString>& Radical::getKanji() const
{
  return kanji;
}

unsigned int Radical::strokes() const
{
  return strokeCount;
}

bool Radical::operator<( const Radical &other ) const
{
  return this->strokeCount < other.strokeCount;
}
