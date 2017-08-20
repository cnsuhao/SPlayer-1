#include "ConfigPageBase.h"

ConfigPageBase::ConfigPageBase(QWidget *parent) :
    QWidget(parent)
  , mApplyOnUiChange(true)
{
}

void ConfigPageBase::applyOnUiChange(bool a)
{
    mApplyOnUiChange = a;
}

bool ConfigPageBase::applyOnUiChange() const
{
    return mApplyOnUiChange;
}

void ConfigPageBase::apply()
{
    applyFromUi();
}

void ConfigPageBase::cancel()
{
    applyToUi();
}

void ConfigPageBase::reset()
{
    // NOTE: make sure Config::instance().reset() is called before it. It is called i ConfigDialog.reset()
    applyToUi();
}
