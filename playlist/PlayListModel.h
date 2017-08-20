#pragma once

#include <QAbstractListModel>
#include "PlayListItem.h"

class PlayListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PlayListModel)
public:
    explicit PlayListModel(QObject *parent = 0);

    QList<PlayListItem> items() const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void updateLayout();
private:
    QList<PlayListItem> mItems;
};
