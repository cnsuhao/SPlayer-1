#pragma once

#include <QWidget>
#include <QModelIndex>
#include "PlayListItem.h"

QT_BEGIN_NAMESPACE
class QListView;
class QToolButton;
QT_END_NAMESPACE
class PlayListDelegate;

class PlayListModel;

class PlayList : public QWidget
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList();

    void setSaveFile(const QString& file);
    QString saveFile() const;
    void load();
    void save();
    void deleteSaveFile();

    PlayListItem itemAt(int row);
    PlayListItem itemAt(const QString &url);
    void insertItemAt(const PlayListItem& item, int row = 0);
    void setItemAt(const PlayListItem& item, int row = 0);
    void remove(const QString& url);
    void insert(const QString& url, int row = 0);
    void setMaxRows(int r);
    int maxRows() const;
    int rowCount() const;

public slots:
    void cleanAndDelete();
    void removeAll();

signals:
    void aboutToPlay(const QString& url);

private slots:
    void removeSelectedItems();
    //
    void addItems();
    void clearItems();

    void onAboutToPlay(const QModelIndex& index);
    //void highlight(const QModelIndex& index);

private:
    QListView *mpListView;
    QToolButton *mpClear, *mpRemove, *mpAdd;
    PlayListDelegate *mpDelegate;
    PlayListModel *mpModel;
    int mMaxRows;
    QString mFile;
    bool mFirstShow;
};
