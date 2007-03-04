/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
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

#ifndef _ENTRYLISTVIEW_H_
#define _ENTRYLISTVIEW_H_

#include <QTableView>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "Entry.h"
#include "EntryList.h"

class EntryListModel;

class EntryListView : public QTableView
{
	Q_OBJECT

	public:

		EntryListView(QWidget *parent = NULL);

	public slots:
		void setEmptyModel( void );

	protected:
		virtual void resizeEvent(QResizeEvent *event);

	private:

};

class EntryListModel : public QAbstractTableModel
{

	Q_OBJECT

	public:
		EntryListModel(const EntryList &list);

		void setEntryList( const EntryList &list );
		EntryList entryList() const { return list; }

		int rowCount(const QModelIndex & parent = QModelIndex()) const;
		int columnCount(const QModelIndex & parent = QModelIndex()) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		Qt::ItemFlags flags ( const QModelIndex & index ) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

	private:
		EntryList list;

};

#endif /* ifndef _ENTRYLISTVIEW_H_ */
