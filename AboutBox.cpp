#include "AboutBox.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QTabWidget>
#include <QVBoxLayout>

AboutBottom::AboutBottom(QWidget *parent) : BaseWidget(parent)
{
    QPushButton *qtavbtn = new QPushButton(this);
    qtavbtn->setText(tr("About QtAV"));
    connect(qtavbtn, SIGNAL(clicked()), this, SIGNAL(onAboutQtAVButtonClicked()));
    QPushButton *okbtn = new QPushButton(this);
    okbtn->setText(tr("OK"));
    connect(okbtn, SIGNAL(clicked()), this, SIGNAL(onOKButtonClicked()));
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->setContentsMargins(10, 0, 10, 0);
    btnLayout->setSpacing(0);
    btnLayout->addWidget(qtavbtn);
    btnLayout->addStretch();
    btnLayout->addWidget(okbtn);
    okbtn->setFocus();
}

AboutBottom::~AboutBottom()
{

}

AboutBox::AboutBox(QWidget *parent) : StandardDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    GetHeader()->SetTitleText(tr("About") + QString::fromLatin1("  ")
                              + QString::fromLatin1(AppName));
    setWindowTitle(tr("About") + QString::fromLatin1("  ")
                              + QString::fromLatin1(AppName));
    GetHeader()->SetTitleCenter(false);
    GetHeader()->SetLogoVisible(false);
    GetHeader()->SetSkinVisible(false);
    GetHeader()->SetMinVisible(false);
    GetHeader()->SetMaxVisible(false);
    GetHeader()->SetStayOnTopVisible(false);
    GetHeader()->setFixedHeight(40);
    GetBottom()->setFixedHeight(50);

    mpAboutBottom = new AboutBottom(this);
    connect(mpAboutBottom, SIGNAL(onAboutQtAVButtonClicked()), this
            , SIGNAL(onAboutQtAV()));
    connect(mpAboutBottom, SIGNAL(onOKButtonClicked()), this, SLOT(close()));
    SetBottomWidget(mpAboutBottom);

    QTextBrowser *viewApp = new QTextBrowser(this);
    QTextBrowser *viewLicenses = new QTextBrowser(this);
    viewApp->setOpenExternalLinks(true);
    viewLicenses->setOpenExternalLinks(true);
    viewApp->setHtml(aboutApp_HTML());
    viewLicenses->setHtml(aboutLicenses_HTML());
    QTabWidget *tab = new QTabWidget(this);
    tab->addTab(viewApp, QString::fromLatin1(AppName));
    tab->addTab(viewLicenses, tr("Licenses"));

    BaseWidget *mpCenterContainer = new BaseWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(mpCenterContainer);
    mpCenterContainer->setLayout(layout);
    layout->addWidget(tab);
    SetCenterWidget(mpCenterContainer);
    resize(445, 700);
}

QString AboutBox::aboutApp_HTML()
{
    static QString aboutApp =
            QString::fromLatin1("<img src='qrc:/Icon/SPlayer.ico'><h3>"
                                AppName "        " AppVersion "</h3>\n"
            "<p>%1</p>\n<p>%2</p>\n<p>%3</p>\n<p>%4</p>\n<p>%5</p>\n"
            "<p>%6 : <a href='" AppPublisherEmail "'>" AppPublisherEmail "</a></p>\n"
            "<p>%7 : <a href='" AppDonateURL "'>" AppDonateURL "</a></p>\n"
            "<p>%8 : <a href='" AppSourceCodeURL "'>" AppSourceCodeURL "</a></p>\n"
            "<p>%9 : <a href='" AppPublisherURL "'>" AppPublisherURL "</a></p>\n"
           ).arg(tr("A high performance multimedia player based on QtAV, Qt and FFmpeg."))
            .arg(tr("Distributed under the terms of GPLv3 or later."))
            .arg(tr("Sochoow City->Sochoow University, Jiang Su Province, China"))
            .arg(tr("Copyright © 2017 by ") + AppPublisher)
            .arg(tr("All rights reserved."))
            .arg(tr("E-mail address"))
            .arg(tr("Donate"))
            .arg(tr("Source code"))
            .arg(tr("Home page"));
    return aboutApp;
}

QString AboutBox::aboutLicenses_HTML()
{
    static QString licensesText = license_LGPLv21() + license_LGPLv3() + license_GPLv3();
    return licensesText;
}

QString AboutBox::license_LGPLv21()
{
    static QString licenseLGPLv21Text;
    QFile license(":/License/lgpl-2.1-standalone.html");
    license.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&license);
    licenseLGPLv21Text = ts.readAll();
    license.close();
    return licenseLGPLv21Text;
}

QString AboutBox::license_LGPLv3()
{
    static QString licenseLGPLv3Text;
    QFile license(":/License/lgpl-3.0-standalone.html");
    license.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&license);
    licenseLGPLv3Text = ts.readAll();
    license.close();
    return licenseLGPLv3Text;
}

QString AboutBox::license_GPLv3()
{
    static QString licenseGPLv3Text;
    QFile license(":/License/gpl-3.0-standalone.html");
    license.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&license);
    licenseGPLv3Text = ts.readAll();
    license.close();
    return licenseGPLv3Text;
}
