/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "entrylistmodel.h"

#include "entry.h"

EntryListModel::EntryListModel( const EntryList &list )
: _list( list )
{

}

EntryList EntryListModel::entryList() const
{
  return _list;
}

Qt::ItemFlags EntryListModel::flags ( const QModelIndex &index ) const
{
  return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
}

bool EntryListModel::setData ( const QModelIndex &index, const QVariant &value, int role )
{
  if ( role == Qt::EditRole )
  {
    const QString &separator = _list[ index.row() ]->outputListDelimiter;

    switch ( index.column() )
    {
      case 0:
        _list[ index.row() ]->Word = value.toString();
        break;
      case 1:
        _list[ index.row() ]->Readings = value.toString().split( separator );
        break;
      case 2:
        _list[ index.row() ]->Meanings = value.toString().split( separator );
        break;
    }
  }

  return false;
}

void EntryListModel::setEntryList( const EntryList &list )
{
  _list = list;
  emit layoutChanged();
}

int EntryListModel::rowCount( const QModelIndex & parent ) const
{
  return parent.isValid() ? 0 : _list.size();
}

int EntryListModel::columnCount( const QModelIndex & parent ) const
{
  return parent.isValid() ? 0 : 3;
}

QVariant EntryListModel::data ( const QModelIndex & index, int role ) const
{
  //kDebug() << "Retrieving data at line" << index.row();
  switch ( role )
  {
    /* DisplayRole and EditRole have the same output */
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
      switch ( index.column() )
      {
        case 0:
          return _list.at( index.row() )->getWord();
          break;
        case 1:
          return _list.at( index.row() )->getReadings();
          break;
        case 2:
          return _list.at( index.row() )->getMeanings();
          break;
      }

      break;
    }
  }

  return QVariant();
}


