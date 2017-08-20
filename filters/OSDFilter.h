#pragma once

#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>
#include "OSD.h"

using namespace QtAV;

class OSDFilter : public VideoFilter, public OSD
{
public:
    OSDFilter(QObject *parent = nullptr);
    bool isSupported(VideoFilterContext::Type ct) const
    {
        return ct == VideoFilterContext::QtPainter || ct == VideoFilterContext::X11;
    }

protected:
    void process(Statistics* statistics, VideoFrame* frame);
};
