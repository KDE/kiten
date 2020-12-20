/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_DICTFILEKANJIDIC_H
#define KITEN_DICTFILEKANJIDIC_H

#include "dictfile.h"

#include "kiten_export.h"

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
    EntryList    *doSearch( const DictQuery &query ) override;
    QStringList           dumpDictionary();
    QStringList   listDictDisplayOptions( QStringList list ) const override;
    bool                  loadDictionary(  const QString &file
                                         , const QString &name ) override;
    QStringList          *loadListType(  KConfigSkeletonItem *item
                                       , QStringList *list
                                       , const QMap<QString,QString> &long2short );
    void                  loadSettings();
    void          loadSettings( KConfigSkeleton *item ) override;
    bool                  validDictionaryFile( const QString &filename ) override;
    bool                  validQuery( const DictQuery &query ) override;

  protected:
    virtual inline Entry *makeEntry( const QString &entry );

    static QStringList *displayFields;

  private:
    QMap<QString,QString> loadDisplayOptions() const;

    QStringList m_kanjidic;
    bool        m_validKanjidic;
};

#endif
