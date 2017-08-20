#include "PlayListItem.h"
#include <QTime>
#include <QDataStream>

QDataStream& operator>> (QDataStream& s, PlayListItem& p)
{
    int stars;
    qint64 duration, last_time;
    QString url, title;
    s >> url >> title >> duration >> last_time >> stars;
    p.setTitle(title);
    p.setUrl(url);
    p.setStars(stars);
    p.setDuration(duration);
    p.setLastTime(last_time);
    return s;
}

QDataStream& operator<< (QDataStream& s, const PlayListItem& p)
{
    s << p.url() << p.title() << p.duration() << p.lastTime() << p.stars();
    return s;
}

PlayListItem::PlayListItem()
    : mStars(0)
    , mLastTime(0)
    , mDuration(0)
{
}

void PlayListItem::setTitle(const QString &title)
{
    mTitle = title;
}

QString PlayListItem::title() const
{
    return mTitle;
}

void PlayListItem::setUrl(const QString &url)
{
    mUrl = url;
}

QString PlayListItem::url() const
{
    return mUrl;
}

void PlayListItem::setStars(int s)
{
    mStars = s;
}

int PlayListItem::stars() const
{
    return mStars;
}

void PlayListItem::setLastTime(qint64 ms)
{
    mLastTime = ms;
    mLastTimeS = QTime(0, 0, 0, 0).addMSecs(ms).toString(QString::fromLatin1("HH:mm:ss"));
}

qint64 PlayListItem::lastTime() const
{
    return mLastTime;
}

QString PlayListItem::lastTimeString() const
{
    return mLastTimeS;
}

void PlayListItem::setDuration(qint64 ms)
{
    mDuration = ms;
    mDurationS = QTime(0, 0, 0, 0).addMSecs(ms).toString(QString::fromLatin1("HH:mm:ss"));
}

qint64 PlayListItem::duration() const
{
    return mDuration;
}

QString PlayListItem::durationString() const
{
    return mDurationS;
}

bool PlayListItem::operator ==(const PlayListItem& other) const
{
    return url() == other.url();
}
