/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
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

#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

#include <QAction>
#include <QTextBrowser>

#include "entry.h"

class KActionCollection;
class KActionMenu;

class ResultsView : public QTextBrowser
{
  Q_OBJECT

  public:
    explicit     ResultsView( QWidget *parent = nullptr, const char *name = nullptr );

    void         addResult( Entry *result, bool common = false );
    void         addKanjiResult( Entry*, bool common = false );
    void         setLaterScrollValue( int scrollValue );

  public slots:
    void         append( const QString &text );
    void         clear();
    void         flush();
    void         print( const QString &title );
    void         setBasicMode( bool yes );
    void         setContents( const QString &text );

  signals:
    void         entrySpecifiedForExport( int index );
    void         urlClicked( const QString& );

  protected:
    QString      generateCSS();

  private slots:
    void doScroll();

  private:
    QAction           *_addToExportListAction = nullptr;
    bool               _basicMode;
    KActionCollection *_popupActions = nullptr;
    KActionMenu       *_popupMenu = nullptr;
    QString            _printText;
    int                _scrollValue;
};

#endif
