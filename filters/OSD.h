#pragma once

#include <QtAV/QtAV_Global.h>
#include <QtAV/AVPlayer.h>
#include <QPoint>
#include <QFont>

namespace QtAV {

class Statistics;

class OSD
{
public:
    enum ShowType
    {
        ShowCurrentAndTotalTime = 1,
        ShowCurrentVolumePercent = 1<<1,
        ShowMuteStatus = 1<<2,
        ShowPlayPauseStatus = 1<<3,
        ShowCurrentLocalTime = 1<<4,
        ShowNone
    };

    OSD();
    virtual ~OSD();

    void setShowType(ShowType type);
    ShowType showType() const;
    bool hasShowType(ShowType t) const;
    QString text(Statistics* statistics);

    void setAVPlayer(QtAV::AVPlayer *m_AVPlayer);

protected:
    ShowType mShowType;
    int mSecsTotal;

    QtAV::AVPlayer *m_pAVPlayer;
};

}//namespace QtAV
