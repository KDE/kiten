/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#ifndef SEARCHSTRINGINPUT_H
#define SEARCHSTRINGINPUT_H

#include <QObject>

#include "dictquery.h"

class QAction;
class KHistoryComboBox;
class KSelectAction;
class Kiten;
class WordType;

class SearchStringInput : public QObject
{
  Q_OBJECT

  public:
    explicit  SearchStringInput( Kiten *parent );

    DictQuery getSearchQuery() const;
    void      setDefaultsFromConfig();
    void      setSearchQuery( const DictQuery &query );
    void      updateFontFromConfig();

  public slots:
    void      test();

  signals:
    void      search();

  private slots:
    void      focusInput();

  private:
    KSelectAction    *_actionFilterRare = nullptr;
    // Search exact/anywhere/beginning
    KSelectAction    *_actionSearchSection = nullptr;
    KSelectAction    *_actionSelectWordType = nullptr;
    KHistoryComboBox *_actionTextInput = nullptr;
    QAction          *_actionFocusInput = nullptr;
    Kiten            *_parent = nullptr;
};

#endif
