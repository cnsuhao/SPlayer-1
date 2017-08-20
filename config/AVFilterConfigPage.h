#pragma once

#include "ConfigPageBase.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE
class AVFilterConfigPage : public ConfigPageBase
{
    Q_OBJECT
public:
    AVFilterConfigPage(QWidget* parent = 0);
    virtual QString name() const;
protected:
    virtual void applyToUi();
    virtual void applyFromUi();
private Q_SLOTS:
    void audioFilterChanged(const QString& name);
    void videoFilterChanged(const QString& name);
private:
    struct {
        QString type;
        QCheckBox *enable;
        QComboBox *name;
        QLabel *description;
        QTextEdit *options;
    } m_ui[2]; //0: video, 1: audio
};
