/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_ENTRYKANJIDIC_H
#define KITEN_ENTRYKANJIDIC_H

#include "entry.h"

#include "kiten_export.h"

#include <QStringList>

class QString;

class KITEN_EXPORT EntryKanjidic : public Entry
{
  public:
                    EntryKanjidic( const EntryKanjidic &dict );
    explicit        EntryKanjidic( const QString &dict );
                    EntryKanjidic( const QString &dict, const QString &entry );

    Entry          *clone() const override;
    QString dumpEntry() const override;
    QString         getAsRadicalReadings() const;
    QStringList     getAsRadicalReadingsList() const;
    QString getDictionaryType() const override;
    QString         getInNamesReadings() const;
    QStringList     getInNamesReadingsList() const;
    QString         getKanjiGrade() const;
    QString         getKunyomiReadings() const;
    QStringList     getKunyomiReadingsList() const;
    QString         getOnyomiReadings() const;
    QStringList     getOnyomiReadingsList() const;
    QString         getStrokesCount() const;
    bool    loadEntry( const QString &entryLine ) override;
    QString toHTML() const override;

  protected:
    bool    extendedItemCheck( const QString &key, const QString &value ) const override;
    virtual QString HTMLExtendedInfo( const QString &field ) const;
    QString HTMLReadings() const override;
    QString HTMLWord() const override;
    virtual QString makeReadingLink( const QString &inReading ) const;

    QStringList AsRadicalReadings;
    QStringList InNamesReadings;
    QStringList KunyomiReadings;
    QStringList OnyomiReadings;
    QStringList originalReadings;

  private:
    QString addReadings( const QStringList &list ) const;
};

#endif
