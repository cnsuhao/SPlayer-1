#include "MainWindow.h"
#include <QtAV>
#include <QtAVWidgets>
#include <QtDebug>
#include <QTimer>
#include <QTime>
#include <QTimeEdit>
#include <QLabel>
#include <QCursor>
#include <QDesktopWidget>
#include <QtWinExtras>
#include <QFileInfo>
#include <QComboBox>
#include <QWidgetAction>
#include <QLayout>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QToolTip>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeDatabase>
#include <QSettings>
#include <QCoreApplication>
#include <QSizePolicy>
#include <QPixmap>
#include <QDir>
#include <QTextBrowser>
#include <QTabWidget>
#include <QPushButton>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QtWin>
#include <QTextStream>
#include "common/Common.h"
#include "ClickableMenu.h"
#include "Slider.h"
#include "Style/SStyle.h"
#include "StatisticsView.h"
#include "config/DecoderConfigPage.h"
#include "config/VideoEQConfigPage.h"
#include "filters/OSDFilter.h"
#include "filters/OSD.h"
#include "Control/SMessageBox.h"
//#include "filters/AVFilterSubtitle.h"
#include "playlist/PlayList.h"
#include "common/Config.h"
#include "common/ScreenSaver.h"
#ifndef QT_NO_OPENGL
#include "QtAV/GLSLFilter.h"
#endif
/*
 *TODO:
 * disable a/v actions if player is 0;
 * use action's value to set player's parameters when start to play a new file
 */


#define AVDEBUG() \
    qDebug("%s %s @%d", __FILE__, __FUNCTION__, __LINE__);

using namespace QtAV;

extern QStringList idsToNames(QVector<VideoDecoderId> ids);
extern QVector<VideoDecoderId> idsFromNames(const QStringList& names);

static const qreal kVolumeInterval = 0.04;
static const int kVolumeSliderMax = 100;

static bool canShowPreviewWindow = false;

static QPoint mCurrentCursorPosition = QPoint();
static int mTimeSliderHoverValue = 0;

MainWindow::MainWindow(QWidget *parent) : StandardDialog(parent)
  , mIsReady(false)
  , mHasPendingPlay(false)
  , mCursorTimer(0)
  , mOSDTimer(0)
  , mTimeSliderHoverTimer(0)
  , mTimeSliderLeaveTimer(0)
  , mRepeateMax(0)
  , mbStayOnTop(false)
  , mpPlayerLayout(NULL)
  , mpTimeSlider(NULL)
  , mpMenu(NULL)
  , mpVOAction(NULL)
  , mpARAction(NULL)
  , mpRepeatEnableAction(NULL)
  , mpRepeatAction(NULL)
  , mpRepeatBox(NULL)
  , mpRepeatA(NULL)
  , mpRepeatB(NULL)
  , mpAudioTrackMenu(NULL)
  , mpPlayer(NULL)
  , mpRenderer(NULL)
  , mpVideoFilter(NULL)
  , mpAudioFilter(NULL)
  , mpDecoderConfigPage(NULL)
  , mpVideoEQ(NULL)
  , mpPlayList(NULL)
  , mpHistory(NULL)
  , mpStatisticsView(NULL)
  , mpOSD(NULL)
  , mpSubtitle(NULL)
  , m_preview(NULL)
  , m_shader(NULL)
  , mpChannelAction(NULL)
  , mpChannelMenu(NULL)
  , mpAudioTrackAction(NULL)
  , m_pWindowBottom(NULL)
  , mpLogoMenu(NULL)
  , mpSkinManagerMenu(NULL)
  , mpLogoWidget(NULL)
  , mpTaskbarButton(NULL)
  , mpTaskbarProgress(NULL)
  , mpThumbnailToolBar(NULL)
  , mpPlayToolButton(NULL)
  , mpStopToolButton(NULL)
  , mpNextToolButton(NULL)
  , mpPreviousToolButton(NULL)
  , mpMuteToolButton(NULL)
  , mpFullscreenControlPanel(NULL)
  , mpFullScreenProgressBar(NULL)
  , mpTrayIcon(NULL)
  , mpConfigDialog(NULL)
  , mpAboutBox(NULL)
  , mpHelpBox(NULL)
  , mpDonateBox(NULL)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    GetHeader()->SetTitleText(QString::fromLatin1(AppName));
    GetHeader()->SetTitleCenter(true);
    GetHeader()->setFixedHeight(30);
    GetBottom()->setFixedHeight(65);

    m_pWindowBottom = new WindowBottom(this);
    SetBottomWidget(m_pWindowBottom);
    m_pWindowBottom->getVolumeSlider()->setValue(Config::instance().getLastVolume());

    setMinimumSize(600, GetHeader()->height() + GetBottom()->height() + 305);

    mpOSD = new OSDFilter(this);
    mpSubtitle = new SubtitleFilter(this);

    setupUi();

    mpConfigDialog = new ConfigDialog();
    mpConfigDialog->hide();

    mpAboutBox = new AboutBox();
    connect(mpAboutBox, SIGNAL(onAboutQtAV()), this, SLOT(aboutQtAV()));
    mpAboutBox->hide();

    mpHelpBox = new HelpBox();
    mpHelpBox->hide();

    mpDonateBox = new DonateBox();
    mpDonateBox->hide();

    createFullScreenProgressBar();

    createSkinManagerMenu();
    createLogoMenu();
    createShortcuts();

    setMouseTracking(true);
    setAcceptDrops(true);

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        createTrayIcon();
        connect(mpTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                SLOT(show()));
    }

    if (QtWin::isCompositionEnabled())
    {
        QtWin::enableBlurBehindWindow(this);
        createJumpList();
        createTaskbar();
        createThumbnailToolBar();
    }
    else
    {
        QtWin::disableBlurBehindWindow(this);
    }

    connect(this, SIGNAL(ready()), this, SLOT(processPendingActions()));
}

MainWindow::~MainWindow()
{
    if (mpConfigDialog)
    {
        mpConfigDialog->close();
        delete mpConfigDialog;
        mpConfigDialog = NULL;
    }
    if (m_preview)
    {
        m_preview->close();
        delete m_preview;
        m_preview = NULL;
    }
    if (mpStatisticsView)
    {
        mpStatisticsView->close();
        delete mpStatisticsView;
        mpStatisticsView = NULL;
    }
    if (mpLogoWidget)
    {
        mpLogoWidget->close();
        delete mpLogoWidget;
        mpLogoWidget = NULL;
    }
    if (mpFullScreenProgressBar)
    {
        mpFullScreenProgressBar->close();
        delete mpFullScreenProgressBar;
        mpFullScreenProgressBar = NULL;
    }
    if (mpFullscreenControlPanel)
    {
        mpFullscreenControlPanel->close();
        delete mpFullscreenControlPanel;
        mpFullscreenControlPanel = NULL;
    }
    if (mpAboutBox)
    {
        mpAboutBox->close();
        delete mpAboutBox;
        mpAboutBox = NULL;
    }
    if (mpHelpBox)
    {
        mpHelpBox->close();
        delete mpHelpBox;
        mpHelpBox = NULL;
    }
    if (mpDonateBox)
    {
        mpDonateBox->close();
        delete mpDonateBox;
        mpDonateBox = NULL;
    }
}

void MainWindow::initPlayer()
{
    mpPlayer = new AVPlayer(this);
    mIsReady = true;
    VideoRenderer *vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());
    if (!vo || !vo->isAvailable() || !vo->widget()) {
        SMessageBox::error(nullptr, QString::fromLatin1("QtAV"), tr("Video renderer is ") + tr("not availabe on your platform!"));
    }
    setRenderer(vo);
    //mpSubtitle->installTo(mpPlayer); //filter on frame
    mpSubtitle->setPlayer(mpPlayer);
    mpOSD->setAVPlayer(mpPlayer);
    //mpPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
    installEventFilter(this);
    onCaptureConfigChanged();
    onAVFilterVideoConfigChanged();
    onAVFilterAudioConfigChanged();
    connect(&Config::instance(), SIGNAL(forceFrameRateChanged()), this
            , SLOT(setFrameRate()));
    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)), this
            , SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureFormatChanged(QString)), this
            , SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)), this
            , SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(avfilterVideoChanged()), this
            , SLOT(onAVFilterVideoConfigChanged()));
    connect(&Config::instance(), SIGNAL(avfilterAudioChanged()), this
            , SLOT(onAVFilterAudioConfigChanged()));
    connect(&Config::instance(), SIGNAL(bufferValueChanged()), this
            , SLOT(onBufferValueChanged()));
    connect(&Config::instance(), SIGNAL(abortOnTimeoutChanged()), this
            , SLOT(onAbortOnTimeoutChanged()));

    connect(m_pWindowBottom, SIGNAL(onStopButtonClicked()), this, SLOT(stopUnload()));
    connect(m_pWindowBottom->getVolumeSlider(), SIGNAL(sliderPressed()), this
            , SLOT(setVolume()));
    connect(m_pWindowBottom->getVolumeSlider(), SIGNAL(valueChanged(int)), this
            , SLOT(setVolume()));

    connect(mpPlayer, SIGNAL(seekFinished(qint64)), this, SLOT(onSeekFinished(qint64)));
    connect(mpPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), this
            , SLOT(onMediaStatusChanged()));
    connect(mpPlayer, SIGNAL(bufferProgressChanged(qreal)), this
            , SLOT(onBufferProgress(qreal)));
    connect(mpPlayer, SIGNAL(error(QtAV::AVError)), this
            , SLOT(handleError(QtAV::AVError)));
    connect(mpPlayer, SIGNAL(started()), this, SLOT(onStartPlay()));
    connect(mpPlayer, SIGNAL(stopped()), this, SLOT(onStopPlay()));
    connect(mpPlayer, SIGNAL(paused(bool)), this, SLOT(onPaused(bool)));
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this
            , SLOT(onPositionChange(qint64)));
    connect(mpVideoEQ, SIGNAL(brightnessChanged(int)), this
            , SLOT(onBrightnessChanged(int)));
    connect(mpVideoEQ, SIGNAL(contrastChanged(int)), this, SLOT(onContrastChanged(int)));
    connect(mpVideoEQ, SIGNAL(hueChanegd(int)), this, SLOT(onHueChanged(int)));
    connect(mpVideoEQ, SIGNAL(saturationChanged(int)), this
            , SLOT(onSaturationChanged(int)));

    connect(m_pWindowBottom, SIGNAL(onVideoCaptureButtonClicked())
            , mpPlayer->videoCapture(), SLOT(capture()));

    changeWindowSizeToVideoSize(1280, 720);
    moveWindowToDesktopCenter();

    if (Config::instance().getIsMuted())
    {
        setMute(true);
    }

    emit ready(); //emit this signal after connection. otherwise the slots may not be called for the first time
}

void MainWindow::onSeekFinished(qint64 pos)
{
    qDebug("seek finished at %lld/%lld", pos, mpPlayer->position());
}

void MainWindow::stopUnload()
{
    mpPlayer->stop();
    if (mpLogoWidget)
    {
        if (!mpLogoWidget->isVisible())
        {
            mpLogoWidget->show();
        }
    }
}

