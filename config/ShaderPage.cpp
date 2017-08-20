#include "ShaderPage.h"
#include <QLabel>
#include <QLayout>
#include "common/Config.h"

ShaderPage::ShaderPage(QWidget *parent)
    : ConfigPageBase(parent)
{
    QVBoxLayout *gl = new QVBoxLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetMaximumSize);
    const int mw = 600;
    m_enable = new QCheckBox(tr("Enable"));
    gl->addWidget(m_enable);
    m_fbo = new QCheckBox(tr("Intermediate FBO"));
    gl->addWidget(m_fbo);
    gl->addWidget(new QLabel(tr("Fragment shader header")));
    m_header = new QTextEdit();
    //m_header->setMaximumWidth(mw);
    m_header->setMaximumHeight(mw/6);
    m_header->setToolTip(tr("Additional header code"));
    gl->addWidget(m_header);
    gl->addWidget(new QLabel(tr("Fragment shader texel sample function")));
    m_sample = new QTextEdit();
    //m_sample->setMaximumWidth(mw);
    m_sample->setMaximumHeight(mw/6);
    m_sample->setToolTip(QLatin1String("vec4 sample2d(sampler2D tex, vec2 pos, int p)"));
    gl->addWidget(m_sample);
    gl->addWidget(new QLabel(QLatin1String("Fragment shader RGB post process code")));
    m_pp = new QTextEdit();
    //m_pp->setMaximumWidth(mw);
    m_pp->setMaximumHeight(mw/6);
    gl->addWidget(m_pp);
    gl->addStretch();
}

QString ShaderPage::name() const
{
    return tr("Shader");
}

void ShaderPage::applyToUi()
{
    m_enable->setChecked(Config::instance().userShaderEnabled());
    m_fbo->setChecked(Config::instance().intermediateFBO());
    m_header->setText(Config::instance().fragHeader());
    m_sample->setText(Config::instance().fragSample());
    m_pp->setText(Config::instance().fragPostProcess());
}

void ShaderPage::applyFromUi()
{
    Config::instance()
            .setUserShaderEnabled(m_enable->isChecked())
            .setIntermediateFBO(m_fbo->isChecked())
            .setFragHeader(m_header->toPlainText())
            .setFragSample(m_sample->toPlainText())
            .setFragPostProcess(m_pp->toPlainText())
            ;
}
