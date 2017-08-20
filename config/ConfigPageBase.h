#pragma once

#include <QWidget>

class ConfigPageBase : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigPageBase(QWidget *parent = 0);
    virtual QString name() const = 0;
    void applyOnUiChange(bool a);
    // default is true. in dialog is false, must call ConfigDialog::apply() to apply
    bool applyOnUiChange() const;

public slots:
    // deprecated. call applyFromUi()
    void apply();
    // deprecated. call applyToUi().
    void cancel();
    //call applyToUi() after Config::instance().reset();
    void reset();
    /*!
     * \brief applyToUi
     * Apply configurations to UI. Call this in your page ctor when ui layout is ready.
     */
    virtual void applyToUi() = 0;

protected:
    /*!
     * \brief applyFromUi
     * Save configuration values from UI. Call Config::xxx(value) in your implementation
     */
    virtual void applyFromUi() = 0;

private:
    bool mApplyOnUiChange;
};
