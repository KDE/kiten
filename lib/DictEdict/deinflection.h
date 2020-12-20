/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_DEINFLECTION_H
#define KITEN_DEINFLECTION_H


#include "dictfile.h"

class DictQuery;
class EntryEdict;
class EntryList;
class QString;

class Deinflection
{
  public:
    explicit    Deinflection( const QString &name );

    QString    *getDeinflectionLabel();
    QString    *getWordType();
    EntryList  *search( const DictQuery &query, const QVector<QString> &preliminaryResults );
    bool        load();

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

    EntryEdict *makeEntry( const QString &entry );

    static QList<Conjugation> *conjugationList;

    QString                    m_deinflectionLabel;
    QString                    m_wordType;
    const QString              m_dictionaryName;
};

#endif
