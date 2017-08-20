#pragma once

#include <QVariant>
#include <QWidget>
#include "ConfigPageBase.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QToolButton;
class QSpinBox;
class QVBoxLayout;
QT_END_NAMESPACE
class DecoderConfigPage : public ConfigPageBase
{
    Q_OBJECT
    class DecoderItemWidget;
public:
    explicit DecoderConfigPage(QWidget *parent = 0);
    virtual QString name() const;
    QVariantHash audioDecoderOptions() const;
    QVariantHash videoDecoderOptions() const;

protected:
    virtual void applyToUi();
    virtual void applyFromUi();

private slots:
    void videoDecoderEnableChanged();
    void priorityUp();
    void priorityDown();
    void onDecSelected(DecoderItemWidget* iw);
    void updateDecodersUi();
    void onConfigChanged();

private:
    QSpinBox *mpThreads;
    QToolButton *mpUp, *mpDown;
    QList<DecoderItemWidget*> mDecItems;
    DecoderItemWidget *mpSelectedDec;
    QVBoxLayout *mpDecLayout;
};
