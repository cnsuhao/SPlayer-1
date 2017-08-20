#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QEventLoop>
#include <QCloseEvent>
#include "Control/BaseWidget.h"
#include "StandardDialog/StandardDialog.h"

class SMsgBoxYesNoBottom : public BaseWidget
{
    Q_OBJECT

public:
    SMsgBoxYesNoBottom(QWidget *parent);
    virtual ~SMsgBoxYesNoBottom();

signals:
    void onYesButtonClicked();
    void onNoButtonClicked();

private:
    QPushButton *mpYesButton;
    QPushButton *mpNoButton;
};

class SMsgBoxOKBottom : public BaseWidget
{
    Q_OBJECT

public:
    SMsgBoxOKBottom(QWidget *parent);
    virtual ~SMsgBoxOKBottom();

signals:
    void onOKButtonClicked();

private:
    QPushButton *mpOKButton;
};

class SMessageBox : public StandardDialog
{
    Q_OBJECT

public:
    //msgType : 消息类型——0，正常，图标为一个绿色对勾，按钮为一个OK；
    //1，通知，图标为一个蓝色感叹号，按钮为一个OK；
    //2，询问，图标为一个蓝色问号，按钮为一个Yes和一个No
    //3，错误，图标为一个红色叉号，按钮为一个OK
    //返回值：-1——错误/未知，0——OK/Yes，1——No
    int getMsgBoxResult();
    void setTitle(const QString &title = QString());
    void setTxt(const QString &txt = QString());
    void setMsgBoxType(const int &msgType = 0);
    int exec();
    static int information(QWidget *parent = nullptr, const QString &title = QString()
                        , const QString &text = QString());
    static int error(QWidget *parent = nullptr, const QString &title = QString()
                        , const QString &text = QString());
    static int normal(QWidget *parent = nullptr, const QString &title = QString()
                        , const QString &text = QString());
    static int question(QWidget *parent = nullptr, const QString &title = QString()
                        , const QString &text = QString());

private slots:
    void onButtonOKClicked();
    void onButtonYesClicked();
    void onButtonNoClicked();

protected:
    void Slot_MouseDoubleClicked();
    void closeEvent(QCloseEvent *event);

private:
    explicit SMessageBox(QWidget *parent = nullptr);
    ~SMessageBox();

private:
    SMsgBoxOKBottom *mpOKBottom;
    SMsgBoxYesNoBottom *mpYesNoBottom;
    QLabel *mpMsgIcon;
    QLabel *mpMsgTxt;
    int mMsgBoxResult;
    QEventLoop *mpEventLoop;
};