void MainWindow::setupUi()
{
    BaseWidget *pCenterContainer = new BaseWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(pCenterContainer);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    pCenterContainer->setLayout(mainLayout);

    mpPlayerLayout = new QVBoxLayout();

    mpTimeSlider = new Slider(pCenterContainer);
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setTracking(true);
    mpTimeSlider->setOrientation(Qt::Horizontal);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setFixedHeight(12);

    //setVolume();

    QMenu *subMenu = 0;
    QWidgetAction *pWA = 0;
    mpMenu = new QMenu(pCenterContainer);
    mpMenu->addAction(tr("Open file"), this, SLOT(openFile()));
    mpMenu->addAction(tr("Open URL"), this, SLOT(openUrl()));

    mpMenu->addSeparator();

    subMenu = new QMenu(tr("Play list"));
    mpMenu->addMenu(subMenu);
    mpPlayList = new PlayList(this);
    mpPlayList->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/playlist.qds"));
    //mpPlayList->load();
    connect(mpPlayList, SIGNAL(aboutToPlay(QString)), this, SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpPlayList);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new QMenu(tr("History"));
    mpMenu->addMenu(subMenu);
    mpHistory = new PlayList(this);
    mpHistory->setMaxRows(20);
    mpHistory->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/history.qds"));
    if (Config::instance().getSaveHistoryState())
    {
        mpHistory->load();
    }
    connect(mpHistory, SIGNAL(aboutToPlay(QString)), this, SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpHistory);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    mpMenu->addSeparator();

    mpMenu->addAction(tr("About"), this, SLOT(about()));
    mpMenu->addAction(tr("Help"), this, SLOT(help()));
    mpMenu->addAction(tr("Donate"), this, SLOT(donate()));
    mpMenu->addAction(tr("Setup"), this, SLOT(setup()));

    mpMenu->addSeparator();

    subMenu = new QMenu(tr("Speed"));
    mpMenu->addMenu(subMenu);
    QDoubleSpinBox *pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new ClickableMenu(tr("Repeat"));
    mpMenu->addMenu(subMenu);
    mpRepeatEnableAction = subMenu->addAction(tr("Enable"));
    mpRepeatEnableAction->setCheckable(true);
    connect(mpRepeatEnableAction, SIGNAL(toggled(bool)), this, SLOT(toggleRepeat(bool)));
    // TODO: move to a func or class
    mpRepeatBox = new QSpinBox(0);
    mpRepeatBox->setMinimum(-1);
    mpRepeatBox->setValue(-1);
    mpRepeatBox->setToolTip(QString::fromLatin1("-1: ") + tr("infinity"));
    connect(mpRepeatBox, SIGNAL(valueChanged(int)), this, SLOT(setRepeateMax(int)));
    QLabel *pRepeatLabel = new QLabel(tr("Times"));
    QHBoxLayout *hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatBox);
    QVBoxLayout *vb = new QVBoxLayout;
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("From"));
    mpRepeatA = new QTimeEdit();
    mpRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    mpRepeatA->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatA, SIGNAL(timeChanged(QTime)), this, SLOT(repeatAChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatA);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("To"));
    mpRepeatB = new QTimeEdit();
    mpRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss"));
    mpRepeatB->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatB, SIGNAL(timeChanged(QTime)), this, SLOT(repeatBChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatB);
    vb->addLayout(hb);
    QWidget *wgt = new QWidget;
    wgt->setLayout(vb);

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    mpRepeatAction = pWA;

    mpMenu->addSeparator();

    subMenu = new ClickableMenu(tr("Clock"));
    mpMenu->addMenu(subMenu);
    QActionGroup *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeClockType(QAction*)));
    subMenu->addAction(tr("Auto"))->setData(-1);
    subMenu->addAction(tr("Audio"))->setData(AVClock::AudioClock);
    subMenu->addAction(tr("Video"))->setData(AVClock::VideoClock);
    foreach(QAction* action, subMenu->actions()) {
        action->setActionGroup(ag);
        action->setCheckable(true);
    }
    QAction *autoClockAction = subMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(tr("Take effect in next playback"));

    subMenu = new ClickableMenu(tr("Subtitle"));
    mpMenu->addMenu(subMenu);
    QAction *act = subMenu->addAction(tr("Enable"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->isEnabled());
    connect(act, SIGNAL(toggled(bool)), this, SLOT(toggoleSubtitleEnabled(bool)));
    act = subMenu->addAction(tr("Auto load"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->autoLoad());
    connect(act, SIGNAL(toggled(bool)), this, SLOT(toggleSubtitleAutoLoad(bool)));
    subMenu->addAction(tr("Open"), this, SLOT(openSubtitle()));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Engine")));
    QComboBox *box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(QString::fromLatin1("LibASS"), QString::fromLatin1("LibASS"));
    box->addItem(QString::fromLatin1("FFmpeg"), QString::fromLatin1("FFmpeg"));
    connect(box, SIGNAL(activated(QString)), this, SLOT(setSubtitleEngine(QString)));
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(tr("FFmpeg supports more subtitles but only render plain text") + QString::fromLatin1("\n") + tr("LibASS supports ass styles"));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Charset")));
    box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(tr("Auto detect"), QString::fromLatin1("AutoDetect"));
    box->addItem(tr("System"), QString::fromLatin1("System"));
    foreach (const QByteArray& cs, QTextCodec::availableCodecs()) {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }
    connect(box, SIGNAL(activated(QString)), this, SLOT(setSubtitleCharset(QString)));
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(tr("Auto detect requires libchardet"));

    subMenu = new ClickableMenu(tr("Audio track"));
    mpMenu->addMenu(subMenu);
    mpAudioTrackMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeAudioTrack(QAction*)));
    initAudioTrackMenu();

    subMenu = new ClickableMenu(tr("Channel"));
    mpMenu->addMenu(subMenu);
    mpChannelMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeChannel(QAction*)));
    subMenu->addAction(tr("As input"))->setData(AudioFormat::ChannelLayout_Unsupported); //will set to input in resampler if not supported.
    subMenu->addAction(tr("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(tr("Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(tr("Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(tr("Right"))->setData(AudioFormat::ChannelLayout_Right);
    ag = new QActionGroup(subMenu);
    ag->setExclusive(true);
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu = new QMenu(tr("Aspect ratio"), mpMenu);
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(switchAspectRatio(QAction*)));
    mpARAction = subMenu->addAction(tr("Video"));
    mpARAction->setData(0);
    subMenu->addAction(tr("Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0/3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0/9.0);
    subMenu->addAction(tr("Custom"))->setData(-2);
    foreach(QAction* action, subMenu->actions()) {
        action->setCheckable(true);
    }
    mpARAction->setChecked(true);

    subMenu = new ClickableMenu(tr("Color space"));
    mpMenu->addMenu(subMenu);
    mpVideoEQ = new VideoEQConfigPage();
    connect(mpVideoEQ, SIGNAL(engineChanged()), this, SLOT(onVideoEQEngineChanged()));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpVideoEQ);
    subMenu->addAction(pWA);

    subMenu = new ClickableMenu(tr("Decoder"));
    mpMenu->addMenu(subMenu);
    mpDecoderConfigPage = new DecoderConfigPage();
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu = new ClickableMenu(tr("Renderer"));
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeVO(QAction*)));
    //TODO: AVOutput.name,detail(description). check whether it is available
    VideoRendererId *vo = NULL;
    while ((vo = VideoRenderer::next(vo)))
    {
        // skip non-widget renderers
        if (*vo == VideoRendererId_OpenGLWindow || *vo == VideoRendererId_GraphicsItem)
            continue;
        QAction *voa = subMenu->addAction(QString::fromLatin1(VideoRenderer::name(*vo)));
        voa->setData(*vo);
        voa->setCheckable(true);
        if (!mpVOAction)
            mpVOAction = voa;
    }
    mpVOAction->setChecked(true);
    mVOActions = subMenu->actions();

    mpMenu->addSeparator();

    mpMenu->addAction(tr("Media information"), this, SLOT(showInfo()));

    mainLayout->addLayout(mpPlayerLayout);
    mainLayout->addWidget(mpTimeSlider);

    SetCenterWidget(pCenterContainer);
    connect(pSpeedBox, SIGNAL(valueChanged(double)), this
            , SLOT(onSpinBoxChanged(double)));
    connect(m_pWindowBottom, SIGNAL(onOpenFileButtonClicked()), this
            , SLOT(openFile()));
    connect(m_pWindowBottom, SIGNAL(onPlayButtonClicked()), this
            , SLOT(togglePlayPause()));
    connect(m_pWindowBottom, SIGNAL(onSetupButtonClicked()), this, SLOT(setup()));
    connect(m_pWindowBottom, SIGNAL(onFullscreenButtonClicked()), this
            , SLOT(toggleFullscreen()));
    connect(m_pWindowBottom, SIGNAL(onMuteButtonClicked()), this
            , SLOT(toggleMuteState()));
    connect(m_pWindowBottom, SIGNAL(onNextButtonClicked()), this, SLOT(playNext()));
    connect(m_pWindowBottom, SIGNAL(onPreviousButtonClicked()), this
                                                            , SLOT(playPrevious()));
    //valueChanged can be triggered by non-mouse event
    connect(mpTimeSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(mpTimeSlider, SIGNAL(sliderPressed()), this, SLOT(seek()));
    connect(mpTimeSlider, SIGNAL(onLeave()), this, SLOT(onTimeSliderLeave()));
    connect(mpTimeSlider, SIGNAL(onHover(int,int)), this
            , SLOT(onTimeSliderHover(int,int)));
    connect(&Config::instance(), SIGNAL(userShaderEnabledChanged()), this
            , SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragHeaderChanged()), this
            , SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragSampleChanged()), this
            , SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragPostProcessChanged()), this
            , SLOT(onUserShaderChanged()));

    connect(&Config::instance(), SIGNAL(languageChanged()), this
            , SLOT(updateLanguage()));
    connect(&Config::instance(), SIGNAL(skinChanged()), this
            , SLOT(updateSkin()));
    connect(&Config::instance(), SIGNAL(showLocalTimeChanged()), this
            , SLOT(showOSDLocalTime()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));
}

void MainWindow::changeChannel(QAction *action)
{
    if (action == mpChannelAction) {
        action->toggle();
        return;
    }
    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        qWarning("No audio output!");
        return;
    }
    mpChannelAction->setChecked(false);
    mpChannelAction = action;
    mpChannelAction->setChecked(true);
    if (!ao->close()) {
        qWarning("close audio failed");
        return;
    }
    AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);
    if (!ao->open()) {
        qWarning("open audio failed");
        return;
    }
}

void MainWindow::changeAudioTrack(QAction *action)
{
    int track = action->data().toInt();
    if (mpAudioTrackAction == action && track >= 0) { // external action is always clickable
        action->toggle();
        return;
    }
    if (track < 0) {
        QString f =
                QFileDialog::getOpenFileName(0, tr("Open an external audio track")
                               , QFileInfo(Config::instance().lastFile()).absolutePath());
        if (f.isEmpty()) {
            action->toggle();
            return;
        }
        mpPlayer->setExternalAudio(f);
    } else {
        mpPlayer->setExternalAudio(QString());
        if (!mpPlayer->setAudioStream(track)) {
            action->toggle();
            return;
        }
    }

    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(false);
    mpAudioTrackAction = action;
    mpAudioTrackAction->setChecked(true);
    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
}

void MainWindow::changeVO(QAction *action)
{
    if (action == mpVOAction) {
        action->toggle(); //check state changes if clicked
        return;
    }
    VideoRendererId vid = (VideoRendererId)action->data().toInt();
    VideoRenderer *vo = VideoRenderer::create(vid);
    if (vo && vo->isAvailable()) {
        if (!setRenderer(vo))
            action->toggle();
    } else {
        action->toggle(); //check state changes if clicked
        SMessageBox::error(nullptr, QString::fromLatin1("QtAV"), tr("not availabe on your platform!"));
        return;
    }
}

void MainWindow::processPendingActions()
{
    if (mHasPendingPlay) {
        mHasPendingPlay = false;
        play(mFile);
    }
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    mAudioBackends = backends;
}

