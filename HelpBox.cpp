#include "HelpBox.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QTabWidget>
#include <QVBoxLayout>

HelpBottom::HelpBottom(QWidget *parent) : BaseWidget(parent)
{
    QPushButton *okbtn = new QPushButton(this);
    okbtn->setText(tr("OK"));
    connect(okbtn, SIGNAL(clicked()), this, SIGNAL(onOKButtonClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->setContentsMargins(10, 0, 10, 0);
    btnLayout->setSpacing(0);
    btnLayout->addStretch();
    btnLayout->addWidget(okbtn);
    okbtn->setFocus();
}

HelpBottom::~HelpBottom()
{

}

HelpBox::HelpBox(QWidget *parent) : StandardDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    GetHeader()->SetTitleText(tr("Help"));
    setWindowTitle(tr("Help"));
    GetHeader()->SetTitleCenter(false);
    GetHeader()->SetLogoVisible(false);
    GetHeader()->SetSkinVisible(false);
    GetHeader()->SetMinVisible(false);
    GetHeader()->SetMaxVisible(false);
    GetHeader()->SetStayOnTopVisible(false);
    GetHeader()->setFixedHeight(40);
    GetBottom()->setFixedHeight(50);

    QTextBrowser *viewHelp = new QTextBrowser(this);
    viewHelp->setOpenExternalLinks(true);
    viewHelp->setHtml(appHelp_HTML());
    QTabWidget *tab = new QTabWidget(this);
    tab->addTab(viewHelp, tr("Help"));

    mpHelpBottom = new HelpBottom(this);
    connect(mpHelpBottom, SIGNAL(onOKButtonClicked()), this, SLOT(close()));
    SetBottomWidget(mpHelpBottom);

    BaseWidget *mpCenterContainer = new BaseWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(mpCenterContainer);
    mpCenterContainer->setLayout(layout);
    layout->addWidget(tab);
    SetCenterWidget(mpCenterContainer);
    resize(570, 440);
    SetFixed(true);
}

void HelpBox::Slot_MouseDoubleClicked()
{

}

QString HelpBox::appHelp_HTML()
{
    static QString appHelp =
            QString::fromLatin1("<h4>%1</h4><ul><li>%2</li><li>%3</li><li>%4</li>"
            "<li>%5</li><li>%6</li><li>%7</li><li>%8</li><li>%9</li><li>%10</li>"
            "<li>%11</li><li>%12</li></ul>"
            "<p>%13<a href='" AppWikiURL "'>" AppWikiURL "</a><p>"
           ).arg(tr("Shortcuts"))
            .arg(tr("Switch fullscreen/normal window : Enter / Return / F11"))
            .arg(tr("Open file : Ctrl + O"))
            .arg(tr("Play/Pause : Space"))
            .arg(tr("Quit player/Stop/Exit fullscreen : Esc"))
            .arg(tr("Mute/Sound : M"))
            .arg(tr("Volume +/- : Up / Down / Mouse Wheel"))
            .arg(tr("Seek forward/backward : -&gt; / &lt;-"))
            .arg(tr("Seek next/previous chapter : Page Down / Page Up"))
            .arg(tr("Setup : F5"))
            .arg(tr("Help : F1"))
            .arg(tr("Drag video file to the play area can also open it"))
            .arg(tr("More information : "));
    return appHelp;
}
