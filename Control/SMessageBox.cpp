#include "SMessageBox.h"
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QPixmap>

SMsgBoxYesNoBottom::SMsgBoxYesNoBottom(QWidget *parent) : BaseWidget(parent)
  , mpYesButton(NULL)
  , mpNoButton(NULL)
{
    mpYesButton = new QPushButton(this);
    mpYesButton->setText(tr("Yes"));
    connect(mpYesButton, SIGNAL(clicked()), this, SIGNAL(onYesButtonClicked()));
    mpNoButton = new QPushButton(this);
    mpNoButton->setText(tr("No"));
    connect(mpNoButton, SIGNAL(clicked()), this, SIGNAL(onNoButtonClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->setContentsMargins(10, 0, 10, 0);
    btnLayout->setSpacing(10);
    btnLayout->addStretch();
    btnLayout->addWidget(mpYesButton);
    btnLayout->addWidget(mpNoButton);
}

SMsgBoxYesNoBottom::~SMsgBoxYesNoBottom()
{

}

SMsgBoxOKBottom::SMsgBoxOKBottom(QWidget *parent) : BaseWidget(parent)
  , mpOKButton(NULL)
{
    mpOKButton = new QPushButton(this);
    mpOKButton->setText(tr("OK"));
    connect(mpOKButton, SIGNAL(clicked()), this, SIGNAL(onOKButtonClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->setContentsMargins(10, 0, 10, 0);
    btnLayout->setSpacing(10);
    btnLayout->addStretch();
    btnLayout->addWidget(mpOKButton);
}

SMsgBoxOKBottom::~SMsgBoxOKBottom()
{

}

SMessageBox::SMessageBox(QWidget *parent) : StandardDialog(parent)
  , mpOKBottom(NULL)
  , mpYesNoBottom(NULL)
  , mpMsgIcon(NULL)
  , mpMsgTxt(NULL)
  , mMsgBoxResult(-1)
  , mpEventLoop(NULL)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    GetHeader()->SetTitleText(QString::fromLatin1("Message Box"));
    setWindowTitle(QString::fromLatin1("Message Box"));
    GetHeader()->SetTitleCenter(false);
    GetHeader()->SetLogoVisible(false);
    GetHeader()->SetSkinVisible(false);
    GetHeader()->SetMinVisible(false);
    GetHeader()->SetMaxVisible(false);
    GetHeader()->SetStayOnTopVisible(false);
    GetHeader()->setFixedHeight(40);
    GetBottom()->setFixedHeight(50);

    mpOKBottom = new SMsgBoxOKBottom(this);
    connect(mpOKBottom, SIGNAL(onOKButtonClicked()), this, SLOT(onButtonOKClicked()));
    mpOKBottom->hide();
    mpYesNoBottom = new SMsgBoxYesNoBottom(this);
    connect(mpYesNoBottom, SIGNAL(onYesButtonClicked()), this, SLOT(onButtonYesClicked()));
    connect(mpYesNoBottom, SIGNAL(onNoButtonClicked()), this, SLOT(onButtonNoClicked()));
    mpYesNoBottom->hide();

    BaseWidget *mpCenterContainer = new BaseWidget(this);
    mpMsgIcon = new QLabel(mpCenterContainer);
    mpMsgIcon->setScaledContents(true);
    mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Normal.png"));
    mpMsgTxt = new QLabel(mpCenterContainer);
    mpMsgTxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout *m_pHLayout = new QHBoxLayout(mpCenterContainer);
    m_pHLayout->setContentsMargins(10, 10, 10, 10);
    m_pHLayout->setSpacing(10);
    m_pHLayout->addWidget(mpMsgIcon);
    m_pHLayout->addWidget(mpMsgTxt);
    mpCenterContainer->setLayout(m_pHLayout);
    SetCenterWidget(mpCenterContainer);

    SetFixed(true);
}

SMessageBox::~SMessageBox()
{
    if (mpOKBottom)
    {
        mpOKBottom->close();
        delete mpOKBottom;
        mpOKBottom = NULL;
    }
    if (mpYesNoBottom)
    {
        mpYesNoBottom->close();
        delete mpYesNoBottom;
        mpYesNoBottom = NULL;
    }
    if (mpMsgIcon)
    {
        mpMsgIcon->close();
        delete mpMsgIcon;
        mpMsgIcon = NULL;
    }
    if (mpMsgTxt)
    {
        mpMsgTxt->close();
        delete mpMsgTxt;
        mpMsgTxt = NULL;
    }
    if (mpEventLoop)
    {
        delete mpEventLoop;
        mpEventLoop = NULL;
    }
}

int SMessageBox::getMsgBoxResult()
{
    return mMsgBoxResult;
}

void SMessageBox::setTitle(const QString &title)
{
    if (title.isEmpty())
    {
        return;
    }
    GetHeader()->SetTitleText(title);
    setWindowTitle(title);
}

void SMessageBox::setTxt(const QString &txt)
{
    if (txt.isEmpty())
    {
        return;
    }
    mpMsgTxt->setText(txt);
}

void SMessageBox::setMsgBoxType(const int &msgType)
{
    switch (msgType)
    {
    case 0:
        mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Normal.png"));
        SetBottomWidget(mpOKBottom);
        mpOKBottom->show();
        mpYesNoBottom->hide();
        break;
    case 1:
        mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Information.png"));
        SetBottomWidget(mpOKBottom);
        mpOKBottom->show();
        mpYesNoBottom->hide();
        break;
    case 2:
        mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Question.png"));
        SetBottomWidget(mpYesNoBottom);
        mpYesNoBottom->show();
        mpOKBottom->hide();
        break;
    case 3:
        mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Error.png"));
        SetBottomWidget(mpOKBottom);
        mpOKBottom->show();
        mpYesNoBottom->hide();
        break;
    default:
        mpMsgIcon->setPixmap(QPixmap(":/Icon/MsgBox_Normal.png"));
        SetBottomWidget(mpOKBottom);
        mpOKBottom->show();
        mpYesNoBottom->hide();
        break;
    }
}

int SMessageBox::exec()
{
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_ShowModal);
    show();
    mpEventLoop = new QEventLoop(this);
    mpEventLoop->exec();
    return mMsgBoxResult;
}

int SMessageBox::information(QWidget *parent, const QString &title, const QString &text)
{
    SMessageBox msgBox;
    if (!parent)
    {
        msgBox.setParent(parent);
    }
    if (!title.isEmpty())
    {
        msgBox.setTitle(title);
    }
    if (!text.isEmpty())
    {
        msgBox.setTxt(text);
    }
    msgBox.setMsgBoxType(1);
    return msgBox.exec();
}

int SMessageBox::error(QWidget *parent, const QString &title, const QString &text)
{
    SMessageBox msgBox;
    if (!parent)
    {
        msgBox.setParent(parent);
    }
    if (!title.isEmpty())
    {
        msgBox.setTitle(title);
    }
    if (!text.isEmpty())
    {
        msgBox.setTxt(text);
    }
    msgBox.setMsgBoxType(3);
    return msgBox.exec();
}

int SMessageBox::normal(QWidget *parent, const QString &title, const QString &text)
{
    SMessageBox msgBox;
    if (!parent)
    {
        msgBox.setParent(parent);
    }
    if (!title.isEmpty())
    {
        msgBox.setTitle(title);
    }
    if (!text.isEmpty())
    {
        msgBox.setTxt(text);
    }
    msgBox.setMsgBoxType(0);
    return msgBox.exec();
}

int SMessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    SMessageBox msgBox;
    if (!parent)
    {
        msgBox.setParent(parent);
    }
    if (!title.isEmpty())
    {
        msgBox.setTitle(title);
    }
    if (!text.isEmpty())
    {
        msgBox.setTxt(text);
    }
    msgBox.setMsgBoxType(2);
    return msgBox.exec();
}

void SMessageBox::onButtonOKClicked()
{
    mMsgBoxResult = 0;
    close();
}

void SMessageBox::onButtonYesClicked()
{
    mMsgBoxResult = 0;
    close();
}

void SMessageBox::onButtonNoClicked()
{
    mMsgBoxResult = 1;
    close();
}

void SMessageBox::Slot_MouseDoubleClicked()
{

}

void SMessageBox::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时结束事件循环，在exec()方法中返回选择结果;
    if (mpEventLoop != NULL)
    {
        mpEventLoop->exit();
    }
    event->accept();
}
