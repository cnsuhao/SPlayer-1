#include "OSD.h"
#include <QtAV/Statistics.h>
#include <QTime>

namespace QtAV {

OSD::OSD():
    mShowType(ShowNone)
  , mSecsTotal(-1)
  , m_pAVPlayer(NULL)
{

}

OSD::~OSD()
{
    if (m_pAVPlayer)
    {
        m_pAVPlayer = NULL;
    }
}

void OSD::setShowType(ShowType type)
{
    mShowType = type;
}

OSD::ShowType OSD::showType() const
{
    return mShowType;
}

bool OSD::hasShowType(ShowType t) const
{
    return (t&mShowType) == t;
}

QString OSD::text(Statistics *statistics)
{
    QString text = QString::fromLatin1(" ");
    if (hasShowType(ShowCurrentAndTotalTime))
    {
        Statistics::Common *av = &statistics->video;
        if (!av->available)
        {
            av = &statistics->audio;
        }
        text = av->current_time.toString(QString::fromLatin1("HH:mm:ss"));
        if (statistics->duration.isNull())
        {
            text += QString::fromLatin1("/--:--:--");
            return text;
        }
        mSecsTotal = QTime(0, 0, 0).secsTo(statistics->duration); //why video.total_time may be wrong(mkv)
        if (mSecsTotal > 0)
        {
            text += QString::fromLatin1("/").append(statistics->duration.toString(QString::fromLatin1("HH:mm:ss")));
        }
        else
        {
            text += QString::fromLatin1("/--:--:--");
        }
        return text;
    }
    if (hasShowType(ShowCurrentVolumePercent))
    {
        if (!m_pAVPlayer || !m_pAVPlayer->audio() || !m_pAVPlayer->audio()->isAvailable())
        {
            return text;
        }
        int vol = static_cast<int>(m_pAVPlayer->audio()->volume() * 100.0 / 4.0);
        text = QObject::tr("Volume ") + QString::number(vol) + QString::fromLatin1("%");
        return text;
    }
    if (hasShowType(ShowMuteStatus))
    {
        if (!m_pAVPlayer || !m_pAVPlayer->audio() || !m_pAVPlayer->audio()->isAvailable())
        {
            return text;
        }
        if (m_pAVPlayer->audio()->isMute())
        {
            text = QObject::tr("Muted");
            return text;
        }
        text = QObject::tr("Sound");
        return text;
    }
    if (hasShowType(ShowPlayPauseStatus))
    {
        if (!m_pAVPlayer)
        {
            return text;
        }
        if (m_pAVPlayer->isPaused())
        {
            text = QObject::tr("Paused");
            return text;
        }
        text = QObject::tr("Play");
        return text;
    }
    if (hasShowType(ShowCurrentLocalTime))
    {
        text = QTime::currentTime().toString("hh:mm:ss");
        return text;
    }
    return text;
}

void OSD::setAVPlayer(AVPlayer *m_AVPlayer)
{
    if (m_AVPlayer)
    {
        m_pAVPlayer = m_AVPlayer;
    }
}


} //namespace QtAV

