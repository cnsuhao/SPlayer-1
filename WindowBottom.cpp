#include "WindowBottom.h"
#include "common/Common.h"

//播放、停止等主界面底部视图的效果呈现
WindowBottom::WindowBottom(QWidget *parent):BaseWidget(parent)
{
    QImage image;
    m_pCurrentDurationLabel = new QLabel(this);
    m_pCurrentDurationLabel->setText("00:00:00");
    //m_pCurrentDurationLabel->setMouseTracking(true);
    m_pCurrentDurationLabel->setToolTip(tr("Current duration"));
    m_pSeperatorLabel = new QLabel(this);
    m_pSeperatorLabel->setText("/");
    m_pTotalDurationLabel = new QLabel(this);
    m_pTotalDurationLabel->setText("00:00:00");
    m_pTotalDurationLabel->setToolTip(tr("Total duration"));
    //图片按钮
    m_pSetupButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/SetupButton.png"));
    m_pSetupButton->setFixedSize(image.width()/4,image.height());
    m_pSetupButton->AppendImage(image);
    m_pSetupButton->setToolTip(tr("Setup"));

    m_pOpenFileButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/OpenFileSmallButton.png"));
    m_pOpenFileButton->setFixedSize(image.width()/4,image.height());
    m_pOpenFileButton->AppendImage(image);
    m_pOpenFileButton->setToolTip(tr("Open file"));

    m_pStopButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/StopButton.png"));
    m_pStopButton->setFixedSize(image.width()/4,image.height());
    m_pStopButton->AppendImage(image);
    m_pStopButton->setToolTip(tr("Stop"));

    m_pPreviousButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/PreviousButton.png"));
    m_pPreviousButton->setFixedSize(image.width()/4,image.height());
    m_pPreviousButton->AppendImage(image);
    m_pPreviousButton->setToolTip(tr("Play previous file"));

    m_pPlayButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/PlayButton.png"));
    m_pPlayButton->setFixedSize(image.width()/4,image.height());
    m_pPlayButton->AppendImage(image);
    m_pPlayButton->setToolTip(tr("Play"));

    m_pNextButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/NextButton.png"));
    m_pNextButton->setFixedSize(image.width()/4,image.height());
    m_pNextButton->AppendImage(image);
    m_pNextButton->setToolTip(tr("Play next file"));

    m_pVideoCaptureButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/VideoCaptureButton.png"));
    m_pVideoCaptureButton->setFixedSize(image.width()/4,image.height());
    m_pVideoCaptureButton->AppendImage(image);
    m_pVideoCaptureButton->setToolTip(tr("Capture"));

    m_pMuteButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/MuteButton.png"));
    m_pMuteButton->setFixedSize(image.width()/4,image.height());
    m_pMuteButton->AppendImage(image);
    m_pMuteButton->setToolTip(tr("Mute"));

    m_pVolumeSlider = new Slider(this);
    m_pVolumeSlider->setFixedSize(60,4);
    m_pVolumeSlider->setOrientation(Qt::Horizontal);
    m_pVolumeSlider->setMinimum(0);
    m_pVolumeSlider->setMaximum(kVolumeSliderMax);
    m_pVolumeSlider->setValue(int(1.0/kVolumeInterval*qreal(kVolumeSliderMax)/100.0));

    m_pFullscreenButton = new SButton(this);
    image = QImage(Common::getImagePath(":/Image/FullscreenButton.png"));
    m_pFullscreenButton->setFixedSize(image.width()/4,image.height());
    m_pFullscreenButton->AppendImage(image);
    m_pFullscreenButton->setToolTip(tr("Fullscreen"));

    //水平布局
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(this);
    pHBoxLayout->setContentsMargins(10,0,10,0);
    pHBoxLayout->setSpacing(0);
    //依次添加Stretch以及Widget
    pHBoxLayout->addStretch(1);
    pHBoxLayout->addWidget(m_pSetupButton);
    pHBoxLayout->addSpacing(10);
    pHBoxLayout->addWidget(m_pOpenFileButton);
    pHBoxLayout->addSpacing(10);
    pHBoxLayout->addWidget(m_pStopButton);
    pHBoxLayout->addSpacing(10);
    pHBoxLayout->addWidget(m_pPreviousButton);
    pHBoxLayout->addWidget(m_pPlayButton);
    pHBoxLayout->addWidget(m_pNextButton);
    pHBoxLayout->addSpacing(10);
    pHBoxLayout->addWidget(m_pVideoCaptureButton);
    pHBoxLayout->addSpacing(10);
    pHBoxLayout->addWidget(m_pMuteButton);
    pHBoxLayout->addStretch(1);

    connect(m_pSetupButton, SIGNAL(Signal_Clicked()), this, SIGNAL(onSetupButtonClicked()));
    connect(m_pOpenFileButton, SIGNAL(Signal_Clicked()), this
            , SIGNAL(onOpenFileButtonClicked()));
    connect(m_pStopButton, SIGNAL(Signal_Clicked()), this, SIGNAL(onStopButtonClicked()));
    connect(m_pPreviousButton, SIGNAL(Signal_Clicked()), this
            , SIGNAL(onPreviousButtonClicked()));
    connect(m_pPlayButton, SIGNAL(Signal_Clicked()), this, SIGNAL(onPlayButtonClicked()));
    connect(m_pNextButton, SIGNAL(Signal_Clicked()), this, SIGNAL(onNextButtonClicked()));
    connect(m_pVideoCaptureButton, SIGNAL(Signal_Clicked()), this
            , SIGNAL(onVideoCaptureButtonClicked()));
    connect(m_pMuteButton, SIGNAL(Signal_Clicked()), this, SIGNAL(onMuteButtonClicked()));
    connect(m_pFullscreenButton, SIGNAL(Signal_Clicked()), this
            , SIGNAL(onFullscreenButtonClicked()));
}

