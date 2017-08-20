#pragma once

#include <QMenu>
#include "Control/BaseWidget.h"
#include "Control/SButton.h"

class SOpenFileButton : public SButton
{
    Q_OBJECT

public:
    SOpenFileButton(QWidget *parent = nullptr);
    virtual ~SOpenFileButton();

signals:
    void onArrowButtonClicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

class LogoWidget : public BaseWidget
{
    Q_OBJECT

public:
    LogoWidget(QWidget *parent = nullptr);
    virtual ~LogoWidget();

private slots:
    void onShowPopupMenu();

signals:
    void onOpenLocalFileButtonClicked();
    void onOpenURLButtonClicked();

public:
    void SetLogoImage(const QString &imageFilePath);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    void createCentralButtonMenu();

private:
    QImage m_logoImage;
    SOpenFileButton *m_pOpenFile;
    QMenu *m_pCentralButtonMenu;
};
