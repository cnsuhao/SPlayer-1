#pragma once

#include <QHBoxLayout>
#include "Control/BaseWidget.h"

//界面底部基类
class StandardBottom : public BaseWidget
{
public:
    explicit StandardBottom(QWidget *parent = nullptr);
    virtual ~StandardBottom();

public:
    void AddWidget(QWidget *pWidget);
    bool GetIsBottomSetted();
    void RemoveWidget();

private:
    QHBoxLayout *m_pHBoxLayout;
    QWidget *m_pBottomWidget;
};
