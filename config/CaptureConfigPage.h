#pragma once

#include "ConfigPageBase.h"
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

/*
 * TODO: ConfigPageBase: auto save(true for menu ui, false for dialog ui)
 * virtual public slot: apply()
 */

class Slider;

class CaptureConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:
    explicit CaptureConfigPage(QWidget *parent = nullptr);
    virtual QString name() const;

protected:
    virtual void applyToUi();
    virtual void applyFromUi();

private slots:
    // only emit signals. no value stores.
    void selectSaveDir();
    void openCaptureDir();

    void changeDirByUi(QString dir);
    void changeFormatByUi(QString format);
    void changeQualityByUi(int quality);

private:
    QLineEdit *mpDir;
    QComboBox *mpFormat;
    Slider *mpQuality;
};