bool MainWindow::setRenderer(QtAV::VideoRenderer *renderer)
{
    if (!renderer)
        return false;
    if (!renderer->widget()) {
        SMessageBox::error(nullptr, QString::fromLatin1("QtAV"), tr("Can not use this renderer"));
        return false;
    }
    mpOSD->uninstall();
    mpSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); //mouseMoveEvent without press.
    mpPlayer->setRenderer(renderer);
    QWidget *r = 0;
    if (mpRenderer)
        r = mpRenderer->widget();
    //release old renderer and add new
    if (r) {
        mpPlayerLayout->removeWidget(r);
        if (r->testAttribute(Qt::WA_DeleteOnClose)) {
            r->close();
        } else {
            r->close();
            delete r;
        }
        r = 0;
    }
    mpRenderer = renderer;
    //setInSize?
    mpPlayerLayout->addWidget(renderer->widget());
    if (mpVOAction) {
        mpVOAction->setChecked(false);
    }
    foreach (QAction *action, mVOActions) {
        if (action->data() == renderer->id()) {
            mpVOAction = action;
            break;
        }
    }
    mpVOAction->setChecked(true);
    const VideoRendererId vid = mpPlayer->renderer()->id();
    if (vid == VideoRendererId_GLWidget
            || vid == VideoRendererId_GLWidget2
            || vid == VideoRendererId_OpenGLWidget
            ) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale << VideoEQConfigPage::GLSL);
        mpVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    } else if (vid == VideoRendererId_XV) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        mpVideoEQ->setEngine(VideoEQConfigPage::XV);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    } else {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        mpVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        mpPlayer->renderer()->forcePreferredPixelFormat(false);
    }
    onVideoEQEngineChanged();
    mpOSD->installTo(mpRenderer);
    mpSubtitle->installTo(mpRenderer);
    onUserShaderChanged();
#define GL_ASS 0
#if GL_ASS
    GLSLFilter* glsl = new GLSLFilter(this);
    glsl->setOutputSize(QSize(4096, 2160));
    //mpRenderer->installFilter(glsl);
    if (mpRenderer->opengl()) {
        connect(mpRenderer->opengl(), &OpenGLVideo::beforeRendering, [this](){
            OpenGLVideo* glv = mpRenderer->opengl();
            glv->setSubImages(mpSubtitle->subImages(glv->frameTime(), glv->frameWidth(), glv->frameHeight()));
        });
    }
#endif

    if (vid == VideoRendererId_OpenGLWidget)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("opengl"));
    }
    else if (vid == VideoRendererId_GLWidget2)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("gl"));
    }
    else if (vid == VideoRendererId_Direct2D)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("d2d"));
    }
    else if (vid == VideoRendererId_GDI)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("gdi"));
    }
    else if (vid == VideoRendererId_XV)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("xv"));
    }
    else if (vid == VideoRendererId_X11)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("x11"));
    }
    else if (vid == VideoRendererId_Widget)
    {
        Config::instance().setVideoRenderer(QString::fromLatin1("qt"));
    }
    else
    {
#ifndef QT_NO_OPENGL
        Config::instance().setVideoRenderer(QString::fromLatin1("gl"));
#else
        Config::instance().setVideoRenderer(QString::fromLatin1("qt"));
#endif
    }

    if (mpLogoWidget)
    {
        mpLogoWidget = NULL;
    }
    createLogoWidget(mpRenderer->widget());
    if (mpPlayer->isLoaded() || mpPlayer->isPlaying() || mpPlayer->isPaused())
    {
        mpLogoWidget->hide();
    }

    return true;
}

void MainWindow::play(const QString &name, const int &command)
{
    if (name.isEmpty())
    {
        return;
    }
    Config::instance().setLastFile(name);
    mFile = name;
    if (!mIsReady)
    {
        mHasPendingPlay = true;
        return;
    }
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://"))) {
        mTitle = QFileInfo(mFile).fileName();
    }
    GetHeader()->SetTitleText(mTitle);
    setWindowTitle(mTitle);
    mpPlayer->stop(); //if no stop, in setPriority decoder will reopen
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    if (!mAudioBackends.isEmpty())
        mpPlayer->audio()->setBackends(mAudioBackends);
    if (!mpRepeatEnableAction->isChecked())
        mRepeateMax = 0;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
    mpPlayer->setInterruptTimeout(Config::instance().timeout()*1000.0);
    mpPlayer->setBufferMode(QtAV::BufferPackets);
    mpPlayer->setBufferValue(Config::instance().bufferValue());
    mpPlayer->setRepeat(mRepeateMax);
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());
    if (Config::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());
    qDebug() << Config::instance().avformatOptions();
    PlayListItem item;
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setLastTime(0);
    mpHistory->remove(mFile);
    mpHistory->insertItemAt(item, 0);
    if (command == 1)
    {
        if (mpPlayList->rowCount() > 1)
        {
            PlayListItem previousItem = mpPlayList->itemAt(0);
            mpPlayList->remove(previousItem.url());
            mpPlayList->insertItemAt(previousItem, mpPlayList->rowCount());
        }
    }
    else if (command == -1)
    {
        if (mpPlayList->rowCount() > 1)
        {
            PlayListItem previousItem = mpPlayList->itemAt(0);
            mpPlayList->remove(mFile);
            mpPlayList->insertItemAt(previousItem, 0);
        }
    }
    else
    {
        mpPlayList->removeAll();
        mpPlayList->insertItemAt(item, 0);
        addSimilarFilesToPlayList(mFile);
    }
    autoLoadExternalSubtitleFile(mFile);
    mpPlayer->play(name);
}

void MainWindow::play(const QUrl &url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
}

void MainWindow::setVideoDecoderNames(const QStringList &vd)
{
    QStringList vdnames;
    foreach (const QString& v, vd) {
        vdnames << v.toLower();
    }
    QStringList vidp;
    QStringList vids = idsToNames(VideoDecoder::registered());
    foreach (const QString& v, vids) {
        if (vdnames.contains(v.toLower())) {
            vidp.append(v);
        }
    }
    Config::instance().setDecoderPriorityNames(vidp);
}

void MainWindow::openFile()
{
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open a media file"));
    fileDialog.setMimeTypeFilters(QStringList()
                                  << QString::fromLatin1("application/octet-stream")
                                  << Common::supportedMimeTypes());
    fileDialog.setDirectory(QFileInfo(Config::instance().lastFile()).absoluteDir());
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString file = fileDialog.selectedUrls().constFirst().toLocalFile();
        if (file.isEmpty())
        {
            return;
        }
        if (QFileInfo(file).exists() && QFileInfo(file).isFile())
        {
            play(file);
        }
    }
}

void MainWindow::togglePlayPause()
{
    if (mpPlayer->isPlaying())
    {
        qDebug("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(!mpPlayer->isPaused());
    }
    else
    {
        if (mFile.isEmpty())
        {
            return;
        }
        if (!mpPlayer->isPlaying())
        {
            play(mFile);
        }
        else
        {
            mpPlayer->play();
        }
    }
    if (mpOSD)
    {
        if (mOSDTimer)
        {
            killTimer(mOSDTimer);
            mOSDTimer = 0;
        }
        mpOSD->setShowType(OSD::ShowType::ShowPlayPauseStatus);
        mOSDTimer = startTimer(3000);
    }
}

void MainWindow::cleanHistory()
{
    if (!mpHistory || !mpPlayList)
    {
        return;
    }
    mpHistory->cleanAndDelete();
    mpPlayList->cleanAndDelete();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!mpPlayer)
        return;
    mpPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p)
    {
        qDebug("start pausing...");
        m_pWindowBottom->changePauseButtonToPlayButton();
        if (mpThumbnailToolBar)
        {
            mpPlayToolButton->setToolTip(tr("Play"));
            mpPlayToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        if (mpTaskbarButton)
        {
            mpTaskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPause));
            if (mpPlayer->isSeekable())
            {
                mpTaskbarProgress->show();
                mpTaskbarProgress->pause();
            }
        }
    }
    else
    {
        qDebug("stop pausing...");
        m_pWindowBottom->changePlayButtonToPauseButton();
        if (mpThumbnailToolBar)
        {
            mpPlayToolButton->setToolTip(tr("Pause"));
            mpPlayToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        }
        if (mpTaskbarButton)
        {
            mpTaskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            if (mpPlayer->isSeekable())
            {
                mpTaskbarProgress->show();
                mpTaskbarProgress->resume();
            }
        }
    }
}

void MainWindow::onStartPlay()
{
    changeWindowSizeToVideoSize(mpPlayer->statistics().video_only.coded_width
                                , mpPlayer->statistics().video_only.coded_height);

    mpRenderer->setRegionOfInterest(QRectF());
    mFile = mpPlayer->file(); //open from EventFilter's menu
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
        mTitle = QFileInfo(mFile).fileName();
    GetHeader()->SetTitleText(mTitle);
    setWindowTitle(mTitle);

    m_pWindowBottom->changePlayButtonToPauseButton();
    mpTimeSlider->setMinimum(mpPlayer->mediaStartPosition());
    mpTimeSlider->setMaximum(mpPlayer->mediaStopPosition());
    mpTimeSlider->setValue(0);
    mpTimeSlider->setEnabled(mpPlayer->isSeekable());

    if (mpOSD)
    {
        if (Config::instance().getShowLocalTime())
        {
            mpOSD->setShowType(OSD::ShowType::ShowCurrentLocalTime);
        }
    }

    if (mpFullScreenProgressBar)
    {
        if (mpPlayer->isSeekable())
        {
            mpFullScreenProgressBar->setMinimum(mpPlayer->mediaStartPosition());
            mpFullScreenProgressBar->setMaximum(mpPlayer->mediaStopPosition());
        }
        else
        {
            mpFullScreenProgressBar->setMinimum(0);
            mpFullScreenProgressBar->setMaximum(0);
        }
        mpFullScreenProgressBar->setValue(0);
    }

    m_pWindowBottom->getTotalDurationLabel()\
            ->setText(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition())\
                      .toString(QString::fromLatin1("HH:mm:ss")));
    setVolume(true);
    ScreenSaver::instance().disable();
    initAudioTrackMenu();
    mpRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatA->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->startPosition()));
    mpRepeatB->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->stopPosition()));
    mCursorTimer = startTimer(3000);
    PlayListItem item = mpHistory->itemAt(0);
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setDuration(mpPlayer->duration());
    mpHistory->setItemAt(item, 0);
    mpPlayList->setItemAt(item, 0);
    updateChannelMenu();

    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());

    if (mpTaskbarButton)
    {
        mpTaskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        if (mpPlayer->isSeekable())
        {
            mpTaskbarProgress->show();
            mpTaskbarProgress->setMinimum(mpTimeSlider->minimum());
            mpTaskbarProgress->setMaximum(mpTimeSlider->maximum());
            mpTaskbarProgress->setValue(0);
            mpTaskbarProgress->resume();
        }
    }
    if (mpLogoWidget->isVisible())
    {
        mpLogoWidget->hide();
    }
    if (mpThumbnailToolBar)
    {
        mpPlayToolButton->setToolTip(tr("Pause"));
        mpPlayToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }

    if (mpTrayIcon)
    {
        mpTrayIcon->setToolTip(tr("Now playing : ") + mTitle);
    }
}

