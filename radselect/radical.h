/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADICAL_H
#define RADICAL_H

#include <QSet>
#include <QString>

class Radical
{
  public:
                         Radical();
    explicit             Radical(  const QString &irad
                                 , unsigned int strokes = 0
                                 , unsigned int index = 0 );

    QString              toString() const;

    const QSet<QString>& getKanji() const;
    void                 addKanji( const QSet<QString> &newKanji );
    unsigned int         strokes() const;

    static bool          compareIndices( const Radical &a, const Radical &b );
    static bool          compareFrequencies( const Radical &a, const Radical &b );

  protected:
    QString       string;
    unsigned int  strokeCount;
    unsigned int  idx;
    QSet<QString> kanji;
    QSet<QString> components;
};

#endif
