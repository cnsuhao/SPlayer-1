#include "MiscPage.h"
#include <QLabel>
#include <QLayout>
#include <QCoreApplication>
#include <QThread>
#include "common/Config.h"
#include "WindowManager.h"
#include "common/Common.h"
#include "Control/SMessageBox.h"

MiscPage::MiscPage()
{
    QGridLayout *gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r = 0;
    m_preview_on = new QCheckBox(tr("Preview"));
    gl->addWidget(m_preview_on, r++, 0);
    m_preview_w = new QSpinBox();
    m_preview_w->setRange(1, 1920);
    m_preview_h = new QSpinBox();
    m_preview_h->setRange(1, 1080);
    gl->addWidget(new QLabel(QString::fromLatin1("%1 %2: ").arg(tr("Preview")).arg(tr("size"))), r, 0);
    QHBoxLayout *hb = new QHBoxLayout();
    hb->addWidget(m_preview_w);
    hb->addWidget(new QLabel(QString::fromLatin1("x")));
    hb->addWidget(m_preview_h);
    gl->addLayout(hb, r, 1);
    r++;
    gl->addWidget(new QLabel(tr("Force fps")), r, 0);
    m_fps = new QDoubleSpinBox();
    m_fps->setMinimum(-m_fps->maximum());
    m_fps->setToolTip(QString::fromLatin1("<= 0 : ") + tr("Ignore"));
    gl->addWidget(m_fps, r++, 1);

    gl->addWidget(new QLabel(tr("Progress update interval") + QString::fromLatin1("(ms)")), r, 0);
    m_notify_interval = new QSpinBox();
    m_notify_interval->setEnabled(false);
    gl->addWidget(m_notify_interval, r++, 1);

    gl->addWidget(new QLabel(tr("Buffer frames")), r, 0);
    m_buffer_value = new QSpinBox();
    m_buffer_value->setRange(-1, 32767);
    m_buffer_value->setToolTip(QString::fromLatin1("-1 : auto"));
    gl->addWidget(m_buffer_value, r++, 1);

    gl->addWidget(new QLabel(QString::fromLatin1("%1(%2)").arg(tr("Timeout")).arg(tr("s"))), r, 0);
    m_timeout = new QDoubleSpinBox();
    m_timeout->setDecimals(3);
    m_timeout->setSingleStep(1.0);
    m_timeout->setMinimum(-0.5);
    m_timeout->setToolTip(QString::fromLatin1("<= 0 : never"));
    m_timeout_abort = new QCheckBox(tr("Abort"));
    hb = new QHBoxLayout();
    hb->addWidget(m_timeout);
    hb->addWidget(m_timeout_abort);
    gl->addLayout(hb, r++, 1);

    gl->addWidget(new QLabel(tr("OpenGL type")), r, 0);
    m_opengl = new QComboBox();
    m_opengl->addItem(QString::fromLatin1("Auto"), Config::Auto);
    m_opengl->addItem(QString::fromLatin1("Desktop"), Config::Desktop);
    m_opengl->addItem(QString::fromLatin1("OpenGLES"), Config::OpenGLES);
    m_opengl->addItem(QString::fromLatin1("Software"), Config::Software);
    m_opengl->setToolTip(tr("Windows only") + " Qt >= 5.4 + Dynamic GL" + QString::fromLatin1("\n") + tr("OpenGLES is Used by DXVA Zero Copy"));
    gl->addWidget(m_opengl, r, 1);
    m_angle_platform = new QComboBox();
    m_angle_platform->setToolTip(tr("D3D9 has performance if ZeroCopy is disabled or for software decoders") + QString::fromLatin1("\n") + tr("RESTART REQUIRED"));
    m_angle_platform->addItems(QStringList() << QString::fromLatin1("D3D9") << QString::fromLatin1("D3D11") << QString::fromLatin1("AUTO") << QString::fromLatin1("WARP"));
#ifndef QT_OPENGL_DYNAMIC
    m_opengl->setEnabled(false);
    m_angle_platform->setEnabled(false);
#endif
    gl->addWidget(m_angle_platform, r++, 2);

    gl->addWidget(new QLabel("EGL"), r, 0);
    m_egl = new QCheckBox();
    m_egl->setToolTip(tr("Currently only works for Qt >= 5.5 XCB build"));
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0) || !defined(Q_OS_LINUX)
    m_egl->setEnabled(false);
