/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
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

#ifndef KITEN_DICTFILEKANJIDIC_H
#define KITEN_DICTFILEKANJIDIC_H

#include "dictfile.h"

#include "libkitenexport.h"

#include <QStringList>

class DictQuery;
class Entry;
class EntryList;
class KConfigSkeleton;
class KConfigSkeletonItem;

class KITEN_EXPORT DictFileKanjidic : public DictFile
{
  friend class EntryKanjidic;

  public:
                          DictFileKanjidic();
    virtual              ~DictFileKanjidic();

    QMap<QString,QString> displayOptions() const;
    virtual EntryList    *doSearch( const DictQuery &query );
    QStringList           dumpDictionary();
    virtual QStringList   listDictDisplayOptions( QStringList list ) const;
    bool                  loadDictionary(  const QString &file
                                         , const QString &name );
    QStringList          *loadListType(  KConfigSkeletonItem *item
                                       , QStringList *list
                                       , const QMap<QString,QString> &long2short );
    virtual void          loadSettings( KConfigSkeleton *item );
    bool                  validDictionaryFile( const QString &filename );
    bool                  validQuery( const DictQuery &query );

  protected:
    virtual inline Entry *makeEntry( QString entry );

    static QStringList *displayFields;

  private:
    QStringList m_kanjidic;
    bool        m_validKanjidic;
};

#endif
