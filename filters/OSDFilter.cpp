#include "OSDFilter.h"
#include <QtAV/Statistics.h>
#include <QPainter>

OSDFilter::OSDFilter(QObject *parent) : VideoFilter(parent), OSD()
{

}

void OSDFilter::process(Statistics *statistics, VideoFrame *frame)
{
    Q_UNUSED(frame);
    if (mShowType == ShowNone)
    {
        return;
    }
    context()->drawPlainText(context()->rect, Qt::AlignLeft/*Qt::AlignCenter*/, text(statistics));
}