void MainWindow::onStopPlay()
{
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    if (mpPlayer->currentRepeat() >= 0 && mpPlayer->currentRepeat() < mpPlayer->repeat())
        return;
    // use shortcut to replay in EventFilter, the options will not be set, so set here
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());
    if (Config::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());

    m_pWindowBottom->changePauseButtonToPlayButton();
    mpTimeSlider->setValue(0);
    qDebug(">>>>>>>>>>>>>>disable slider");
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setMaximum(0);

    if (mpFullScreenProgressBar)
    {
        mpFullScreenProgressBar->setValue(0);
        mpFullScreenProgressBar->setMinimum(0);
        mpFullScreenProgressBar->setMaximum(0);
    }

    m_pWindowBottom->getCurrentDurationLabel()->setText(QString::fromLatin1("00:00:00"));
    m_pWindowBottom->getTotalDurationLabel()->setText(QString::fromLatin1("00:00:00"));
    ScreenSaver::instance().enable();
    toggleRepeat(false);
    //mRepeateMax = 0;
    if (mCursorTimer)
    {
        killTimer(mCursorTimer);
        mCursorTimer = 0;
    }
    unsetCursor();
    if (m_preview)
        m_preview->setFile(QString());

    if (mpTaskbarButton)
    {
        mpTaskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaStop));
        mpTaskbarProgress->reset();
        mpTaskbarProgress->hide();
    }

    if (mpTrayIcon)
    {
        mpTrayIcon->setToolTip(QString::fromLatin1(AppDisplayName));
    }

    if (windowState() == Qt::WindowFullScreen)
    {
        toggleFullscreen(true);
    }
}

void MainWindow::setFrameRate()
{
    if (!mpPlayer)
        return;
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
}

void MainWindow::seek(int value)
{
    mpPlayer->setSeekType(AccurateSeek);
    mpPlayer->seek((qint64)value);
    showPreviewWindow();
}

void MainWindow::seek()
{
    seek(mpTimeSlider->value());
}

void MainWindow::setVolume()
{
    if (!mpPlayer/* || !mpPlayer->audio() || !mpPlayer->audio()->isAvailable()*/)
    {
        return;
    }
    setVolume(false);
}

void MainWindow::setVolume(bool isDevUse)
{
    if (!mpPlayer)
    {
        return;
    }
    qreal v = qreal(m_pWindowBottom->getVolumeSlider()->value())*kVolumeInterval;
    if (qAbs(int(mpPlayer->audio()->volume()/kVolumeInterval) - \
             m_pWindowBottom->getVolumeSlider()->value()) >= int(0.1/kVolumeInterval))
    {
        mpPlayer->audio()->setVolume(v);
    }
    int v2 = static_cast<int>(v * 100.0 / 4.0);
    m_pWindowBottom->getVolumeSlider()->setToolTip(tr("Volume ") + QString::number(v2)
                                                   + QString::fromLatin1("%"));
    if (!isDevUse)
    {
        if (m_pWindowBottom->getVolumeSlider()->value() == 0)
        {
            setMute(true);
        }
        else if (mpPlayer->audio()->isMute())
        {
            toggleMuteState();
        }
        if (mpOSD)
        {
            if (mOSDTimer)
            {
                killTimer(mOSDTimer);
                mOSDTimer = 0;
            }
            mpOSD->setShowType(OSD::ShowType::ShowCurrentVolumePercent);
            mOSDTimer = startTimer(3000);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (Config::instance().getSaveHistoryState())
    {
        mpHistory->save();
        //mpPlayList->save();
    }
    else
    {
        cleanHistory();
    }
    Config::instance().setLastVolume(m_pWindowBottom->getVolumeSlider()->value());
    if (m_pWindowBottom->getVolumeSlider()->value() < 1)
    {
        Config::instance().setIsMuted(true);
    }
    else
    {
        Config::instance().setIsMuted(mpPlayer->audio()->isMute());
    }
    if (mpPlayer)
    {
        mpPlayer->stop();
        delete mpPlayer;
        mpPlayer = NULL;
    }
    StandardDialog::closeEvent(e);
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mCursorTimer)
    {
        setCursor(Qt::BlankCursor);
    }
    else if (e->timerId() == mTimeSliderHoverTimer)
    {
        if (!canShowPreviewWindow)
        {
            canShowPreviewWindow = true;
            showPreviewWindow();
        }
    }
    else if (e->timerId() == mTimeSliderLeaveTimer)
    {
        if (mTimeSliderLeaveTimer)
        {
            killTimer(mTimeSliderLeaveTimer);
            mTimeSliderLeaveTimer = 0;
        }
        closePreviewWindow();
    }
    else if (e->timerId() == mOSDTimer)
    {
        if (Config::instance().getShowLocalTime())
        {
            if (mpOSD)
            {
                if (mOSDTimer)
                {
                    killTimer(mOSDTimer);
                    mOSDTimer = 0;
                }
                mpOSD->setShowType(OSD::ShowType::ShowCurrentLocalTime);
            }
        }
        else
        {
            hideOSD();
        }
    }
    StandardDialog::timerEvent(e);
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (mpPlayer->isSeekable())
    {
        mpTimeSlider->setValue(pos);
        if (mpFullScreenProgressBar)
        {
            mpFullScreenProgressBar->setValue(pos);
        }
        if (mpTaskbarButton)
        {
            mpTaskbarProgress->setValue(pos);
        }
    }
    m_pWindowBottom->getCurrentDurationLabel()\
            ->setText(QTime(0, 0, 0).addMSecs(pos)\
                      .toString(QString::fromLatin1("HH:mm:ss")));
}

void MainWindow::repeatAChanged(const QTime& t)
{
    if (!mpPlayer)
        return;
    mpPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::repeatBChanged(const QTime& t)
{
    if (!mpPlayer)
        return;
    // when this slot is called? even if only range is set?
    if (t <= mpRepeatA->time())
        return;
    mpPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!mpPlayer)
    {
        StandardDialog::wheelEvent(event);
        return;
    }
    if (!event->angleDelta().isNull())
    {
        if (event->angleDelta().y() > 0)
        {
            increaseVolume();
        }
        else if (event->angleDelta().y() < 0)
        {
            decreaseVolume();
        }
    }
    StandardDialog::wheelEvent(event);
}

static bool canHandleDrop(const QDragEnterEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() < 1)
    {
        return false;
    }
    QMimeDatabase mimeDatabase;
    return Common::supportedMimeTypes().
        contains(mimeDatabase.mimeTypeForUrl(urls.constFirst()).name());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->setAccepted(canHandleDrop(event));
    StandardDialog::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    event->accept();
    play(event->mimeData()->urls().constFirst());
    StandardDialog::dropEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (windowState() != Qt::WindowFullScreen)
    {
        StandardDialog::mousePressEvent(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    unsetCursor();
    if (windowState() == Qt::WindowFullScreen)
    {
        QDesktopWidget desktop;
        if (QCursor::pos().y() >=
                (desktop.screen()->height() - mpTimeSlider->height()
                 - m_pWindowBottom->height() - 50))
        {
            showFullscreenControlPanel();
        }
        else
        {
            hideFullscreenControlPanel();
        }
    }
    else
    {
        StandardDialog::mouseMoveEvent(event);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type type = event->type();
    if (mpFullscreenControlPanel)
    {
        if (obj == mpFullscreenControlPanel)
        {
            switch (type)
            {
            case QEvent::KeyPress:
            {
                QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
                int key = key_event->key();
                Qt::KeyboardModifiers modifiers = key_event->modifiers();
                switch (key)
                {
                case Qt::Key_Cancel:
                    escKeyPressed();
                    return true;
                    break;
                case Qt::Key_Refresh:
                    setup();
                    return true;
                    break;
                case Qt::Key_Help:
                    about();
                    return true;
                    break;
                case Qt::Key_Space:
                    togglePlayPause();
                    return true;
                    break;
                case Qt::Key_Return:
                case Qt::Key_Enter:
                    toggleFullscreen();
                    return true;
                    break;
                case Qt::Key_Up:
                    increaseVolume();
                    return true;
                    break;
                case Qt::Key_Down:
                    decreaseVolume();
                    return true;
                    break;
                case Qt::Key_O:
                {
                    if (modifiers == Qt::ControlModifier)
                    {
                        openFile();
                        return true;
                    }
                    return false;
                    break;
                }
                case Qt::Key_Left:
                    seekBackward();
                    return true;
                    break;
                case Qt::Key_Right:
                    seekForward();
                    return true;
                    break;
                case Qt::Key_M:
                    toggleMuteState();
                    return true;
                    break;
                case Qt::Key_PageDown:
                    seekNextChapter();
                    return true;
                    break;
                case Qt::Key_PageUp:
                    seekPreviousChapter();
                    return true;
                    break;
                default:
                    return false;
                }
                break;
            }
            case QEvent::Wheel:
            {
                QWheelEvent *wheel_event = static_cast<QWheelEvent*>(event);
                if (!mpPlayer)
                {
                    StandardDialog::wheelEvent(wheel_event);
                    return false;
                }
                if (!wheel_event->angleDelta().isNull())
                {
                    if (wheel_event->angleDelta().y() > 0)
                    {
                        increaseVolume();
                        return true;
                    }
                    else if (wheel_event->angleDelta().y() < 0)
                    {
                        decreaseVolume();
                        return true;
                    }
                }
                StandardDialog::wheelEvent(wheel_event);
                return false;
                break;
            }
            }
        }
    }
    switch (type)
    {
    case QEvent::GraphicsSceneContextMenu:
    {
        QGraphicsSceneContextMenuEvent *e = static_cast<QGraphicsSceneContextMenuEvent*>(event);
        onShowPopupMenu(e->screenPos());
        return true;
        break;
    }
    case QEvent::ContextMenu:
    {
        QContextMenuEvent *e = static_cast<QContextMenuEvent*>(event);
        onShowPopupMenu(e->globalPos());
        return true;
        break;
    }
    default:
        return false;
    }
    return StandardDialog::eventFilter(obj, event);
    //return true; //false: for text input
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() != QEvent::WindowStateChange)
    {
        StandardDialog::changeEvent(event);
        return;
    }
    if (!mpPlayer)
    {
        StandardDialog::changeEvent(event);
        return;
    }
    if (windowState() == Qt::WindowMinimized)
    {
        if (mpPlayer->isPlaying())
        {
            if (!mpPlayer->isPaused())
            {
                togglePlayPause();
            }
        }
    }
    StandardDialog::changeEvent(event);
}

/*bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *param = static_cast<MSG *>(message);

    switch (param->message)
    {
    case WM_COPYDATA:
    {
        COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(param->lParam);
        if (cds->dwData == (ULONG_PTR)CUSTOM_SPLAYER_TYPE)
        {
            QString strMessage = QString::fromUtf8(reinterpret_cast<char *>(cds->lpData), cds->cbData);
            qDebug() << "[Windows Message] : " << strMessage.toUtf8().constData();
            QFileInfo fileInfo(strMessage);
            if (fileInfo.exists() && fileInfo.isFile())
            {
                play(strMessage);
            }
            *result = 1;
            return true;
        }
    }
    }

    return StandardDialog::nativeEvent(eventType, message, result);
}*/

void MainWindow::about()
{
    if (!mpAboutBox)
    {
        return;
    }
    if (mpAboutBox->isVisible())
    {
        mpAboutBox->hide();
    }
    mpAboutBox->setWindowModality(Qt::ApplicationModal);
    mpAboutBox->show();
}

void MainWindow::aboutQtAV()
{
    QtAV::about();
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(this, tr("Open an URL")
                                        , QString::fromLatin1("URL"));
    if (url.isEmpty())
    {
        return;
    }
    play(url);
}

void MainWindow::updateChannelMenu()
{
    if (mpChannelAction)
        mpChannelAction->setChecked(false);
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        return;
    }
    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as = mpChannelMenu->actions();
    foreach (QAction *action, as) {
        if (action->data().toInt() != (int)cl)
            continue;
        action->setChecked(true);
        mpChannelAction = action;
        break;
    }
}

void MainWindow::initAudioTrackMenu()
{
    int track = -2;
    QAction *a = 0;
    QList<QAction*> as;
    int tracks = 0;
    if (!mpPlayer) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
        goto end;
    }
    track = mpPlayer->currentAudioStream();
    as = mpAudioTrackMenu->actions();
    tracks = mpPlayer->audioStreamCount();
    if (mpAudioTrackAction && tracks == as.size()-1 && mpAudioTrackAction->data().toInt() == track)
        return;
    while (tracks + 1 < as.size()) {
        a = as.takeLast();
        mpAudioTrackMenu->removeAction(a);
        delete a;
    }
    if (as.isEmpty()) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
    }
    while (tracks + 1 > as.size()) {
        a = mpAudioTrackMenu->addAction(QString::number(as.size()-1));
        a->setData(as.size()-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
    }
end:
    foreach(QAction *ac, as) {
        if (ac->data().toInt() == track && track >= 0) {
            if (mpPlayer && mpPlayer->externalAudio().isEmpty()) {
                qDebug("track found!!!!!");
                mpAudioTrackAction = ac;
                ac->setChecked(true);
            }
        } else {
            ac->setChecked(false);
        }
    }
    if (mpPlayer && !mpPlayer->externalAudio().isEmpty()) {
        mpAudioTrackAction = as.first();
    }
    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(true);
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();
    if (action == mpARAction && r != -2) {
        action->toggle(); //check state changes if clicked
        return;
    }
    if (r == 0) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    } else if (r == -1) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
    } else {
        if (r == -2)
            r = QInputDialog::getDouble(0, tr("Aspect ratio"), QString(), 1.0);
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::CustomAspectRation);
        mpPlayer->renderer()->setOutAspectRatio(r);
    }
    mpARAction->setChecked(false);
    mpARAction = action;
    mpARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    mpRepeatEnableAction->setChecked(r);
    mpRepeatAction->defaultWidget()->setEnabled(r); //why need defaultWidget?
    if (r) {
        mRepeateMax = mpRepeatBox->value();
    } else {
        mRepeateMax = 0;
    }
    if (mpPlayer) {
        mpPlayer->setRepeat(mRepeateMax);
        if (r) {
            repeatAChanged(mpRepeatA->time());
            repeatBChanged(mpRepeatB->time());
        } else {
            mpPlayer->setTimeRange(0);
        }
    }
}

