#pragma once

#include <QDialog>
#include <QtAV/Statistics.h>

using namespace QtAV;

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class StatisticsView : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    void setStatistics(const Statistics &s);

protected:
    virtual void hideEvent(QHideEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void timerEvent(QTimerEvent *e);

signals:
    
public slots:
    
private:
    void initBaseItems(QList<QTreeWidgetItem*>* items);
    QTreeWidgetItem* createNodeWithItems(QTreeWidget* view, const QString& name, const QStringList& itemNames, QList<QTreeWidgetItem*>* items = 0);
    void setMetadataItem(QTreeWidgetItem* parent, const QHash<QString,QString>& metadata);
    QTreeWidget *mpView;
    QList<QTreeWidgetItem*> mBaseItems;
    QList<QTreeWidgetItem*> mVideoItems;
    //TODO: multiple streams
    QList<QTreeWidgetItem*> mAudioItems;
    Statistics mStatistics;
    int mTimer;

    QTreeWidgetItem *mpFPS, *mpAudioBitRate, *mpVideoBitRate;
    QTreeWidgetItem *mpMetadata, *mpAudioMetadata, *mpVideoMetadata;
};
