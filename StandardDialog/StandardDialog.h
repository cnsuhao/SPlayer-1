#pragma once

#include <QVBoxLayout>
#include "StandardHeader.h"
#include "StandardBottom.h"
#include "Control/BaseWidget.h"

class StandardDialog : public BaseWidget
{
    Q_OBJECT

public:
    explicit StandardDialog(QWidget *parent = nullptr);
    virtual ~StandardDialog();

public:
    StandardHeader *GetHeader();
    StandardBottom *GetBottom();

public:
    void SetFixed(bool bFixedSize);
    bool GetFixed();
    void setWindowMaximize(bool canMaximize);
    bool getIsMaximized();
    void setGeometryAsMaximizedWindow();
    void setRestoreGeometry(QRect Geometry);
    void setRestoreSize(QSize Size);
    void exitFromFullscreenMode();
    QRect getRestoreGeometry();
    QSize getRestoreSize();
    void setWindowCanMove(bool canMove);
    bool getWindowCanMove();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

protected slots:
    virtual void Slot_LogoClicked();
    virtual void Slot_StayOnTopClicked();
    virtual void Slot_SkinClicked();
    virtual void Slot_MinClicked();
    virtual void Slot_MaxClicked();
    virtual void Slot_CloseClicked();
    virtual void Slot_MouseDoubleClicked();
    virtual void Slot_HeaderPressed();
    virtual void Slot_HeaderReleased();

//protected:
public:
    void SetCenterWidget(QWidget *pCenterWidget);
    void SetBottomWidget(QWidget *pBottomWidget);
    void RemoveBottomWidget();

private:
    StandardHeader *m_pHeader;
    StandardBottom *m_pBottom;
    QVBoxLayout *m_pVBoxLayout;
    bool m_bDialogMaxed;
    QRect m_restoreGeometry;
    QPoint m_sourcePos;
    bool m_bFixedSize;
    bool m_bHeaderPressed;
    QSize m_restoreSize;
    bool m_bStayOnTop;
    bool m_bCanMove;
};
