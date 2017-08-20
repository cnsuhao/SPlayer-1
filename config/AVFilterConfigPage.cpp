#include "AVFilterConfigPage.h"
#include <QLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QtAV/LibAVFilter.h>
#include "common/Config.h"

AVFilterConfigPage::AVFilterConfigPage(QWidget *parent)
    : ConfigPageBase(parent)
{
    setObjectName(QString::fromLatin1("avfilter"));
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r = 0;
    m_ui[0].type = tr("Video");
    m_ui[1].type = tr("Audio");
    const int mw = 300;
    for (size_t i = 0; i < sizeof(m_ui)/sizeof(m_ui[0]); ++i) {
        m_ui[i].enable = new QCheckBox(tr("Enable") + QString::fromLatin1(" ") + m_ui[i].type);
        gl->addWidget(m_ui[i].enable, r++, 0);
        m_ui[i].name = new QComboBox();
        m_ui[i].name->setToolTip(QString::fromLatin1("%1 %2 %3").arg(tr("Registered")).arg(m_ui[i].type).arg(tr("filters")));
        gl->addWidget(m_ui[i].name, r++, 0);
        m_ui[i].description = new QLabel();
        m_ui[i].description->setMaximumWidth(mw);
        gl->addWidget(m_ui[i].description, r++, 0);
        gl->addWidget(new QLabel(tr("Parameters")), r++, 0);
        m_ui[i].options = new QTextEdit();
        m_ui[i].options->setMaximumWidth(mw);
        m_ui[i].options->setMaximumHeight(mw/6);
        gl->addWidget(m_ui[i].options, r++, 0);
    }
    m_ui[0].options->setToolTip(QString::fromLatin1("example: negate"));
    m_ui[1].options->setToolTip(QString::fromLatin1("example: volume=volume=2.0"));
    connect(m_ui[0].name, SIGNAL(currentIndexChanged(QString)), SLOT(videoFilterChanged(QString)));
    m_ui[0].name->addItems(QtAV::LibAVFilter::videoFilters());
    connect(m_ui[1].name, SIGNAL(currentIndexChanged(QString)), SLOT(audioFilterChanged(QString)));
    m_ui[1].name->addItems(QtAV::LibAVFilter::audioFilters());
}

QString AVFilterConfigPage::name() const
{
    return QString::fromLatin1("AVFilter");
}


void AVFilterConfigPage::applyFromUi()
{
    Config::instance()
            .avfilterVideoOptions(m_ui[0].options->toPlainText())
            .avfilterVideoEnable(m_ui[0].enable->isChecked())
            .avfilterAudioOptions(m_ui[1].options->toPlainText())
            .avfilterAudioEnable(m_ui[1].enable->isChecked())
            ;
}

void AVFilterConfigPage::applyToUi()
{
    m_ui[0].enable->setChecked(Config::instance().avfilterVideoEnable());
    m_ui[0].options->setText(Config::instance().avfilterVideoOptions());
    m_ui[1].enable->setChecked(Config::instance().avfilterAudioEnable());
    m_ui[1].options->setText(Config::instance().avfilterAudioOptions());
}

void AVFilterConfigPage::videoFilterChanged(const QString &name)
{
    m_ui[0].description->setText(QtAV::LibAVFilter::filterDescription(name));
}

void AVFilterConfigPage::audioFilterChanged(const QString &name)
{
    m_ui[1].description->setText(QtAV::LibAVFilter::filterDescription(name));
}
