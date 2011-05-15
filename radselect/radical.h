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

#ifndef RADICAL_H
#define RADICAL_H

#include <QSet>
#include <QString>

class Radical : public QString
{
  public:
                         Radical();
    explicit             Radical(  const QString &irad
                                 , unsigned int strokes = 0 );

    const QSet<QString>& getKanji() const;
    void                 addKanji( const QSet<QString> &newKanji );
    unsigned int         strokes() const;

    bool                 operator<( const Radical &other ) const;

  protected:
    unsigned int  strokeCount;
    QSet<QString> kanji;
    QSet<QString> components;
};

#endif
