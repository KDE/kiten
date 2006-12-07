#ifndef _ENTRYLISTVIEW_H_
#define _ENTRYLISTVIEW_H_

#include <QTableView>
#include <QModelIndex>
#include <QAbstractTableModel>
#include "entry.h"

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