void MainWindow::setRepeateMax(int m)
{
    mRepeateMax = m;
    if (mpPlayer) {
        mpPlayer->setRepeat(m);
    }
}

void MainWindow::onPlayListClick(const QString &key, const QString &value)
{
    mTitle = key;
    play(value);
}

void MainWindow::showInfo()
{
    if (!mpStatisticsView)
    {
        mpStatisticsView = new StatisticsView(this);
    }
    if (mpPlayer)
    {
        mpStatisticsView->setStatistics(mpPlayer->statistics());
    }
    int w = 600;
    int h = 500;
    if (width() / 2 >= w)
    {
        w = width() / 2;
    }
    if (height() / 2 >= h)
    {
        h = height() / 2;
    }
    mpStatisticsView->resize(w, h);
    mpStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    mTimeSliderHoverValue = value;
    mCurrentCursorPosition = mapToGlobal(mpTimeSlider->pos() + QPoint(pos, 30));
    if (mTimeSliderLeaveTimer)
    {
        killTimer(mTimeSliderLeaveTimer);
        mTimeSliderLeaveTimer = 0;
    }
    if (mTimeSliderHoverTimer == 0)
    {
        mTimeSliderHoverTimer = startTimer(700);
    }
    if (canShowPreviewWindow)
    {
        showPreviewWindow();
    }
}

void MainWindow::onTimeSliderLeave()
{
    if (m_preview)
    {
        if (m_preview->isVisible())
        {
            m_preview->hide();
        }
    }
    if (mTimeSliderHoverTimer)
    {
        killTimer(mTimeSliderHoverTimer);
        mTimeSliderHoverTimer = 0;
    }
    if (mTimeSliderLeaveTimer)
    {
        killTimer(mTimeSliderLeaveTimer);
        mTimeSliderLeaveTimer = 0;
    }
    mTimeSliderLeaveTimer = startTimer(5000);
    canShowPreviewWindow = false;
}

void MainWindow::handleError(const AVError &e)
{
    SMessageBox::error(nullptr, tr("Player error"), e.string());
}

void MainWindow::onMediaStatusChanged()
{
    QString status;
    AVPlayer *player = reinterpret_cast<AVPlayer*>(sender());
    if (!player) { //why it happens? reinterpret_cast  works.
        qWarning() << "invalid sender() " << sender() << player;
        return;
    }
    switch (player->mediaStatus()) {
    case NoMedia:
        status = tr("No media");
        break;
    case InvalidMedia:
        status = tr("Invalid meida");
        break;
    case BufferingMedia:
        status = tr("Buffering...");
        break;
    case BufferedMedia:
        status = tr("Buffered");
        break;
    case LoadingMedia:
        status = tr("Loading...");
        break;
    case LoadedMedia:
        status = tr("Loaded");
        break;
    case StalledMedia:
        status = tr("Stalled");
        break;
    case EndOfMedia:
        status = tr("Ended");
        playNext();
        break;
    default:
        status = QString();
        onStopPlay();
        break;
    }
    qDebug() << "status changed " << status.toUtf8().data();
    GetHeader()->SetTitleText(status + QString::fromLatin1(" ") + mTitle);
    setWindowTitle(status + QString::fromLatin1(" ") + mTitle);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = mpPlayer->bufferSpeed();
    QString s;
    if (bs > 1024*1024*1024)
        s = QString("%1G/s").arg(bs/1024.0/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024*1024)
        s = QString("%1M/s").arg(bs/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024)
        s = QString("%1K/s").arg(bs/1024.0, 6, 'f', 1);
    else
        s = QString("%1B/s").arg(bs, 6, 'f', 1);
    GetHeader()->SetTitleText(QString::fromLatin1("Buffering... %1% @%2 ").arg(percent*100.0, 5, 'f', 1).arg(s) + mTitle);
    setWindowTitle(QString::fromLatin1("Buffering... %1% @%2 ").arg(percent*100.0, 5, 'f', 1).arg(s) + mTitle);
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer *vo = mpPlayer->renderer();
    VideoEQConfigPage::Engine e = mpVideoEQ->engine();
    if (e == VideoEQConfigPage::SWScale
            && vo->id() != VideoRendererId_X11 // X11 scales in the renderer
            ) {
        vo->forcePreferredPixelFormat(true);
        vo->setPreferredPixelFormat(VideoFormat::Format_RGB32);
    } else {
        vo->forcePreferredPixelFormat(false);
    }
    onBrightnessChanged(mpVideoEQ->brightness()*100.0);
    onContrastChanged(mpVideoEQ->contrast()*100.0);
    onHueChanged(mpVideoEQ->hue()*100.0);
    onSaturationChanged(mpVideoEQ->saturation()*100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    VideoRenderer *vo = mpPlayer->renderer();
    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
            && vo->setBrightness(mpVideoEQ->brightness())) {
        mpPlayer->setBrightness(0);
    } else {
        vo->setBrightness(0);
        mpPlayer->setBrightness(b);
    }
}

void MainWindow::onContrastChanged(int c)
{
    VideoRenderer *vo = mpPlayer->renderer();
    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
            && vo->setContrast(mpVideoEQ->contrast())) {
        mpPlayer->setContrast(0);
    } else {
        vo->setContrast(0);
        mpPlayer->setContrast(c);
    }
}

void MainWindow::onHueChanged(int h)
{
    VideoRenderer *vo = mpPlayer->renderer();
    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
            && vo->setHue(mpVideoEQ->hue())) {
        mpPlayer->setHue(0);
    } else {
        vo->setHue(0);
        mpPlayer->setHue(h);
    }
}

void MainWindow::onSaturationChanged(int s)
{
    VideoRenderer *vo = mpPlayer->renderer();
    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
            && vo->setSaturation(mpVideoEQ->saturation())) {
        mpPlayer->setSaturation(0);
    } else {
        vo->setSaturation(0);
        mpPlayer->setSaturation(s);
    }
}

void MainWindow::onCaptureConfigChanged()
{
    mpPlayer->videoCapture()->setCaptureDir(Config::instance().captureDir());
    mpPlayer->videoCapture()->setQuality(Config::instance().captureQuality());
    if (Config::instance().captureFormat().toLower() == QLatin1String("original")) {
        mpPlayer->videoCapture()->setOriginalFormat(true);
    } else {
        mpPlayer->videoCapture()->setOriginalFormat(false);
        mpPlayer->videoCapture()->setSaveFormat(Config::instance().captureFormat());
    }
    m_pWindowBottom->getVideoCaptureButton()\
            ->setToolTip(QString::fromLatin1("%1\n%2: %3\n%4: %5")
                                        .arg(tr("Capture video frame"))
                                        .arg(tr("Save to"))
                                        .arg(mpPlayer->videoCapture()->captureDir())
                                        .arg(tr("Format"))
                                        .arg(Config::instance().captureFormat()));

}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (mpVideoFilter) {
        mpVideoFilter->uninstall();
        delete mpVideoFilter;
        mpVideoFilter = NULL;
    }
    mpVideoFilter = new LibAVFilterVideo(this);
    mpVideoFilter->setEnabled(Config::instance().avfilterVideoEnable());
    mpPlayer->installFilter(mpVideoFilter);
    mpVideoFilter->setOptions(Config::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (mpAudioFilter) {
        mpAudioFilter->uninstall();
        delete mpAudioFilter;
        mpAudioFilter = NULL;
    }
    mpAudioFilter = new LibAVFilterAudio(this);
    mpAudioFilter->setEnabled(Config::instance().avfilterAudioEnable());
    mpAudioFilter->installTo(mpPlayer);
    mpAudioFilter->setOptions(Config::instance().avfilterAudioOptions());
}

void MainWindow::donate()
{
    //QDesktopServices::openUrl(QUrl(QString::fromLatin1(AppDonateURL)));
    if (!mpDonateBox)
    {
        return;
    }
    if (mpDonateBox->isVisible())
    {
        mpDonateBox->hide();
    }
    int w = 1020;
    int h = 800;
    if (width() / 2 >= w)
    {
        w = width() / 2;
    }
    if (height() / 2 >= h)
    {
        h = height() / 2;
    }
    mpDonateBox->resize(w, h);
    mpDonateBox->setWindowModality(Qt::ApplicationModal);
    mpDonateBox->show();
}

void MainWindow::onBufferValueChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setBufferValue(Config::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
}

void MainWindow::onUserShaderChanged()
{
    if (!mpRenderer || !mpRenderer->opengl())
        return;
#ifndef QT_NO_OPENGL
    if (Config::instance().userShaderEnabled()) {
        if (!m_shader)
            m_shader = new DynamicShaderObject(this);
        m_shader->setHeader(Config::instance().fragHeader());
        m_shader->setSample(Config::instance().fragSample());
        m_shader->setPostProcess(Config::instance().fragPostProcess());
        mpRenderer->opengl()->setUserShader(m_shader);
    } else {
        mpRenderer->opengl()->setUserShader(NULL);
    }
#endif
}

void MainWindow::setup()
{
    //ConfigDialog::display();
    if (!mpConfigDialog)
    {
        return;
    }
    if (mpConfigDialog->isVisible())
    {
        mpConfigDialog->hide();
    }
    mpConfigDialog->setWindowModality(Qt::ApplicationModal);
    mpConfigDialog->show();
}

void MainWindow::handleFullscreenChange()
{
    workaroundRendererSize();
}

void MainWindow::toggoleSubtitleEnabled(bool value)
{
    mpSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    mpSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open a subtitle file"));
    QStringList filters;
    filters << tr("Subtitle files (*.ass *.srt *.sub *.idx)")
            << tr("Any files (*)");
    fileDialog.setNameFilters(filters);
    fileDialog.setDirectory(QFileInfo(Config::instance().lastFile()).absoluteDir());
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString file = fileDialog.selectedUrls().constFirst().toLocalFile();
        if (file.isEmpty())
        {
            return;
        }
        if (QFileInfo(file).exists() && QFileInfo(file).isFile())
        {
            loadExternalSubtitleFile(file);
        }
    }
}

