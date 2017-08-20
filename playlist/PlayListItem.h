#pragma once

#include <QString>
#include <QVariant>

class PlayListItem
{
public:
    PlayListItem();
    void setTitle(const QString& title);
    QString title() const;
    void setUrl(const QString& url);
    QString url() const;
    void setStars(int s);
    int stars() const;
    void setLastTime(qint64 ms);
    qint64 lastTime() const;
    QString lastTimeString() const;
    void setDuration(qint64 ms);
    qint64 duration() const;
    QString durationString() const;
    //icon
    bool operator ==(const PlayListItem& other) const;

private:
    QString mTitle;
    QString mUrl;
    int mStars;
    qint64 mLastTime, mDuration;
    QString mLastTimeS, mDurationS;
};

Q_DECLARE_METATYPE(PlayListItem)

QDataStream& operator>> (QDataStream& s, PlayListItem& p);
QDataStream& operator<< (QDataStream& s, const PlayListItem& p);
