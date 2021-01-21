/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADICALFILE_H
#define RADICALFILE_H

#include <QHash>
#include <QMultiMap>
#include <QSet>
#include <QString>

#include "kanji.h"
#include "radical.h"
#include "radicalbutton.h"

class RadicalFile
{
  public:
    explicit                RadicalFile( QString &radkfile, const QString &kanjidic = QString() );

    QSet<Kanji>             kanjiContainingRadicals( QSet<QString> &radicalList ) const;
    bool                    loadRadicalFile( QString &radkfile );
    bool                    loadKanjidic( const QString &kanjidic );
    QMultiMap<int,Radical> *mapRadicalsByStrokes( int max_strokes = 0 ) const;
    QSet<QString>           radicalsInKanji( QSet<Kanji> &kanjiList ) const;

  private:
    QHash<QString, Kanji>   m_kanji;
    QHash<QString, Radical> m_radicals;
};

#endif
