#pragma once

#include <QLabel>
#include "Control/SButton.h"
#include "Control/BaseWidget.h"
#include "Slider.h"

class WindowBottom : public BaseWidget
{
    Q_OBJECT

public:
    WindowBottom(QWidget *parent = nullptr);
    virtual ~WindowBottom();

    QLabel *getCurrentDurationLabel();
    QLabel *getTotalDurationLabel();
    QWidget *getVideoCaptureButton();
    Slider *getVolumeSlider();

    void changePlayButtonToPauseButton();
    void changePauseButtonToPlayButton();
    void changeMuteButtonToMutedButton();
    void changeMutedButtonToMuteButton();
    void changeFullscreenButtonToExitFullscreenButton();
    void changeExitFullscreenButtonToFullscreenButton();

signals:
    void onPlayButtonClicked();
    void onSetupButtonClicked();
    void onStopButtonClicked();
    void onFullscreenButtonClicked();
    void onMuteButtonClicked();
    void onOpenFileButtonClicked();
    void onVideoCaptureButtonClicked();
    void onPreviousButtonClicked();
    void onNextButtonClicked();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QLabel *m_pCurrentDurationLabel;
    QLabel *m_pSeperatorLabel;
    QLabel *m_pTotalDurationLabel;
    SButton *m_pStopButton;
    SButton *m_pPreviousButton;
    SButton *m_pPlayButton;
    SButton *m_pNextButton;
    SButton *m_pMuteButton;
    SButton *m_pSetupButton;
    SButton *m_pOpenFileButton;
    SButton *m_pVideoCaptureButton;
    Slider *m_pVolumeSlider;
    SButton *m_pFullscreenButton;

    const int kVolumeSliderMax = 100;
    const qreal kVolumeInterval = 0.04;
};
