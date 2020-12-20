/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_DICTFILEEDICT_H
#define KITEN_DICTFILEEDICT_H

#include <config-kiten.h>

#include "dictfile.h"
#include "indexededictfile.h"
#include "kiten_export.h"
#include "linearedictfile.h"

#include <QMap>

#include <sys/types.h>

class Deinflection;
class DictQuery;
class DictionaryPreferenceDialog;
class EntryEdict;
class KConfigSkeleton;
class KConfigSkeletonItem;
class QString;
class QStringList;

class KITEN_EXPORT DictFileEdict : public DictFile
{
  friend class EntryEdict;

  public:
                                        DictFileEdict();
    virtual                            ~DictFileEdict();

    EntryList                  *doSearch( const DictQuery &query ) override;
    QStringList                 listDictDisplayOptions( QStringList x ) const override;
    bool                                loadDictionary(  const QString &file
                                                       , const QString &name ) override;
    void                                loadSettings();
    void                        loadSettings( KConfigSkeleton *config ) override;
    DictionaryPreferenceDialog *preferencesWidget(  KConfigSkeleton *config
                                                          , QWidget *parent = nullptr ) override;
    bool                        validDictionaryFile( const QString &filename ) override;
    bool                                validQuery( const DictQuery &query ) override;

    static QString                     *deinflectionLabel;
    static QString                     *wordType;

  protected:
    virtual QMap<QString,QString> displayOptions() const;
    QStringList                  *loadListType(  KConfigSkeletonItem *item
                                               , QStringList *list
                                               , const QMap<QString,QString> &long2short );
    //This is a blatant abuse of protected methods to make the kanji subclass easy
    virtual Entry                *makeEntry( const QString &entry );

    LinearEdictFile     m_edictFile;

    static QStringList *displayFields;

  private:
    QMap<QString,QString> loadDisplayOptions() const;

    Deinflection *m_deinflection;
    bool          m_hasDeinflection;
};

#endif
