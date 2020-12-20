/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    void      focusInput();

  signals:
    void      search();

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
