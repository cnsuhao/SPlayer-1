#pragma once

#include <QTabWidget>
#include <QList>
#include "Control/BaseWidget.h"
#include "StandardDialog/StandardDialog.h"

class ConfigPageBase;

class ConfigBottom : public BaseWidget
{
    Q_OBJECT

public:
    ConfigBottom(QWidget *parent);
    virtual ~ConfigBottom();

signals:
    void onResetButtonClicked();
    void onOKButtonClicked();
    void onCancelButtonClicked();
    void onApplyButtonClicked();
};

class ConfigDialog : public StandardDialog
{
    Q_OBJECT

public:
    //static void display();
    explicit ConfigDialog(QWidget *parent = nullptr);

private slots:
    void onApply();
    void onCancel();
    void onReset();

protected:
    void Slot_CloseClicked();
    void Slot_MouseDoubleClicked();

private:
    //explicit ConfigDialog(QWidget *parent = nullptr);
    QTabWidget *mpContent;
    QList<ConfigPageBase*> mPages;
    ConfigBottom *mpConfigBottom;
};
