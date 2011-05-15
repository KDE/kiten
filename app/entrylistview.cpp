/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
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

#include "entrylistview.h"

#include <KDebug>

#include <QHeaderView>

//These are the ratios for how much space the word and reading headers should
//take. The meaning will always stretch and take up the remaining space.
#define WORDRATIO    0.2
#define READINGRATIO 0.2

EntryListView::EntryListView( QWidget *parent )
{
  horizontalHeader()->setStretchLastSection( true );
}

void EntryListView::resizeEvent( QResizeEvent *event )
{
  QHeaderView *header = horizontalHeader();
  header->resizeSection( 0, float( width() ) * WORDRATIO    );
  header->resizeSection( 1, float( width() ) * READINGRATIO );
}

void EntryListView::setEmptyModel()
{
  //TODO: implement me
}

#include "entrylistview.moc"
