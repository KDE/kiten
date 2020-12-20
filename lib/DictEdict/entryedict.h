/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_ENTRYEDICT_H
#define KITEN_ENTRYEDICT_H

#include "entry.h"

#include "kiten_export.h"

#include <QHash>
#include <QList>
#include <QSet>

#ifndef KITEN_EDICTFORMATTING
#define KITEN_EDICTFORMATTING

namespace EdictFormatting
{
  extern QStringList Nouns;
  extern QStringList Verbs;
  extern QStringList Adjectives;
  extern QStringList Adverbs;
  extern QStringList IchidanVerbs;
  extern QStringList GodanVerbs;
  extern QStringList FukisokuVerbs;
  extern QStringList Expressions;
  extern QStringList Prefix;
  extern QStringList Suffix;
  extern QString     Particle;
  
  extern QSet<QString> PartsOfSpeech;
  extern QSet<QString> MiscMarkings;
  extern QSet<QString> FieldOfApplication;
}

#endif

class KITEN_EXPORT EntryEdict : public Entry
{
  friend class DictFileEdict;

  public:
//     EntryEdict( const EntryEdict &x ) : Entry( x ) {} //No special members to copy in this one
                    EntryEdict( const QString &dict );
                    EntryEdict( const QString &dict, const QString &entry );

    Entry          *clone() const override;
    /**
     * Simple accessor.
     */
    QString         getTypes() const;
    /**
     * Simple accessor.
     */
    QStringList     getTypesList() const;

    bool            isAdjective() const;
    bool            isAdverb() const;
    bool            isCommon() const;
    bool            isExpression() const;
    bool            isFukisokuVerb() const;
    bool            isGodanVerb() const;
    bool            isIchidanVerb() const;
    bool            isNoun() const;
    bool            isParticle() const;
    bool            isPrefix() const;
    bool            isSuffix() const;
    bool            isVerb() const;

    bool            matchesWordType( const DictQuery &query ) const;

    QString dumpEntry() const override;
    QString getDictionaryType() const override;
    QString HTMLWord() const override;
    bool    loadEntry( const QString &entryLine ) override;
    QString toHTML() const override;

  protected:
    virtual QString kanjiLinkify( const QString &inString ) const;

  private:
    /**
     * Types that match this entry (i.e. n, adj, adv).
     */
    QStringList    m_types;

    //Field of Application goes into the hash
    QList<QString> m_miscMarkings;
};

#endif