void MainWindow::setSubtitleCharset(const QString &charSet)
{
    Q_UNUSED(charSet);
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(const QString &value)
{
    Q_UNUSED(value)
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::changeClockType(QAction *action)
{
    action->setChecked(true);
    int value = action->data().toInt();
    if (value < 0) {
        mpPlayer->masterClock()->setClockAuto(true);
        // TODO: guess clock type
        return;
    }
    mpPlayer->masterClock()->setClockAuto(false);
    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::onShowPopupMenu(const QPoint &p)
{
    //mpMenu->popup(mapToGlobal(QPoint(pt.x(), pt.y())));
    mpMenu->exec(p);
}

void MainWindow::toggleFullscreen()
{
    toggleFullscreen(false);
}

void MainWindow::toggleFullscreen(bool isDevUse)
{
    if (!isDevUse)
    {
        if (!mpPlayer || !mpPlayer->isLoaded())
        {
            return;
        }
    }
    //在视频暂停状态下切换全屏状态，视频画面不会自动刷新，切换全屏/正常会导致画面被裁剪，观感很差
    //但只要窗口重绘就不会发生这个问题，但不知为什么，调用窗口的update()并不好用，调用
    //mpPlayer->renderer->widget()->update()也不好用，而且调整调用的地方也不管用
    //没有办法，只能先把窗口整个隐藏掉，最后再显示出来，强迫窗口整体刷新，只有这个笨办法是有效果的
    //只有暂停时会这样，我曾试过先判断有没有暂停，再去决定是否隐藏窗口，但会出现一些比较奇怪的问题
    //就是全屏时暂停后切换全屏/窗口状态会导致窗口隐藏掉显示不出来，具体原因暂时查不出来，可能是
    //我对Qt处理事件的流程还不熟悉的缘故，所以在解决这个问题前就暂时先不判断了，所有情况都是先隐藏
    //之后再显示出来，不区别对待
    hide();
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    handleFullscreenChange();
    if (windowState() == Qt::WindowFullScreen)
    {
        GetHeader()->hide();
        GetBottom()->hide();
        m_pWindowBottom->changeFullscreenButtonToExitFullscreenButton();
        if (mpFullScreenProgressBar)
        {
            if (mpPlayer->isSeekable())
            {
                if (!mpFullScreenProgressBar->isVisible())
                {
                    mpFullScreenProgressBar->show();
                }
            }
            else
            {
                mpFullScreenProgressBar->hide();
            }
        }
        if (!mpFullscreenControlPanel)
        {
            createFullscreenControlPanel();
        }
        setWindowCanMove(false);
        QDesktopWidget desktop;
        if (QCursor::pos().y() >=
                (desktop.screen()->height() - mpTimeSlider->height()
                 - m_pWindowBottom->height() - 50))
        {
            showFullscreenControlPanel();
        }
    }
    else
    {
        if (mpFullScreenProgressBar)
        {
            if (mpFullScreenProgressBar->isVisible())
            {
                mpFullScreenProgressBar->hide();
            }
        }
        GetHeader()->show();
        m_pWindowBottom->changeExitFullscreenButtonToFullscreenButton();
        if (mpFullscreenControlPanel)
        {
            destroyFullscreenControlPanel();
        }
        GetBottom()->show();
        setWindowCanMove(true);
    }
    show();
}

void MainWindow::changeWindowSizeToVideoSize(const int &w, const int &h)
{
    if (w < 2 || h < 2)
    {
        return;
    }
    int h2 = h + GetHeader()->height() + mpTimeSlider->height()
            + m_pWindowBottom->height();
    QDesktopWidget desktop;
    if (windowState() == Qt::WindowFullScreen)
    {
        if (h2 >= desktop.availableGeometry().height())
        {
            setRestoreGeometry(desktop.availableGeometry());
            setRestoreSize(desktop.availableGeometry().size());
            //setMinimumSize(desktop.availableGeometry().size());
            return;
        }
        setRestoreGeometry(QRect(getRestoreGeometry().topLeft(), QSize(w, h2)));
        setRestoreSize(QSize(w, h2));
        //setMinimumSize(w, h2);
        return;
    }
    if (getIsMaximized())
    {
        if (h2 < desktop.availableGeometry().height())
        {
            setRestoreGeometry(QRect(getRestoreGeometry().topLeft(), QSize(w, h2)));
            setRestoreSize(QSize(w, h2));
            //setMinimumSize(w, h2);
            return;
        }
        setRestoreGeometry(desktop.availableGeometry());
        setRestoreSize(desktop.availableGeometry().size());
        //setMinimumSize(desktop.availableGeometry().size());
        return;
    }
    if (h2 >= desktop.availableGeometry().height())
    {
        //setRestoreGeometry(desktop.availableGeometry());
        //setWindowMaximize(true);
        //setMinimumSize(size());
        int x = (desktop.availableGeometry().height() * w) / h2;
        //不知为什么等比例缩放横向上会出现11像素左右的很明显的黑边，
        //断点调试时也看过跟缩放有关的数据，发现数据计算完全正确，
        //播放视频时也把整个界面截图后放到PS里计算过相关的数据，
        //但数据完全没有问题，但观感上确实有很明显的很宽的黑边，
        //从数据、代码和逻辑上都无法找出谬误之处，故目前暂时无法找出确切原因，只能临时修改
        //不是计算误差的问题，误差只有零点几，人眼看不出来的
        resize(x - 11, desktop.availableGeometry().height());
        move((desktop.screen()->width() - width()) / 2, 0);
        return;
    }
    resize(w, h2);
    //setMinimumSize(size());
    setRestoreGeometry(geometry());
}

void MainWindow::moveWindowToDesktopCenter()
{
    QDesktopWidget desktop;
    int x = (desktop.screen()->width() - width()) / 2;
    int y = (desktop.screen()->height() - height()) / 2;
    move(QPoint(x, y));
}

void MainWindow::changeWindowSkin()
{
    QAction *act = (QAction *)sender();
    QString skinName = act->whatsThis();
    Common::changeSkinByName(skinName);
}

void MainWindow::Slot_SkinClicked()
{
    QPoint pt = QPoint(GetHeader()->getSkinButton()->geometry().left()
                       , GetHeader()->rect().bottom() + 1);
    mpSkinManagerMenu->exec(GetHeader()->mapToGlobal(pt));
}

void MainWindow::Slot_LogoClicked()
{
    QPoint pt = GetHeader()->rect().bottomLeft();
    mpLogoMenu->exec(GetHeader()->mapToGlobal(QPoint(pt.x(), pt.y() + 1)));
}

void MainWindow::Slot_StayOnTopClicked()
{
    mbStayOnTop = !mbStayOnTop;
    setWindowFlag(Qt::WindowStaysOnTopHint, mbStayOnTop);
    if (!isVisible())
    {
        show();
    }
    StandardDialog::Slot_StayOnTopClicked();
}

void MainWindow::setMute(bool canMute)
{
    if (!mpPlayer/* || !mpPlayer->audio() || !mpPlayer->audio()->isAvailable()*/)
    {
        return;
    }
    mpPlayer->audio()->setMute(canMute);
    if (mpPlayer->audio()->isMute())
    {
        m_pWindowBottom->changeMuteButtonToMutedButton();
        if (mpThumbnailToolBar)
        {
            mpMuteToolButton->setToolTip(tr("Sound"));
            mpMuteToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        }
    }
    else
    {
        m_pWindowBottom->changeMutedButtonToMuteButton();
        if (mpThumbnailToolBar)
        {
            mpMuteToolButton->setToolTip(tr("Mute"));
            mpMuteToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        }
    }
}

void MainWindow::toggleMuteState()
{
    if (!mpPlayer/* || !mpPlayer->audio() || !mpPlayer->audio()->isAvailable()*/)
    {
        return;
    }
    if (m_pWindowBottom->getVolumeSlider()->value() > 0)
    {
        setMute(!mpPlayer->audio()->isMute());
    }
    if (mpOSD)
    {
        if (mOSDTimer)
        {
            killTimer(mOSDTimer);
            mOSDTimer = 0;
        }
        mpOSD->setShowType(OSD::ShowType::ShowMuteStatus);
        mOSDTimer = startTimer(3000);
    }
}

void MainWindow::showFullscreenControlPanel()
{
    if (mpFullScreenProgressBar)
    {
        if (mpFullScreenProgressBar->isVisible())
        {
            mpFullScreenProgressBar->hide();
        }
    }
    if (mpFullscreenControlPanel)
    {
        if (!mpFullscreenControlPanel->isVisible())
        {
            mpFullscreenControlPanel->show();
        }
    }
}

void MainWindow::createFullscreenControlPanel()
{
    if (mpFullscreenControlPanel)
    {
        destroyFullscreenControlPanel();
    }
    mpFullscreenControlPanel = new StandardDialog(this);
    mpFullscreenControlPanel->installEventFilter(this);
    mpFullscreenControlPanel->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    //mpFullscreenControlPanel->setAttribute(Qt::WA_TranslucentBackground);
    mpFullscreenControlPanel->setWindowOpacity(0.75);
    mpFullscreenControlPanel->setWindowCanMove(false);
    mpFullscreenControlPanel->GetHeader()->setFixedHeight(0);
    mpFullscreenControlPanel->GetHeader()->hide();
    mpFullscreenControlPanel->GetBottom()->setFixedHeight(m_pWindowBottom->height());
    RemoveBottomWidget();
    m_pWindowBottom->setParent(mpFullscreenControlPanel);
    mpFullscreenControlPanel->SetBottomWidget(m_pWindowBottom);
    mpTimeSlider->parentWidget()->layout()->removeWidget(mpTimeSlider);
    mpTimeSlider->setParent(mpFullscreenControlPanel);
    mpFullscreenControlPanel->SetCenterWidget(mpTimeSlider);
    QDesktopWidget desktop;
    mpFullscreenControlPanel->setGeometry(0
       , (desktop.screen()->height() - m_pWindowBottom->height() - mpTimeSlider->height())
       , desktop.screen()->width(), (m_pWindowBottom->height() + mpTimeSlider->height()));
    mpFullscreenControlPanel->SetFixed(true);
    if (mpFullscreenControlPanel->isVisible())
    {
        mpFullscreenControlPanel->hide();
    }
}

void MainWindow::hideFullscreenControlPanel()
{
    if (mpFullscreenControlPanel)
    {
        if (mpFullscreenControlPanel->isVisible())
        {
            mpFullscreenControlPanel->hide();
        }
    }
    if (mpFullScreenProgressBar)
    {
        if (mpPlayer->isSeekable())
        {
            if (!mpFullScreenProgressBar->isVisible())
            {
                mpFullScreenProgressBar->show();
            }
        }
        else
        {
            mpFullScreenProgressBar->hide();
        }
    }
}

void MainWindow::updateLanguage()
{
    //Language changed
    qDebug() << "User changed UI language.";
    SMessageBox::information(nullptr
                             , tr("Language changed")
                             , tr("Language has been changed"
                                  ", restart the player to make it go into effect."));
}

void MainWindow::updateSkin()
{
    Common::changeSkinByName(Config::instance().getSkin());
    SMessageBox::information(nullptr
                             , tr("Skin changed")
                             , tr("Skin has been changed"
                                  ", some UI elements need to restart the player "
                                  "to be updated."));
}

void MainWindow::hideOSD()
{
    if (mpOSD)
    {
        if (mOSDTimer)
        {
            killTimer(mOSDTimer);
            mOSDTimer = 0;
        }
        mpOSD->setShowType(OSD::ShowType::ShowNone);
    }
}

void MainWindow::increaseVolume()
{
    if (mpPlayer/* && mpPlayer->audio() && mpPlayer->audio()->isAvailable()*/)
    {
        int newVolume = m_pWindowBottom->getVolumeSlider()->value() + 1;
        m_pWindowBottom->getVolumeSlider()->setValue(newVolume);
        if (mpPlayer->audio()->isMute())
        {
            toggleMuteState();
        }
        qDebug("vol = %.3f", mpPlayer->audio()->volume());
    }
}

void MainWindow::decreaseVolume()
{
    if (mpPlayer/* && mpPlayer->audio() && mpPlayer->audio()->isAvailable()*/)
    {
        if (mpPlayer->audio()->isMute())
        {
            toggleMuteState();
        }
        int newVolume = m_pWindowBottom->getVolumeSlider()->value() - 1;
        if (newVolume < 1)
        {
            toggleMuteState();
            m_pWindowBottom->getVolumeSlider()->setValue(0);
        }
        else
        {
            m_pWindowBottom->getVolumeSlider()->setValue(newVolume);
        }
        qDebug("vol = %.3f", mpPlayer->audio()->volume());
    }
}

void MainWindow::seekForward()
{
    if (mpPlayer && mpPlayer->isLoaded())
    {
        qDebug("->");
        mpPlayer->setSeekType(KeyFrameSeek/*AccurateSeek*/);
        mpPlayer->seekForward();
        if (mpOSD)
        {
            if (mOSDTimer)
            {
                killTimer(mOSDTimer);
                mOSDTimer = 0;
            }
            mpOSD->setShowType(OSD::ShowType::ShowCurrentAndTotalTime);
            mOSDTimer = startTimer(3000);
        }
    }
}

void MainWindow::seekBackward()
{
    if (mpPlayer && mpPlayer->isLoaded())
    {
        qDebug("<-");
        mpPlayer->setSeekType(KeyFrameSeek/*AccurateSeek*/);
        mpPlayer->seekBackward();
        if (mpOSD)
        {
            if (mOSDTimer)
            {
                killTimer(mOSDTimer);
                mOSDTimer = 0;
            }
            mpOSD->setShowType(OSD::ShowType::ShowCurrentAndTotalTime);
            mOSDTimer = startTimer(3000);
        }
    }
}

void MainWindow::escKeyPressed()
{
    if (windowState() == Qt::WindowFullScreen)
    {
        toggleFullscreen();
        return;
    }
    if (!mpPlayer->isLoaded())
    {
        close();
        return;
    }
    stopUnload();
}

void MainWindow::playNext()
{
    if (mpPlayList->rowCount() <= 1)
    {
        return;
    }
    play(mpPlayList->itemAt(1).url(), 1);
}

void MainWindow::playPrevious()
{
    if (mpPlayList->rowCount() <= 1)
    {
        return;
    }
    play(mpPlayList->itemAt(mpPlayList->rowCount() - 1).url(), -1);
}

void MainWindow::showOSDLocalTime()
{
    if (mpOSD)
    {
        if (Config::instance().getShowLocalTime())
        {
            mpOSD->setShowType(OSD::ShowType::ShowCurrentLocalTime);
        }
        else
        {
            mpOSD->setShowType(OSD::ShowType::ShowNone);
        }
    }
}

void MainWindow::help()
{
    if (!mpHelpBox)
    {
        return;
    }
    if (mpHelpBox->isVisible())
    {
        mpHelpBox->hide();
    }
    mpHelpBox->setWindowModality(Qt::ApplicationModal);
    mpHelpBox->show();
}

void MainWindow::loadExternalSubtitleFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }
    if (QFileInfo(fileName).exists() && QFileInfo(fileName).isFile())
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : \"")
                 << fileName.toUtf8().constData()
                 << QString::fromLatin1("\" loaded.");
        mpSubtitle->setFile(fileName);
    }
}

