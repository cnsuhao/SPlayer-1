#pragma once

#include "ConfigPageBase.h"
#include <QVariant>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE
class AVFormatConfigPage : public ConfigPageBase
{
    Q_OBJECT
public:
    explicit AVFormatConfigPage(QWidget *parent = 0);
    virtual QString name() const;
protected:
    virtual void applyToUi();
    virtual void applyFromUi();
private:
    QCheckBox* m_on;
    QCheckBox *m_direct;
    QSpinBox *m_probeSize;
    QSpinBox *m_analyzeDuration;
    QLineEdit *m_extra;
};
