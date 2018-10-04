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

#ifndef RADSELECTVIEW_H
#define RADSELECTVIEW_H

//generated from the uic from radical_selector.ui (defines Ui::radical_selector)
#include "ui_radical_selector.h"

#include <QList>
#include <QWidget>

class ButtonGrid;
class QListWidgetItem;
class QString;
class QStringList;

#include "radicalfile.h"	//For Kanji/radicalFile definitions

class RadSelectView : public QWidget, public Ui::radical_selector
{
  Q_OBJECT

  public:
    explicit RadSelectView( QWidget *parent );
    virtual ~RadSelectView();

    //Load pre-determined search parameters
    void loadRadicals( const QString &radicals, int strokeMin, int strokeMax );
    void loadKanji( QString &kanji );

  signals:
    void signalChangeStatusbar( const QString &text );
    //Listen for this if you want to detect each minor change
    void searchModified();
    //This is when they've actually pressed a kanji
    void kanjiSelected( const QStringList &kanjiList );

  private slots:
    void loadSettings();
    //Handles either of the stroke limiters moving
    void strokeLimitChanged( int newvalue );
    //Connected to any changes in searches, emits searchModified
    void changedSearch();
    //Clear everything
    void clearSearch();
    //Result is clicked
    void kanjiClicked( QListWidgetItem *item );
    //Result is double-clicked
    void kanjiDoubleClicked( QListWidgetItem *item );
    //Sets the list of visible Kanji
    void listPossibleKanji( const QList<Kanji> &list );
    //Copy text from lineedit to clipboard
    void toClipboard();

  private:
    RadicalFile  *m_radicalInfo;
    ButtonGrid   *m_buttongrid;
    QList<Kanji>  m_possibleKanji;
};

#endif
