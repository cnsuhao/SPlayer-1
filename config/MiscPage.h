#pragma once

#include "ConfigPageBase.h"
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>

class MiscPage : public ConfigPageBase
{
    Q_OBJECT

public:
    MiscPage();
    virtual QString name() const;

protected:
    virtual void applyToUi();
    virtual void applyFromUi();

private slots:
    void toggleAssociteState();
    void cleanHistory();

private:
    QCheckBox *m_preview_on;
    QSpinBox *m_preview_w;
    QSpinBox *m_preview_h;
    QSpinBox *m_notify_interval;
    QDoubleSpinBox *m_fps;
    QSpinBox *m_buffer_value;
    QDoubleSpinBox *m_timeout;
    QCheckBox *m_timeout_abort;
    QComboBox *m_opengl;
    QComboBox *m_angle_platform;
    QCheckBox *m_egl;

    QCheckBox *m_save_history;
    QPushButton *m_clean_history;
    QComboBox *m_ui_language;
    QComboBox *m_ui_skin;
    QCheckBox *m_always_check_assoc;
    QPushButton *m_toggle_assoc_state;
    QCheckBox *m_osd_always_show_local_time;
};
