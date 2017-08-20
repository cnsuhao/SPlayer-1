#pragma once

#include <QStyledItemDelegate>

class PlayListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PlayListDelegate(QObject *parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex & index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    mutable int mHighlightRow;
    mutable QList<int> mSelectedRows;
};
