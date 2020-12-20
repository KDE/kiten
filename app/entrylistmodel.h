/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ENTRYLISTMODEL_H
#define ENTRYLISTMODEL_H

#include "entrylist.h"

#include <QAbstractTableModel>
#include <QModelIndex>

class EntryListModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    explicit      EntryListModel( const EntryList &list );

    int           columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant      data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    EntryList     entryList() const;
    Qt::ItemFlags flags( const QModelIndex &index ) const override;
    int           rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    bool          setData(  const QModelIndex &index
                          , const QVariant &value
                          , int role = Qt::EditRole ) override;
    void          setEntryList( const EntryList &list );

  private:
    EntryList _list;
};


#endif
