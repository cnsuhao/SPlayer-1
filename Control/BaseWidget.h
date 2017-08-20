#pragma once

#include <QWidget>
#include <QColor>
#include <QPen>
#include "ObjectFourTuple.h"

//控件基类，用于公共属性、公共行为的抽象
class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidget(QWidget *parent = nullptr);
    virtual ~BaseWidget();

    //信号定义
signals:
    void Signal_Clicked();

public:
    ObjectFourTuple<QColor>& GetBgColor();
    void ResetButtonState();

    //事件重载
protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    //重绘处理
    virtual void paintEvent(QPaintEvent *event);

private:
    void ResetState();

private:
    ObjectFourTuple<QColor> m_bgColor;
    QMargins m_margin;
    ObjectFourTuple<QPen> m_borderPen;

protected:
    bool m_bEnter;
    bool m_bPress;
};