void MainWindow::seekNextChapter()
{
    if (!mpPlayer || !mpPlayer->isLoaded())
    {
        return;
    }
    mpPlayer->seekNextChapter();
}

void MainWindow::seekPreviousChapter()
{
    if (!mpPlayer || !mpPlayer->isLoaded())
    {
        return;
    }
    mpPlayer->seekPreviousChapter();
}

void MainWindow::destroyFullscreenControlPanel()
{
    if (!mpFullscreenControlPanel)
    {
        return;
    }
    mpFullscreenControlPanel->removeEventFilter(this);
    if (mpFullscreenControlPanel->isVisible())
    {
        mpFullscreenControlPanel->hide();
    }
    mpFullscreenControlPanel->RemoveBottomWidget();
    m_pWindowBottom->setParent(this);
    SetBottomWidget(m_pWindowBottom);
    mpTimeSlider->parentWidget()->layout()->removeWidget(mpTimeSlider);
    mpTimeSlider->setParent(this);
    mpPlayerLayout->layout()->addWidget(mpTimeSlider);
    mpFullscreenControlPanel->close();
    delete mpFullscreenControlPanel;
    mpFullscreenControlPanel = NULL;
}

void MainWindow::createShortcuts()
{
    QShortcut *aboutShortcut = new QShortcut(QKeySequence::HelpContents, this);
    connect(aboutShortcut, SIGNAL(activated()), this, SLOT(help()));

    QShortcut *setupShortcut = new QShortcut(QKeySequence::Refresh, this);
    connect(setupShortcut, SIGNAL(activated()), this, SLOT(setup()));

    QShortcut *quitShortcut = new QShortcut(QKeySequence::Cancel, this);
    connect(quitShortcut, SIGNAL(activated()), this, SLOT(escKeyPressed()));

    QShortcut *openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, SIGNAL(activated()), this, SLOT(openFile()));

    QShortcut *togglePlayPauseShortcut = new QShortcut(Qt::Key_Space, this);
    connect(togglePlayPauseShortcut, SIGNAL(activated()), this, SLOT(togglePlayPause()));

    QShortcut *toggleFullscreenShortcut = new QShortcut(QKeySequence::FullScreen, this);
    connect(toggleFullscreenShortcut, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleFullscreenShortcut2 = new QShortcut(Qt::Key_Return, this);
    connect(toggleFullscreenShortcut2, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleFullscreenShortcut3 = new QShortcut(Qt::Key_Enter, this);
    connect(toggleFullscreenShortcut3, SIGNAL(activated()), this
                                                              , SLOT(toggleFullscreen()));

    QShortcut *toggleMuteShortcut = new QShortcut(Qt::Key_M, this);
    connect(toggleMuteShortcut, SIGNAL(activated()), this, SLOT(toggleMuteState()));

    QShortcut *forwardShortcut = new QShortcut(Qt::Key_Right, this);
    connect(forwardShortcut, SIGNAL(activated()), this, SLOT(seekForward()));

    QShortcut *backwardShortcut = new QShortcut(Qt::Key_Left, this);
    connect(backwardShortcut, SIGNAL(activated()), this, SLOT(seekBackward()));

    QShortcut *increaseShortcut = new QShortcut(Qt::Key_Up, this);
    connect(increaseShortcut, SIGNAL(activated()), this, SLOT(increaseVolume()));

    QShortcut *decreaseShortcut = new QShortcut(Qt::Key_Down, this);
    connect(decreaseShortcut, SIGNAL(activated()), this, SLOT(decreaseVolume()));

    QShortcut *nextChapterShortcut = new QShortcut(QKeySequence::MoveToNextPage, this);
    connect(nextChapterShortcut, SIGNAL(activated()), this, SLOT(seekNextChapter()));

    QShortcut *previousChapterShortcut = new QShortcut(QKeySequence::MoveToPreviousPage
                                                                                  , this);
    connect(previousChapterShortcut, SIGNAL(activated()), this
                                                           , SLOT(seekPreviousChapter()));

    QShortcut *captureShortcut = new QShortcut(QKeySequence::Save, this);
    connect(captureShortcut, SIGNAL(activated()), m_pWindowBottom
            , SIGNAL(onVideoCaptureButtonClicked()));
}

void MainWindow::createFullScreenProgressBar()
{
    mpFullScreenProgressBar = new SProgressBar(this);
    mpFullScreenProgressBar->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint
                                                 | Qt::FramelessWindowHint);
    QDesktopWidget desktop;
    int desktopHeight = desktop.screen()->height();
    int desktopWidth = desktop.screen()->width();
    int h = static_cast<int>(static_cast<qreal>(desktopHeight) * 0.005);
    mpFullScreenProgressBar->setGeometry(0, desktopHeight - h, desktopWidth, h);
    mpFullScreenProgressBar->setFixedSize(mpFullScreenProgressBar->size());
    mpFullScreenProgressBar->setValue(0);
    mpFullScreenProgressBar->hide();
}

void MainWindow::createTrayIcon()
{
    mpTrayIcon = new QSystemTrayIcon(QIcon(":/Icon/SPlayer.ico"), this);
    mpTrayIcon->setContextMenu(mpMenu);
    mpTrayIcon->setToolTip(QString::fromLatin1(AppDisplayName));
    mpTrayIcon->show();
}

void MainWindow::addSimilarFilesToPlayList(const QString &filePath)
{
    if (filePath.isEmpty())
    {
        return;
    }
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(Common::supportedSuffixes());
    QFileInfoList fiList = dir.entryInfoList();
    if (fiList.count() <= 1)
    {
        return;
    }
    QFileInfoList m_playList;
    if (1)
    {
        QString fn = QFileInfo(filePath).completeBaseName();
        if (Common::isDigitStr(fn))
        {
            for (int i = 0; i <= (fiList.count() - 1); ++i)
            {
                QString newFn = fiList.at(i).completeBaseName();
                if (Common::isDigitStr(newFn))
                {
                    m_playList.append(fiList.at(i));
                }
            }
            if (m_playList.count() <= 1)
            {
                return;
            }
            else
            {
                goto Label_PlayList_Add_File_Names;
            }
        }
    }
    /*if (mpPlayList->rowCount() >= 1)
    {
        for (int i = 0; i <= (fiList.count() - 1); ++i)
        {
            for (int j = 0; j <= (mpPlayList->rowCount() - 1); ++j)
            {
                if (fiList.at(i).absoluteFilePath() == mpPlayList->itemAt(j).url())
                {
                    fiList.removeAt(i);
                }
            }
        }
    }
    if (fiList.count() <= 1)
    {
        return;
    }*/
    for (int i = 0; i <= (fiList.count() - 1); ++i)
    {
        QString originalBaseName = QFileInfo(filePath).baseName();
        QString currentBaseName = fiList.at(i).baseName();
        QString currentFirstName1 =
                fiList.at(i).fileName().split('-', QString::SkipEmptyParts
                                                      , Qt::CaseInsensitive).constFirst();
        QString originalFirstName1 =
                QFileInfo(filePath).fileName().split('-', QString::SkipEmptyParts
                                                      , Qt::CaseInsensitive).constFirst();
        QString currentFirstName2 =
                fiList.at(i).fileName().split('_', QString::SkipEmptyParts
                                                      , Qt::CaseInsensitive).constFirst();
        QString originalFirstName2 =
                QFileInfo(filePath).fileName().split('_', QString::SkipEmptyParts
                                                      , Qt::CaseInsensitive).constFirst();
        if (currentFirstName1.toLower() == originalFirstName1.toLower()
                || currentFirstName2.toLower() == originalFirstName2.toLower()
                || currentBaseName.toLower() == originalBaseName.toLower())
        {
            m_playList.append(fiList.at(i));
        }
    }
    if (m_playList.count() <= 1)
    {
        return;
    }

Label_PlayList_Add_File_Names:
    int curNum = m_playList.indexOf(QFileInfo(filePath));
    if (curNum < 0)
    {
        return;
    }
    int countNum = 0;
    for (int i = (curNum + 1); i <= (m_playList.count() - 1); ++i)
    {
        QString url = m_playList.at(i).absoluteFilePath();
        QString title = m_playList.at(i).fileName();
        PlayListItem item;
        item.setUrl(url);
        item.setTitle(title);
        item.setLastTime(0);
        mpPlayList->remove(url);
        mpPlayList->insertItemAt(item, countNum + 1);
        ++countNum;
    }
    for (int i = 0; i <= (curNum - 1); ++i)
    {
        QString url = m_playList.at(i).absoluteFilePath();
        QString title = m_playList.at(i).fileName();
        PlayListItem item;
        item.setUrl(url);
        item.setTitle(title);
        item.setLastTime(0);
        mpPlayList->remove(url);
        mpPlayList->insertItemAt(item, countNum + 1);
        ++countNum;
    }
}

