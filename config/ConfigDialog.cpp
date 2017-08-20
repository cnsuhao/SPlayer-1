#include "ConfigDialog.h"
#include <QFile>
#include <QLayout>
#include <QPushButton>
#include "CaptureConfigPage.h"
#include "DecoderConfigPage.h"
#include "AVFormatConfigPage.h"
#include "AVFilterConfigPage.h"
#include "MiscPage.h"
#include "ShaderPage.h"
#include "common/Config.h"

/*void ConfigDialog::display()
{
    static ConfigDialog *dialog = new ConfigDialog();
    dialog->show();
}*/

ConfigDialog::ConfigDialog(QWidget *parent) : StandardDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    GetHeader()->SetTitleText(tr("Setup"));
    setWindowTitle(tr("Setup"));
    GetHeader()->SetTitleCenter(false);
    GetHeader()->SetLogoVisible(false);
    GetHeader()->SetSkinVisible(false);
    GetHeader()->SetMinVisible(false);
    GetHeader()->SetMaxVisible(false);
    GetHeader()->SetStayOnTopVisible(false);
    GetHeader()->setFixedHeight(40);
    GetBottom()->setFixedHeight(50);

    mpConfigBottom = new ConfigBottom(this);
    connect(mpConfigBottom, SIGNAL(onResetButtonClicked()), this, SLOT(onReset()));
    connect(mpConfigBottom, SIGNAL(onOKButtonClicked()), this, SLOT(onApply()));
    connect(mpConfigBottom, SIGNAL(onCancelButtonClicked()), this, SLOT(onCancel()));
    connect(mpConfigBottom, SIGNAL(onApplyButtonClicked()), this, SLOT(onApply()));
    SetBottomWidget(mpConfigBottom);

    BaseWidget *mpCenterContainer = new BaseWidget(this);

    QVBoxLayout *vbl = new QVBoxLayout(mpCenterContainer);
    mpCenterContainer->setLayout(vbl);
    vbl->setSizeConstraint(QLayout::SetFixedSize);

    mpContent = new QTabWidget(mpCenterContainer);
    mpContent->setTabPosition(QTabWidget::West);

    vbl->addWidget(mpContent);

    mPages << new MiscPage()
           << new CaptureConfigPage()
           << new DecoderConfigPage()
           << new AVFormatConfigPage()
           << new AVFilterConfigPage()
           << new ShaderPage()
              ;

    foreach (ConfigPageBase* page, mPages) {
        page->applyToUi();
        page->applyOnUiChange(false);
        mpContent->addTab(page, page->name());
    }

    SetCenterWidget(mpCenterContainer);

    SetFixed(true);
}

void ConfigDialog::onReset()
{
    Config::instance().reset();
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->reset();
    }
}

void ConfigDialog::Slot_CloseClicked()
{
    onCancel();
}

void ConfigDialog::Slot_MouseDoubleClicked()
{
    //忽略标题栏双击事件，设置窗口不需要这个功能
}

void ConfigDialog::onApply()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->apply();
    }
    Config::instance().save();
    close();
}

void ConfigDialog::onCancel()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->cancel();
    }
    close();
}


ConfigBottom::ConfigBottom(QWidget *parent) : BaseWidget(parent)
{
    QPushButton *mpResetButton = new QPushButton(this);
    mpResetButton->setText(tr("Reset"));
    connect(mpResetButton, SIGNAL(clicked()), this, SIGNAL(onResetButtonClicked()));
    QPushButton *mpOKButton = new QPushButton(this);
    mpOKButton->setText(tr("OK"));
    connect(mpOKButton, SIGNAL(clicked()), this, SIGNAL(onOKButtonClicked()));
    QPushButton *mpCancelButton = new QPushButton(this);
    mpCancelButton->setText(tr("Cancel"));
    connect(mpCancelButton, SIGNAL(clicked()), this, SIGNAL(onCancelButtonClicked()));
    QPushButton *mpApplyButton = new QPushButton(this);
    mpApplyButton->setText(tr("Apply"));
    connect(mpApplyButton, SIGNAL(clicked()), this, SIGNAL(onApplyButtonClicked()));
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(this);
    pHBoxLayout->setContentsMargins(10,0,10,0);
    pHBoxLayout->setSpacing(10);
    pHBoxLayout->addWidget(mpResetButton);
    pHBoxLayout->addStretch();
    pHBoxLayout->addWidget(mpOKButton);
    pHBoxLayout->addWidget(mpCancelButton);
    pHBoxLayout->addWidget(mpApplyButton);
}

ConfigBottom::~ConfigBottom()
{

}
