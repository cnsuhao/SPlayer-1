#pragma once

#include "ConfigPageBase.h"
#include <QTextEdit>
#include <QCheckBox>

class ShaderPage : public ConfigPageBase
{
    Q_OBJECT

public:
    ShaderPage(QWidget* parent = nullptr);
    virtual QString name() const;

protected:
    virtual void applyToUi();
    virtual void applyFromUi();

private:
    QCheckBox *m_enable;
    QCheckBox *m_fbo;
    QTextEdit *m_header;
    QTextEdit *m_sample;
    QTextEdit *m_pp;
};
