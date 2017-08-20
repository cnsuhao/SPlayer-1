#include "SProgressBar.h"
#include <QPainter>
#include <QMouseEvent>

SProgressBar::SProgressBar(QWidget *parent) : BaseWidget(parent)
  , m_currentProgress(0)
  , min(0)
  , max(1000)
{

}

SProgressBar::~SProgressBar()
{

}

void SProgressBar::setMinimum(qint64 value)
{
    min = value;
}

void SProgressBar::setMaximum(qint64 value)
{
    max = value;
}

void SProgressBar::setValue(qint64 value)
{
    if (value <= min)
    {
        m_currentProgress = min;
    }
    else if (value >= max)
    {
        m_currentProgress = max;
    }
    else
    {
        m_currentProgress = value;
    }
    update();
}

qint64 SProgressBar::value()
{
    return m_currentProgress;
}

void SProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter objPainter(this);
    objPainter.setRenderHint(QPainter::Antialiasing);
    //绘背景
    objPainter.fillRect(rect(),QColor(31,31,31));
    //绘内容区背景
    objPainter.fillRect(contentsRect(),QColor(78,78,78));
    qint64 len = max - min;
    if (len <= 0)
    {
        len = 1;
    }
    qint64 nWidth = contentsRect().width() * m_currentProgress / len;
    //绘进度条背景;
    objPainter.fillRect(contentsRect().x(),contentsRect().y(),nWidth,contentsRect().height(),QColor(26,158,255));

    BaseWidget::paintEvent(event);
}

void SProgressBar::mousePressEvent(QMouseEvent *event)
{
    //只做进度指示器，不处理任何鼠标事件
    event->ignore();
}

void SProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    //只做进度指示器，不处理任何鼠标事件
    event->ignore();
}
