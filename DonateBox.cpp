#include "DonateBox.h"
#include <QLabel>
#include <QVBoxLayout>
#include "Control/BaseWidget.h"

DonateBox::DonateBox(QWidget *parent) : StandardDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    GetHeader()->SetTitleText(tr("Donate"));
    setWindowTitle(tr("Donate"));
    GetHeader()->SetTitleCenter(false);
    GetHeader()->SetLogoVisible(false);
    GetHeader()->SetSkinVisible(false);
    GetHeader()->SetMinVisible(false);
    GetHeader()->SetMaxVisible(false);
    GetHeader()->SetStayOnTopVisible(false);
    GetHeader()->setFixedHeight(30);
    GetBottom()->setFixedHeight(0);
    GetBottom()->hide();

    BaseWidget *mpCenterContainer = new BaseWidget(this);
    QLabel *mQCodePicture = new QLabel(mpCenterContainer);
    mQCodePicture->setScaledContents(true);
    mQCodePicture->setPixmap(QPixmap(":/Image/DonateQCode.png"));
    mQCodePicture->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *m_pVLayout = new QVBoxLayout(mpCenterContainer);
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    m_pVLayout->addWidget(mQCodePicture);
    mpCenterContainer->setLayout(m_pVLayout);
    SetCenterWidget(mpCenterContainer);
}
