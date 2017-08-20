#include "PlayListModel.h"
#include "PlayListItem.h"
#include <QVector>

PlayListModel::PlayListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QList<PlayListItem> PlayListModel::items() const
{
    return mItems;
}

Qt::ItemFlags PlayListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;;
    return  QAbstractListModel::flags(index) | Qt::ItemIsSelectable;
}

int PlayListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return mItems.size();
}

QVariant PlayListModel::data(const QModelIndex &index, int role) const
{
    QVariant v;
    if (index.row() < 0 || index.row() >= mItems.size())
        return v;

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        v.setValue(mItems.at(index.row()));

    return v;
}

bool PlayListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < mItems.size()
            && (role == Qt::EditRole || role == Qt::DisplayRole)) {
        // TODO: compare value?
        mItems.replace(index.row(), value.value<PlayListItem>());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        emit dataChanged(index, index, QVector<int>() << role);
#else
        emit dataChanged(index, index);
#endif
        return true;
    }
    return false;
}


bool PlayListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        mItems.insert(row, PlayListItem());
    endInsertRows();
    return true;
}


bool PlayListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        mItems.removeAt(row);
    endRemoveRows();
    return true;
}

void PlayListModel::updateLayout()
{
    emit layoutChanged();
}
