#include "entryListView.h"

EntryListView::EntryListView(QWidget *parent)
{

}

EntryList* EntryListView::entryList()
{
	EntryListModel *m = qobject_cast<EntryListModel*>(model());
	if (m) return m->entryList();
	return NULL;
}

void EntryListView::setEntryList( EntryList *list)
{
	QAbstractItemModel *oldModel = model();
	setModel(new EntryListModel(list));
	if (oldModel) delete oldModel;
}

EntryListModel::EntryListModel(EntryList *list)
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
		const QString& separator = (*list)[index.row()]->outputListDelimiter;
		switch (index.column())
		{
			case 0:
				(*list)[index.row()]->Word = value.toString();
				break;
			case 1:
				(*list)[index.row()]->Readings = value.toString().split(separator);
				break;
			case 2:
				(*list)[index.row()]->Meanings = value.toString().split(separator);
				break;

		}
	}
	return false;
}

void EntryListView::setEmptyModel( void )
{
	QAbstractItemModel *oldModel = model();
	if (oldModel) delete oldModel;
	setModel(NULL);
}

int EntryListModel::rowCount( const QModelIndex & parent ) const
{
	return list->size();
}

int EntryListModel::columnCount( const QModelIndex & parent ) const
{
	return 3;
}

QVariant EntryListModel::data ( const QModelIndex & index, int role ) const
{
	switch (role)
	{
		/* DisplayRole and EditRole have the same output */
		case Qt::DisplayRole:
		case Qt::EditRole:
			switch (index.column())
			{
				case 0:
					return list->at(index.row())->getWord();
					break;
				case 1:
					return list->at(index.row())->getReadings();
					break;
				case 2:
					return list->at(index.row())->getMeanings();
					break;
			}
			break;
	}
	return QVariant();
}

#include <entryListView.moc>
