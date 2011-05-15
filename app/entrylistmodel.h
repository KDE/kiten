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

#ifndef ENTRYLISTMODEL_H
#define ENTRYLISTMODEL_H

#include "entrylist.h"

#include <QAbstractTableModel>
#include <QModelIndex>

class EntryListModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
                  EntryListModel( const EntryList &list );

    int           columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant      data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    EntryList     entryList() const;
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    int           rowCount( const QModelIndex &parent = QModelIndex() ) const;
    bool          setData(  const QModelIndex &index
                          , const QVariant &value
                          , int role = Qt::EditRole );
    void          setEntryList( const EntryList &list );

  private:
    EntryList _list;
};


#endif
