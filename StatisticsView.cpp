#include "StatisticsView.h"
#include <QTimerEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QObject>

QStringList getBaseInfoKeys() {
    return QStringList()
            << QObject::tr("Url")
            << QObject::tr("Format")
            << QObject::tr("Bit rate")
            << QObject::tr("Start time")
            << QObject::tr("Duration")
               ;
}

QStringList getCommonInfoKeys() {
    return QStringList()
            << QObject::tr("Available")
            << QObject::tr("Codec")
            << QObject::tr("Decoder")
            << QObject::tr("Decoder detail")
            << QObject::tr("Total time")
            << QObject::tr("Start time")
            << QObject::tr("Bit rate")
            << QObject::tr("Frames")
            << QObject::tr("FPS") // avg_frame_rate. guessed by FFmpeg
               ;
}

QStringList getVideoInfoKeys() {
    return getCommonInfoKeys()
            << QObject::tr("FPS Now") //current display fps
            << QObject::tr("Pixel format")
            << QObject::tr("Size") //w x h
            << QObject::tr("Coded size") // w x h
            << QObject::tr("GOP size")
               ;
}
QStringList getAudioInfoKeys() {
    return getCommonInfoKeys()
            << QObject::tr("Sample format")
            << QObject::tr("Sample rate")
            << QObject::tr("Channels")
            << QObject::tr("Channel layout")
            << QObject::tr("Frame size")
               ;
}

QVariantList getBaseInfoValues(const Statistics& s) {
    return QVariantList()
            << s.url
            << s.format
            << QString::number(s.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
            << s.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.duration.toString(QString::fromLatin1("HH:mm:ss"))
               ;
}

QList<QVariant> getVideoInfoValues(const Statistics& s) {
    return QList<QVariant>()
            << s.video.available
            << QString::fromLatin1("%1 (%2)").arg(s.video.codec).arg(s.video.codec_long)
            << s.video.decoder
            << s.video.decoder_detail
            << s.video.total_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.video.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << QString::number(s.video.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
            << s.video.frames
            << s.video.frame_rate
            << s.video.frame_rate
            << s.video_only.pix_fmt
            << QString::fromLatin1("%1x%2").arg(s.video_only.width).arg(s.video_only.height)
            << QString::fromLatin1("%1x%2").arg(s.video_only.coded_width).arg(s.video_only.coded_height)
            << s.video_only.gop_size
               ;
}
QList<QVariant> getAudioInfoValues(const Statistics& s) {
    return QList<QVariant>()
            << s.audio.available
            << QString::fromLatin1("%1 (%2)").arg(s.audio.codec).arg(s.audio.codec_long)
            << s.audio.decoder
            << s.audio.decoder_detail
            << s.audio.total_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.audio.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << QString::number(s.audio.bit_rate/1000).append(QString::fromLatin1(" Kb/s"))
            << s.audio.frames
            << s.audio.frame_rate
            << s.audio_only.sample_fmt
            << QString::number(s.audio_only.sample_rate).append(QString::fromLatin1(" Hz"))
            << s.audio_only.channels
            << s.audio_only.channel_layout
            << s.audio_only.frame_size
               ;
}


StatisticsView::StatisticsView(QWidget *parent) :
    QDialog(parent)
  , mTimer(0)
  , mpFPS(0)
  , mpAudioBitRate(0)
  , mpVideoBitRate(0)
{
    setWindowTitle(tr("Media info"));
    setModal(false);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    mpView = new QTreeWidget();
    mpView->setAnimated(true);
    mpView->setHeaderHidden(false);
    mpView->setColumnCount(2);
    mpView->headerItem()->setText(0, tr("Key"));
    mpView->headerItem()->setText(1, tr("Value"));
    initBaseItems(&mBaseItems);
    mpView->addTopLevelItems(mBaseItems);
    mpMetadata = new QTreeWidgetItem();
    mpMetadata->setText(0, tr("Metadata"));
    mpView->addTopLevelItem(mpMetadata);
    QTreeWidgetItem *item = createNodeWithItems(mpView, tr("Video"), getVideoInfoKeys(), &mVideoItems);
    mpFPS = item->child(9);
    //mpVideoBitRate =
    mpVideoMetadata = new QTreeWidgetItem(item);
    mpVideoMetadata->setText(0, tr("Metadata"));
    mpView->addTopLevelItem(item);
    item = createNodeWithItems(mpView, tr("Audio"), getAudioInfoKeys(), &mAudioItems);
    //mpAudioBitRate =
    mpAudioMetadata = new QTreeWidgetItem(item);
    mpAudioMetadata->setText(0, tr("Metadata"));
    mpView->addTopLevelItem(item);
    mpView->resizeColumnToContents(0); //call this after content is done

    QPushButton *btn = new QPushButton(tr("Ok"));
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btn);
    QObject::connect(btn, SIGNAL(clicked()), SLOT(accept()));

    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(mpView);
    vl->addLayout(btnLayout);
    setLayout(vl);
}

void StatisticsView::setStatistics(const Statistics& s)
{
    mStatistics = s;
    QVariantList v = getBaseInfoValues(s);
    int i = 0;
    foreach(QTreeWidgetItem* item, mBaseItems) {
        if (item->data(1, Qt::DisplayRole) != v.at(i)) {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }
        ++i;
    }
    v = getVideoInfoValues(s);
    i = 0;
    foreach(QTreeWidgetItem* item, mVideoItems) {
        if (item->data(1, Qt::DisplayRole) != v.at(i)) {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }
        ++i;
    }
    v = getAudioInfoValues(s);
    i = 0;
    foreach(QTreeWidgetItem* item, mAudioItems) {
        if (item->data(1, Qt::DisplayRole) != v.at(i)) {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }
        ++i;
    }
    setMetadataItem(mpMetadata, s.metadata);
    setMetadataItem(mpVideoMetadata, s.video.metadata);
    setMetadataItem(mpAudioMetadata, s.audio.metadata);
}

void StatisticsView::hideEvent(QHideEvent *e)
{
    QDialog::hideEvent(e);
    killTimer(mTimer);
}

void StatisticsView::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    mTimer = startTimer(1000);
}

