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

#ifndef KITEN_DICTFILEEDICT_H
#define KITEN_DICTFILEEDICT_H

#include <config-kiten.h>

#include "dictfile.h"
#include "indexededictfile.h"
#include "libkitenexport.h"
#include "linearedictfile.h"

#include <QFile>
#include <QMap>

#include <sys/types.h>

class Deinflection;
class DictQuery;
class DictionaryPreferenceDialog;
class EntryEdict;
class KConfigSkeleton;
class KConfigSkeletonItem;
class QByteArray;
class QString;
class QStringList;

class KITEN_EXPORT DictFileEdict : public DictFile
{
  friend class EntryEdict;

  public:
                                        DictFileEdict();
    virtual                            ~DictFileEdict();

    virtual EntryList                  *doSearch( const DictQuery &query );
    virtual QStringList                 listDictDisplayOptions( QStringList x ) const;
    bool                                loadDictionary(  const QString &file
                                                       , const QString &name );
    virtual void                        loadSettings( KConfigSkeleton *config );
    virtual DictionaryPreferenceDialog *preferencesWidget(  KConfigSkeleton *config
                                                          , QWidget *parent = NULL );
    virtual bool                        validDictionaryFile( const QString &filename );
    bool                                validQuery( const DictQuery &query );

    static QString                     *deinflectionLabel;

  protected:
    virtual QMap<QString,QString> displayOptions() const;
    QStringList                  *loadListType(  KConfigSkeletonItem *item
                                               , QStringList *list
                                               , const QMap<QString,QString> &long2short );
    //This is a blatant abuse of protected methods to make the kanji subclass easy
    virtual Entry                *makeEntry( QString entry );

    LinearEdictFile     m_edictFile;

    static QStringList *displayFields;

  private:
    Deinflection *m_deinflection;
    bool          m_hasDeinflection;
};

#endif
