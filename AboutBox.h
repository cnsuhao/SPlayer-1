#pragma once

#include "PCH.h"
#include "Control/BaseWidget.h"
#include "StandardDialog/StandardDialog.h"

class AboutBottom : public BaseWidget
{
    Q_OBJECT

public:
    AboutBottom(QWidget *parent);
    virtual ~AboutBottom();

signals:
    void onOKButtonClicked();
    void onAboutQtAVButtonClicked();
};

class AboutBox : public StandardDialog
{
    Q_OBJECT

public:
    explicit AboutBox(QWidget *parent = nullptr);

signals:
    void onAboutQtAV();

private:
    QString aboutApp_HTML();
    QString aboutLicenses_HTML();
    QString license_LGPLv21();
    QString license_LGPLv3();
    QString license_GPLv3();

private:
    AboutBottom *mpAboutBottom;
};