void StatisticsView::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != mTimer)
        return;
    if (mpFPS) {
        mpFPS->setData(1, Qt::DisplayRole, QString::number(mStatistics.video_only.currentDisplayFPS(), 'f', 2));
    }
}

void StatisticsView::initBaseItems(QList<QTreeWidgetItem *> *items)
{
    QTreeWidgetItem *item = 0;
    foreach(const QString& key, getBaseInfoKeys()) {
        item = new QTreeWidgetItem(0);
        item->setData(0, Qt::DisplayRole, key);
        items->append(item);
    }
}

QTreeWidgetItem* StatisticsView::createNodeWithItems(QTreeWidget *view, const QString &name, const QStringList &itemNames, QList<QTreeWidgetItem *> *items)
{
    QTreeWidgetItem *nodeItem = new QTreeWidgetItem(view);
    nodeItem->setData(0, Qt::DisplayRole, name);
    QTreeWidgetItem *item = 0;
    foreach(const QString& key, itemNames) {
        item = new QTreeWidgetItem(nodeItem);
        item->setData(0, Qt::DisplayRole, key);
        nodeItem->addChild(item);
        if (items)
            items->append(item);
    }
    nodeItem->setExpanded(true);
    return nodeItem;
}

void StatisticsView::setMetadataItem(QTreeWidgetItem *parent, const QHash<QString, QString> &metadata)
{
    if (parent->childCount() > 0) {
        QList<QTreeWidgetItem *> children(parent->takeChildren());
        qDeleteAll(children);
    }
    QHash<QString, QString>::const_iterator it = metadata.constBegin();
    for (;it != metadata.constEnd(); ++it) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        item->setText(0, it.key());
        item->setText(1, it.value());
    }
}
