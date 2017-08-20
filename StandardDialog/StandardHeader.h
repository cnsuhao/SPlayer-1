#pragma once

#include <QLabel>
#include "Control/BaseWidget.h"
#include "Control/SButton.h"

class StandardHeader : public BaseWidget
{
    Q_OBJECT

public:
    explicit StandardHeader(QWidget *parent = nullptr);
    virtual ~StandardHeader();

public:
    void SetLogoVisible(bool bVisible);
    void SetTitleText(const QString &str);
    void SetTitleCenter(bool bCenter);
    void SetStayOnTopVisible(bool bVisible);
    void SetSkinVisible(bool bVisible);
    void SetMinVisible(bool bVisible);
    void SetMaxVisible(bool bVisible);
    void SetCloseVisible(bool bVisible);
    void SetMaxImage();
    void SetRestoreImage();
    void SetStayOnTopImage();
    void SetStayedOnTopImage();

    QWidget *getSkinButton();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void Signal_LogoClicked();
    void Signal_StayOnTopClicked();
    void Signal_SkinClicked();
    void Signal_MinClicked();
    void Signal_MaxClicked();
    void Signal_CloseClicked();
    void Signal_MouseDoubleClicked();
    void Signal_MousePressed();
    void Signal_MouseReleased();

private:
    void CreateLayout();

private:
    SButton *m_pLogo;
    QLabel *m_pTitle;
    SButton *m_pStayOnTop;
    SButton *m_pSkin;
    SButton *m_pMin;
    SButton *m_pMax;
    SButton *m_pClose;
    QColor m_backgroundColor;
};
