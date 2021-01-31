/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "entrylistview.h"

#include <QHeaderView>

//These are the ratios for how much space the word and reading headers should
//take. The meaning will always stretch and take up the remaining space.
#define WORDRATIO    0.2
#define READINGRATIO 0.2

EntryListView::EntryListView( QWidget *parent )
  : QTableView( parent )
{
  horizontalHeader()->setStretchLastSection( true );
}

void EntryListView::resizeEvent( QResizeEvent *event )
{
  Q_UNUSED( event );
  QHeaderView *header = horizontalHeader();
  header->resizeSection( 0, float( width() ) * WORDRATIO    );
  header->resizeSection( 1, float( width() ) * READINGRATIO );
}

void EntryListView::setEmptyModel()
{
  //TODO: implement me
}