WindowBottom::~WindowBottom()
{
    if (m_pVolumeSlider)
    {
        m_pVolumeSlider->close();
        delete m_pVolumeSlider;
        m_pVolumeSlider = NULL;
    }
}

QLabel *WindowBottom::getCurrentDurationLabel()
{
    return m_pCurrentDurationLabel;
}

QLabel *WindowBottom::getTotalDurationLabel()
{
    return m_pTotalDurationLabel;
}

QWidget *WindowBottom::getVideoCaptureButton()
{
    return m_pVideoCaptureButton;
}

Slider *WindowBottom::getVolumeSlider()
{
    return m_pVolumeSlider;
}

void WindowBottom::changePlayButtonToPauseButton()
{
    m_pPlayButton->ChangeImage(0, QImage(Common::getImagePath(":/Image/PauseButton.png")));
    m_pPlayButton->setToolTip(tr("Pause"));
}

void WindowBottom::changePauseButtonToPlayButton()
{
    m_pPlayButton->ChangeImage(0, QImage(Common::getImagePath(":/Image/PlayButton.png")));
    m_pPlayButton->setToolTip(tr("Play"));
}

void WindowBottom::changeMuteButtonToMutedButton()
{
    m_pMuteButton->ChangeImage(0, QImage(Common::getImagePath(":/Image/MutedButton.png")));
    m_pMuteButton->setToolTip(tr("Sound"));
}

void WindowBottom::changeMutedButtonToMuteButton()
{
    m_pMuteButton->ChangeImage(0, QImage(Common::getImagePath(":/Image/MuteButton.png")));
    m_pMuteButton->setToolTip(tr("Mute"));
}

void WindowBottom::changeFullscreenButtonToExitFullscreenButton()
{
    m_pFullscreenButton->ChangeImage(0
                       , QImage(Common::getImagePath(":/Image/ExitFullscreenButton.png")));
    m_pFullscreenButton->setToolTip(tr("Exit from fullscreen"));
}

void WindowBottom::changeExitFullscreenButtonToFullscreenButton()
{
    m_pFullscreenButton->ChangeImage(0
                            , QImage(Common::getImagePath(":/Image/FullscreenButton.png")));
    m_pFullscreenButton->setToolTip(tr("Fullscreen"));
}

//自动布局与手动布局结合方式
void WindowBottom::resizeEvent(QResizeEvent *event)
{
    int textWidth = QFontMetrics(font()).width("00:00:00");
    int textHeight = QFontMetrics(font()).height();

    m_pCurrentDurationLabel->setGeometry(10,(height()-textHeight)/2,textWidth,textHeight);
    m_pSeperatorLabel->setGeometry(m_pCurrentDurationLabel->geometry().right()+5,m_pCurrentDurationLabel->y(),QFontMetrics(font()).width("/"),textHeight);
    m_pTotalDurationLabel->setGeometry(m_pSeperatorLabel->geometry().right()+5,m_pCurrentDurationLabel->y(),textWidth,textHeight);

    m_pVolumeSlider->move(m_pMuteButton->geometry().right(),(height()-4)/2);
    m_pFullscreenButton->move(geometry().right()-10-m_pFullscreenButton->width(),(height()-m_pFullscreenButton->height())/2);

    QWidget::resizeEvent(event);
}
