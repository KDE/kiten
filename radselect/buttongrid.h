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
    void setFont( const QFont &font );

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

    //Radical -> Button Mapping
    QHash<QString, RadicalButton*> m_buttons;
};

#endif
