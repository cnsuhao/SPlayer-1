#pragma once

#include "BaseWidget.h"

class SProgressBar : public BaseWidget
{
public:
    SProgressBar(QWidget *parent = nullptr);
    virtual ~SProgressBar();

public:
    void setMinimum(qint64 value);
    void setMaximum(qint64 value);
    void setValue(qint64 value);
    qint64 value();

protected:
    virtual void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public:
    qint64 m_currentProgress, min, max;
};