#endif
    gl->addWidget(m_egl, r++, 1);

    m_save_history = new QCheckBox();
    m_save_history->setText(tr("Save history"));
    gl->addWidget(m_save_history, r, 0);
    m_clean_history = new QPushButton();
    m_clean_history->setText(tr("Clean history"));
    gl->addWidget(m_clean_history, r++, 1);
    connect(m_clean_history, SIGNAL(clicked()), this, SLOT(cleanHistory()));

    gl->addWidget(new QLabel(tr("Language")), r, 0);
    m_ui_language = new QComboBox();
    m_ui_language->addItems(QStringList() << QString::fromLatin1("system")
            << QString::fromLatin1("en_US") << QString::fromLatin1("zh_CN")
            << QString::fromLatin1("zh_TW"));
    m_ui_language->setToolTip(tr("Restart the player to make it go into effect"));
    gl->addWidget(m_ui_language, r++, 1);

    gl->addWidget(new QLabel(tr("Skin")), r, 0);
    m_ui_skin = new QComboBox();
    m_ui_skin->addItems(QStringList() << QString::fromLatin1("Silvery")
                        << QString::fromLatin1("Blue")
                        << QString::fromLatin1("Light Blue")
                        << QString::fromLatin1("Dark Blue")
                        << QString::fromLatin1("Gray")
                        << QString::fromLatin1("Light Gray")
                        << QString::fromLatin1("Dark Gray")
                        << QString::fromLatin1("Black")
                        << QString::fromLatin1("Light Black")
                        << QString::fromLatin1("Dark Black")
                        << QString::fromLatin1("PS Black")
                        << QString::fromLatin1("Flat Black")
                        << QString::fromLatin1("Flat White"));
    gl->addWidget(m_ui_skin, r++, 1);

    m_always_check_assoc = new QCheckBox(tr("Always try to check file type associations"));
    gl->addWidget(m_always_check_assoc, r, 0);

    m_toggle_assoc_state = new QPushButton(tr("Associate video file types"));
    m_toggle_assoc_state->setToolTip(tr("SPlayer supports both video and audio files, but only associate video files."));
    gl->addWidget(m_toggle_assoc_state, r++, 1);
    connect(m_toggle_assoc_state, SIGNAL(clicked()), this, SLOT(toggleAssociteState()));

    m_osd_always_show_local_time = new QCheckBox();
    m_osd_always_show_local_time->setText(tr("Always show local time on screen"));
    gl->addWidget(m_osd_always_show_local_time, r++, 0);
}

QString MiscPage::name() const
{
    return tr("Misc");
}


void MiscPage::applyFromUi()
{
    Config::instance().setPreviewEnabled(m_preview_on->isChecked())
            .setPreviewWidth(m_preview_w->value())
            .setPreviewHeight(m_preview_h->value())
            .setEGL(m_egl->isChecked())
            .setOpenGLType((Config::OpenGLType)m_opengl->itemData(m_opengl->currentIndex()).toInt())
            .setANGLEPlatform(m_angle_platform->currentText().toLower())
            .setForceFrameRate(m_fps->value())
            .setBufferValue(m_buffer_value->value())
            .setTimeout(m_timeout->value())
            .setAbortOnTimeout(m_timeout_abort->isChecked())
            .setSaveHistoryState(m_save_history->isChecked())
            .setLanguage(m_ui_language->currentText())
            .setSkin(m_ui_skin->currentText())
            .setIsAlwaysCheckAssoc(m_always_check_assoc->isChecked())
            .setShowLocalTime(m_osd_always_show_local_time->isChecked());
}

void MiscPage::toggleAssociteState()
{
    QString regParam;
    if (Common::isFileTypesAssociated())
    {
        regParam = QString::fromLatin1("--unassoc");
    }
    else
    {
        regParam = QString::fromLatin1("--assoc");
    }
    QString filePath = QCoreApplication::applicationFilePath();
    if (!Common::executeProgramWithAdministratorPrivilege(filePath, regParam))
    {
        SMessageBox::error(nullptr, tr("Error"), tr("Cannot change file associations."));
    }
    QThread::msleep(500);
    if (Common::isFileTypesAssociated())
    {
        m_toggle_assoc_state->setText(tr("Unassociate video file types"));
    }
    else
    {
        m_toggle_assoc_state->setText(tr("Associate video file types"));
    }
}

void MiscPage::cleanHistory()
{
    if (!WindowManager::Instance()->GetMainWindow())
    {
        return;
    }
    WindowManager::Instance()->GetMainWindow()->cleanHistory();
    SMessageBox::information(nullptr, tr("History deleted")
                  , tr("Player has successfully deleted play history, if there is any."));
}

void MiscPage::applyToUi()
{
    m_preview_on->setChecked(Config::instance().previewEnabled());
    m_preview_w->setValue(Config::instance().previewWidth());
    m_preview_h->setValue(Config::instance().previewHeight());
    m_opengl->setCurrentIndex(m_opengl->findData(Config::instance().openGLType()));
    m_angle_platform->setCurrentIndex(m_angle_platform->findText(Config::instance().getANGLEPlatform().toUpper()));
    m_egl->setChecked(Config::instance().isEGL());
    m_fps->setValue(Config::instance().forceFrameRate());
    //m_notify_interval->setValue(Config::instance().avfilterOptions());
    m_buffer_value->setValue(Config::instance().bufferValue());
    m_timeout->setValue(Config::instance().timeout());
    m_timeout_abort->setChecked(Config::instance().abortOnTimeout());
    m_save_history->setChecked(Config::instance().getSaveHistoryState());
    m_ui_language->setCurrentIndex(m_ui_language->findText(Config::instance().language()));
    m_ui_skin->setCurrentIndex(m_ui_skin->findText(Config::instance().getSkin()));
    m_always_check_assoc->setChecked(Config::instance().getIsAlwaysCheckAssoc());
    if (Common::isFileTypesAssociated())
    {
        m_toggle_assoc_state->setText(tr("Unassociate video file types"));
    }
    else
    {
        m_toggle_assoc_state->setText(tr("Associate video file types"));
    }
    m_osd_always_show_local_time->setChecked(Config::instance().getShowLocalTime());
}
