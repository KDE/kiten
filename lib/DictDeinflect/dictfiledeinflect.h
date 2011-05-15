/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
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

#ifndef KITEN_DICTFILEDEINFLECT_H
#define KITEN_DICTFILEDEINFLECT_H

#include <QStringList>

#include "../dictfile.h"

class DictQuery;
class QString;

class /* NO_EXPORT */ DictFileDeinflect: public DictFile
{
  public:
                DictFileDeinflect();

    EntryList  *doSearch( const DictQuery &query );
    QStringList listDictDisplayOptions( QStringList orig ) const;
    bool        loadDictionary( const QString &file, const QString &name) ;
    bool        validDictionaryFile( const QString &filename );
    bool        validQuery( const DictQuery &query );

  private:
    struct Conjugation
    {
      //The ending we are replacing
      QString ending;
      //The replacement (dictionary form) ending
      QString replace;
      //What this type of replacement is called
      QString label;
    };

    static QList<Conjugation> *conjugationList;
};

#endif
