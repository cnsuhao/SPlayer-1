#pragma once

#include "PCH.h"
#include "Control/BaseWidget.h"
#include "StandardDialog/StandardDialog.h"

class HelpBottom : public BaseWidget
{
    Q_OBJECT

public:
    HelpBottom(QWidget *parent);
    virtual ~HelpBottom();

signals:
    void onOKButtonClicked();
};

class HelpBox : public StandardDialog
{
    Q_OBJECT

public:
    explicit HelpBox(QWidget *parent = nullptr);

protected:
    void Slot_MouseDoubleClicked();

private:
    QString appHelp_HTML();

private:
    HelpBottom *mpHelpBottom;
};
