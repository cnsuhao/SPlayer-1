#pragma once

#include <QWidget>
#include "StandardDialog/StandardDialog.h"

class DonateBox : public StandardDialog
{
    Q_OBJECT

public:
    explicit DonateBox(QWidget *parent = nullptr);
};
