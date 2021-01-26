/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RADSELECTBUTTONGRID_H
#define RADSELECTBUTTONGRID_H

#include <QHash>
#include <QList>
#include <QSet>
#include <QString>
#include <QWidget>

#include "radicalbutton.h"
#include "radicalfile.h"

class ButtonGrid : public QWidget
{
  Q_OBJECT

  public:
             ButtonGrid( QWidget *parent, RadicalFile *radicalInfo );
    virtual ~ButtonGrid();

  signals:
    /**
     * Our generic message alert signal
     */
    void signalChangeStatusbar( const QString &text );
    /**
     * We emit this whenever something changed. This gives our list of
     * suggested kanji
     */
    void possibleKanji( const QList<Kanji> &kanjiList );
    void clearButtonSelections();

  public slots:
    /**
     * Triggered by a button press
     */
    void radicalClicked(   const QString &newrad
                         , RadicalButton::ButtonStatus newStatus );
    /**
     * Reset all buttons to the up and uncolored state
     */
    void clearSelections();

    void setSortByFrequency( bool enable );

  private:
    void buildRadicalButtons();
    void updateButtons();

    static const unsigned int number_of_radical_columns = 11;
    static const unsigned int maximumStrokeValue = 50;

    enum
    {
      Selection,
      Relational
    } CurrentMode;

    QString        m_relationalRadical;
    QSet<QString>  m_selectedRadicals;
    RadicalFile   *m_radicalInfo;
    bool           m_sortByFrequency;

    //Radical -> Button Mapping
    QHash<QString, RadicalButton*> m_buttons;
};

#endif
