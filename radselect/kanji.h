/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KANJI_H
#define KANJI_H

#include <QList>
#include <QSet>
#include <QString>

#include "radical.h"

class Kanji : public QString
{
  public:
                         Kanji();
    explicit             Kanji(  const QString &kanji
                               , const QSet<QString> &radicals );

    void                 addRadical( const QString &it );
    void                 setStrokes( unsigned int strokes );
    const QSet<QString>& getRadicals() const;
    unsigned int         strokes() const;

    bool operator<( const Kanji &other ) const;

  protected:
    unsigned int  strokeCount;
    QSet<QString> components;
};

#endif