void MainWindow::autoLoadExternalSubtitleFile(const QString &filePath)
{
    if (!mpPlayer)
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : AVPlayer is NULL.");
        return;
    }
    if (mpPlayer->subtitleStreamCount() > 0)
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : Already has ")
                 << QString::number(mpPlayer->subtitleStreamCount())
                 << QString::fromLatin1(" sub streams.");
        return;
    }
    if (filePath.isEmpty())
    {
        qDebug() << QString::fromLatin1("[Load external subtitle] : Empty file path : \"")
                 << filePath.toUtf8().constData()
                 << QString::fromLatin1("\"");
        return;
    }
    //下面这一段是查找和给定文件除后缀名外文件名完全相同的文件，找到就自动加载后返回
    QString fileDir = QFileInfo(filePath).absolutePath() + QString::fromLatin1("/");
    QString subFileBasePath = fileDir + QFileInfo(filePath).completeBaseName();
    QString subFileFullPath = subFileBasePath + QString::fromLatin1(".ass");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".srt");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".sub");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    subFileFullPath = subFileBasePath + QString::fromLatin1(".idx");
    if (QFileInfo(subFileFullPath).exists() && QFileInfo(subFileFullPath).isFile())
    {
        loadExternalSubtitleFile(subFileFullPath);
        return;
    }
    //下面这一段是查找和给定文件名字相似的文件，找到就自动加载第一个（如果有不止一个的话）后返回
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << QString::fromLatin1("*.ass")
            << QString::fromLatin1("*.srt")
            << QString::fromLatin1("*.idx")
            << QString::fromLatin1("*.sub");
    dir.setNameFilters(filters);
    QFileInfoList fiList = dir.entryInfoList();
    if (fiList.count() <= 0)
    {
        return;
    }
    for (int i = 0; i <= (fiList.count() - 1); ++i)
    {
        if (fiList.at(i).baseName().toLower() == QFileInfo(filePath).baseName().toLower())
        {
            loadExternalSubtitleFile(fiList.at(i).absoluteFilePath());
            return;
        }
    }
}

void MainWindow::showPreviewWindow()
{
    if (windowState() != Qt::WindowFullScreen)
    {
        QToolTip::showText(mCurrentCursorPosition, QTime(0, 0, 0)\
                           .addMSecs(mTimeSliderHoverValue)\
                           .toString(QString::fromLatin1("HH:mm:ss")));
    }
    else
    {
        QToolTip::showText(mapToGlobal(QCursor::pos()), QTime(0, 0, 0)\
                           .addMSecs(mTimeSliderHoverValue)\
                           .toString(QString::fromLatin1("HH:mm:ss")));
    }
    if (!Config::instance().previewEnabled())
    {
        return;
    }
    if (!canShowPreviewWindow)
    {
        return;
    }
    const int w = Config::instance().previewWidth();
    const int h = Config::instance().previewHeight();
    if (!m_preview)
    {
        m_preview = new VideoPreviewWidget();
        m_preview->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint
                                  | Qt::WindowStaysOnTopHint);
        m_preview->resize(w, h);
        m_preview->setFile(mpPlayer->file());
    }
    const qint64 interval = 5000;
    qint64 currentTimestamp = m_preview->timestamp();
    if (currentTimestamp >= mTimeSliderHoverValue - interval
            && currentTimestamp <= mTimeSliderHoverValue + interval)
    {
        return;
    }
    m_preview->setTimestamp(mTimeSliderHoverValue);
    m_preview->preview();
    if (windowState() != Qt::WindowFullScreen)
    {
        m_preview->move(mCurrentCursorPosition - QPoint(w/2, h));
    }
    else
    {
        QDesktopWidget desktop;
        QPoint fullscreenPos = mapToGlobal(QPoint(QCursor::pos().x(), 0))
                                      - QPoint(w/2, 0)
                                      + QPoint(0, desktop.screen()->height()
                                      - m_pWindowBottom->height()
                                      - mpTimeSlider->height() - h);
        m_preview->move(fullscreenPos);
    }
    m_preview->show();
}

void MainWindow::closePreviewWindow()
{
    if (mTimeSliderHoverTimer)
    {
        killTimer(mTimeSliderHoverTimer);
        mTimeSliderHoverTimer = 0;
    }
    canShowPreviewWindow = false;
    if (!m_preview)
    {
        return;
    }
    m_preview->close();
    delete m_preview;
    m_preview = NULL;
}

void MainWindow::workaroundRendererSize()
{
    if (!mpRenderer)
        return;
    QSize s = rect().size();
    //resize(QSize(s.width()-1, s.height()-1));
    //resize(s); //window resize to fullscreen size will create another fullScreenChange event
    mpRenderer->widget()->resize(QSize(s.width()+1, s.height()+1));
    mpRenderer->widget()->resize(s);
}

void MainWindow::createSkinManagerMenu()
{
    mpSkinManagerMenu = new QMenu(this);
    QAction *silverySkin = mpSkinManagerMenu->addAction(tr("Silvery"));
    silverySkin->setWhatsThis("Silvery");
    connect(silverySkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *blueSkin = mpSkinManagerMenu->addAction(tr("Blue"));
    blueSkin->setWhatsThis("Blue");
    connect(blueSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *lightBlueSkin = mpSkinManagerMenu->addAction(tr("Light Blue"));
    lightBlueSkin->setWhatsThis("Light Blue");
    connect(lightBlueSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *darkBlueSkin = mpSkinManagerMenu->addAction(tr("Dark Blue"));
    darkBlueSkin->setWhatsThis("Dark Blue");
    connect(darkBlueSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *graySkin = mpSkinManagerMenu->addAction(tr("Gray"));
    graySkin->setWhatsThis("Gray");
    connect(graySkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *lightGraySkin = mpSkinManagerMenu->addAction(tr("Light Gray"));
    lightGraySkin->setWhatsThis("Light Gray");
    connect(lightGraySkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *darkGraySkin = mpSkinManagerMenu->addAction(tr("Dark Gray"));
    darkGraySkin->setWhatsThis("Dark Gray");
    connect(darkGraySkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *blackSkin = mpSkinManagerMenu->addAction(tr("Black"));
    blackSkin->setWhatsThis("Black");
    connect(blackSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *lightBlackSkin = mpSkinManagerMenu->addAction(tr("Light Black"));
    lightBlackSkin->setWhatsThis("Light Black");
    connect(lightBlackSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *darkBlackSkin = mpSkinManagerMenu->addAction(tr("Dark Black"));
    darkBlackSkin->setWhatsThis("Dark Black");
    connect(darkBlackSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *psBlackSkin = mpSkinManagerMenu->addAction(tr("PS Black"));
    psBlackSkin->setWhatsThis("PS Black");
    connect(psBlackSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *flatBlackSkin = mpSkinManagerMenu->addAction(tr("Flat Black"));
    flatBlackSkin->setWhatsThis("Flat Black");
    connect(flatBlackSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
    QAction *flatWhiteSkin = mpSkinManagerMenu->addAction(tr("Flat White"));
    flatWhiteSkin->setWhatsThis("Flat White");
    connect(flatWhiteSkin, SIGNAL(triggered()), this, SLOT(changeWindowSkin()));
}

void MainWindow::createLogoMenu()
{
    mpLogoMenu = new QMenu(this);
    QAction *openLocalFileAction = mpLogoMenu->addAction(tr("Open file"));
    connect(openLocalFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
    QAction *openURLAction = mpLogoMenu->addAction(tr("Open URL"));
    connect(openURLAction, SIGNAL(triggered()), this, SLOT(openUrl()));
    mpLogoMenu->addSeparator();
    QAction *setupAction = mpLogoMenu->addAction(tr("Setup"));
    connect(setupAction, SIGNAL(triggered()), this, SLOT(setup()));
    QAction *aboutAction = mpLogoMenu->addAction(tr("About"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    mpLogoMenu->addSeparator();
    QAction *exitAction = mpLogoMenu->addAction(tr("Exit"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createLogoWidget(QWidget *parent)
{
    QHBoxLayout *pLogoLayout = new QHBoxLayout(parent);
    pLogoLayout->setContentsMargins(0,0,0,0);
    pLogoLayout->setSpacing(0);
    mpLogoWidget = new LogoWidget(parent);
    mpLogoWidget->SetLogoImage(Common::getImagePath(":/Image/Logo.png"));
    pLogoLayout->addWidget(mpLogoWidget);
    connect(mpLogoWidget, SIGNAL(onOpenLocalFileButtonClicked()), this, SLOT(openFile()));
    connect(mpLogoWidget, SIGNAL(onOpenURLButtonClicked()), this, SLOT(openUrl()));
}

void MainWindow::createJumpList()
{
    QWinJumpList *jumplist = new QWinJumpList(this);
    jumplist->recent()->setVisible(true);
}

void MainWindow::createTaskbar()
{
    mpTaskbarButton = new QWinTaskbarButton(this);
    mpTaskbarButton->setWindow(windowHandle());
    mpTaskbarProgress = mpTaskbarButton->progress();
}

void MainWindow::createThumbnailToolBar()
{
    mpThumbnailToolBar = new QWinThumbnailToolBar(this);
    mpThumbnailToolBar->setWindow(windowHandle());

    mpPlayToolButton = new QWinThumbnailToolButton(mpThumbnailToolBar);
    mpPlayToolButton->setEnabled(true);
    mpPlayToolButton->setToolTip(tr("Play"));
    mpPlayToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(mpPlayToolButton, SIGNAL(clicked()), this, SLOT(togglePlayPause()));

    mpNextToolButton = new QWinThumbnailToolButton(mpThumbnailToolBar);
    mpNextToolButton->setEnabled(true);
    mpNextToolButton->setToolTip(tr("Play next file"));
    mpNextToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(mpNextToolButton, SIGNAL(clicked()), this, SLOT(playNext()));

    mpPreviousToolButton = new QWinThumbnailToolButton(mpThumbnailToolBar);
    mpPreviousToolButton->setEnabled(true);
    mpPreviousToolButton->setToolTip(tr("Play previous file"));
    mpPreviousToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(mpPreviousToolButton, SIGNAL(clicked()), this, SLOT(playPrevious()));

    mpStopToolButton = new QWinThumbnailToolButton(mpThumbnailToolBar);
    mpStopToolButton->setEnabled(true);
    mpStopToolButton->setToolTip(tr("Stop"));
    mpStopToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(mpStopToolButton, SIGNAL(clicked()), this, SLOT(stopUnload()));

    mpMuteToolButton = new QWinThumbnailToolButton(mpThumbnailToolBar);
    mpMuteToolButton->setEnabled(true);
    mpMuteToolButton->setToolTip(tr("Mute"));
    mpMuteToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(mpMuteToolButton, SIGNAL(clicked()), this, SLOT(toggleMuteState()));

    mpThumbnailToolBar->addButton(mpStopToolButton);
    mpThumbnailToolBar->addButton(mpPreviousToolButton);
    mpThumbnailToolBar->addButton(mpPlayToolButton);
    mpThumbnailToolBar->addButton(mpNextToolButton);
    mpThumbnailToolBar->addButton(mpMuteToolButton);
}
