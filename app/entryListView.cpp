/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
		(C) 2005 Paul Temple <paul.temple@gmx.net>
		(C) 2006 Joseph Kerian <jkerian@gmail.com>
		(C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "entryListView.h"
#include <QHeaderView>
#include <kdebug.h>

//These are the ratios for how much space the word and reading headers should
//take. The meaning will always stretch and take up the remaining space.
#define WORDRATIO 0.2
#define READINGRATIO 0.2

EntryListView::EntryListView(QWidget *)
{
	horizontalHeader()->setStretchLastSection(true);
}


void EntryListModel::setEntryList( const EntryList &list)
{
	this->list = list;
	emit layoutChanged();
}

void EntryListView::resizeEvent(QResizeEvent *event)
{
	QHeaderView *header = horizontalHeader();
	header->resizeSection(0, float(width()) * WORDRATIO);
	header->resizeSection(1, float(width()) * READINGRATIO);
}

EntryListModel::EntryListModel(const EntryList &list)
	: list(list)
{

}

Qt::ItemFlags EntryListModel::flags ( const QModelIndex & index ) const
{
	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool EntryListModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
	if (role == Qt::EditRole)
	{
		const QString& separator = list[index.row()]->outputListDelimiter;
		switch (index.column())
		{
			case 0:
				list[index.row()]->Word = value.toString();
				break;
			case 1:
				list[index.row()]->Readings = value.toString().split(separator);
				break;
			case 2:
				list[index.row()]->Meanings = value.toString().split(separator);
				break;

		}
	}
	return false;
}

void EntryListView::setEmptyModel( void )
{
	//TODO: implement me
}

int EntryListModel::rowCount( const QModelIndex & parent ) const
{
	return list.size();
}

int EntryListModel::columnCount( const QModelIndex & parent ) const
{
	return 3;
}

QVariant EntryListModel::data ( const QModelIndex & index, int role ) const
{
	//kDebug() << "Retrieving data at line" << index.row() << endl;
	switch (role)
	{
		/* DisplayRole and EditRole have the same output */
		case Qt::DisplayRole:
		case Qt::EditRole:
			switch (index.column())
			{
				case 0:
					return list.at(index.row())->getWord();
					break;
				case 1:
					return list.at(index.row())->getReadings();
					break;
				case 2:
					return list.at(index.row())->getMeanings();
					break;
			}
			break;
	}
	return QVariant();
}


#include <entryListView.moc>
