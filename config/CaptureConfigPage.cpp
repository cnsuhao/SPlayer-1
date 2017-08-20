#include "CaptureConfigPage.h"
#include "common/Config.h"
#include <QLabel>
#include <QFormLayout>
#include <QImageWriter>
#include <QToolButton>
#include <QDesktopServices>
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>
#include "../Slider.h"

CaptureConfigPage::CaptureConfigPage(QWidget *parent) : ConfigPageBase(parent)
{
    QFormLayout *formLayout = new QFormLayout();
    setLayout(formLayout);

    QHBoxLayout *hb = new QHBoxLayout();

    mpDir = new QLineEdit();
    hb->addWidget(mpDir);

    QToolButton *bt = new QToolButton();
    bt->setText(tr("Browse"));
    bt->setToolTip(tr("Change video capture picture save directory"));
    hb->addWidget(bt);

    connect(bt, SIGNAL(clicked()), SLOT(selectSaveDir()));

    QToolButton *bt2 = new QToolButton();
    bt2->setText(tr("Open"));
    bt2->setToolTip(tr("Open video capture picture save directory"));
    hb->addWidget(bt2);
    connect(bt2, SIGNAL(clicked()), SLOT(openCaptureDir()));

    formLayout->addRow(tr("Save dir"), hb);
    mpDir->setEnabled(false);
    mpFormat = new QComboBox();
    formLayout->addRow(tr("Save format"), mpFormat);
    QList<QByteArray> formats;
    formats << "Original" << QImageWriter::supportedImageFormats();
    foreach (const QByteArray& fmt, formats) {
        mpFormat->addItem(QString::fromLatin1(fmt));
    }
    mpQuality = new Slider();
    formLayout->addRow(tr("Quality"), mpQuality);
    mpQuality->setRange(0, 100);
    mpQuality->setOrientation(Qt::Horizontal);
    mpQuality->setSingleStep(1);
    mpQuality->setTickInterval(10);
    mpQuality->setTickPosition(QSlider::TicksBelow);

    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)), mpDir, SLOT(setText(QString)));
    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)), mpQuality, SLOT(setValue(int)));
    connect(mpDir, SIGNAL(textChanged(QString)), SLOT(changeDirByUi(QString)));
    connect(mpFormat, SIGNAL(currentIndexChanged(QString)), SLOT(changeFormatByUi(QString)));
    connect(mpQuality, SIGNAL(valueChanged(int)), SLOT(changeQualityByUi(int)));
}

QString CaptureConfigPage::name() const
{
    return tr("Capture");
}

void CaptureConfigPage::applyFromUi()
{
    Config::instance().setCaptureDir(mpDir->text())
            .setCaptureFormat(mpFormat->currentText())
            .setCaptureQuality(mpQuality->value());
}

void CaptureConfigPage::applyToUi()
{
    mpDir->setText(Config::instance().captureDir());
    int idx = mpFormat->findText(Config::instance().captureFormat());
    if (idx >= 0)
        mpFormat->setCurrentIndex(idx);
    mpQuality->setValue(Config::instance().captureQuality());
}

void CaptureConfigPage::selectSaveDir()
{
    QString dir = QFileDialog::getExistingDirectory(0, tr("Save dir"), mpDir->text());
    if (dir.isEmpty())
        return;
    mpDir->setText(dir);
}

void CaptureConfigPage::openCaptureDir()
{
    qDebug("browse capture dir");
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("file:///") + mpDir->text()));
}

void CaptureConfigPage::changeDirByUi(QString dir)
{
    QFileInfo fileInfo(dir);
    if (fileInfo.exists() && fileInfo.isDir())
    {
        Config::instance().setCaptureDir(dir);
    }
}

void CaptureConfigPage::changeFormatByUi(QString format)
{
    Config::instance().setCaptureFormat(format);
}

void CaptureConfigPage::changeQualityByUi(int quality)
{
    if (quality >= -1 && quality <= 100)
    {
        Config::instance().setCaptureQuality(quality);
    }
    else
    {
        Config::instance().setCaptureQuality(100);
    }
}
