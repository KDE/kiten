/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <tuple>

#include "radical.h"

Radical::Radical()
: strokeCount( 0 )
{
}

Radical::Radical( const QString &irad, unsigned int strokes, unsigned int index )
: string( irad.at( 0 ) )
, strokeCount( strokes )
, idx( index )
{
}

QString Radical::toString() const
{
  return string;
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

bool Radical::compareIndices( const Radical &a, const Radical &b )
{
  return a.idx < b.idx;
}

bool Radical::compareFrequencies( const Radical &a, const Radical &b )
{
  // Negative frequency results in a descending order
  return std::make_tuple( - a.getKanji().size(), a.idx ) <
         std::make_tuple( - b.getKanji().size(), b.idx );
}
