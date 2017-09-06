#pragma once

#include "PCH.h"
#include <QUrl>
#include <QMenu>
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinJumpList>
#include <QSystemTrayIcon>
#include "StandardDialog/StandardDialog.h"
#include "Control/SButton.h"
#include "WindowBottom.h"
#include "LogoWidget.h"
#include "Control/SProgressBar.h"
#include "config/ConfigDialog.h"
#include "AboutBox.h"
#include "HelpBox.h"
#include "DonateBox.h"

QT_BEGIN_NAMESPACE
class QWidgetAction;
class QToolButton;
QT_END_NAMESPACE
namespace QtAV {
class AudioOutput;
class AVError;
class AVPlayer;
class AVClock;
class VideoRenderer;
class LibAVFilterAudio;
class LibAVFilterVideo;
class SubtitleFilter;
class VideoPreviewWidget;
class DynamicShaderObject;
}
QT_BEGIN_NAMESPACE
class QMenu;
class QTimeEdit;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QTimeEdit;
QT_END_NAMESPACE
class Button;
class Slider;
class PlayList;
class DecoderConfigPage;
class VideoEQConfigPage;
class StatisticsView;
class OSDFilter;
class AVFilterSubtitle;
class Preview;

class MainWindow : public StandardDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setAudioBackends(const QStringList& backends);
    bool setRenderer(QtAV::VideoRenderer* renderer);
    void setVideoDecoderNames(const QStringList& vd);

public slots:
    void play(const QString& name, const int &command = 0);
    void play(const QUrl& url);
    void openFile();
    void togglePlayPause();
    void cleanHistory();

signals:
    void ready();

private slots:
    void stopUnload();
    void about();
    void aboutQtAV();
    void openUrl();
    void initAudioTrackMenu();
    void updateChannelMenu();
    void switchAspectRatio(QAction* action);
    void toggleRepeat(bool);
    void setRepeateMax(int);
    void changeVO(QAction* action);
    void changeChannel(QAction* action);
    void changeAudioTrack(QAction* action);
    void onPlayListClick(const QString& key, const QString& value);
    void processPendingActions();
    void initPlayer();
    void setupUi();
    void onSpinBoxChanged(double v);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void setFrameRate();
    void seek();
    void seek(int);
    void setVolume();
    void setVolume(bool isDevUse);
    void showInfo();
    void onPositionChange(qint64 pos);
    void repeatAChanged(const QTime& t);
    void repeatBChanged(const QTime& t);

    void onTimeSliderHover(int pos, int value);
    void onTimeSliderLeave();
    void handleError(const QtAV::AVError& e);
    void onMediaStatusChanged();
    void onBufferProgress(qreal percent);

    void onVideoEQEngineChanged();
    void onBrightnessChanged(int b);
    void onContrastChanged(int c);
    void onHueChanged(int h);
    void onSaturationChanged(int s);

    void onSeekFinished(qint64 pos);
    void onCaptureConfigChanged();
    void onAVFilterVideoConfigChanged();
    void onAVFilterAudioConfigChanged();
    void onBufferValueChanged();
    void onAbortOnTimeoutChanged();

    void onUserShaderChanged();

    void donate();
    void setup();

    void handleFullscreenChange();
    void toggoleSubtitleEnabled(bool value);
    void toggleSubtitleAutoLoad(bool value);
    void openSubtitle();
    void setSubtitleCharset(const QString& charSet);
    void setSubtitleEngine(const QString& value);

    void changeClockType(QAction* action);

    void onShowPopupMenu(const QPoint &p);
    void toggleFullscreen();
    void toggleFullscreen(bool isDevUse);

    void changeWindowSizeToVideoSize(const int &w, const int &h);
    void moveWindowToDesktopCenter();
    void changeWindowSkin();

    void Slot_SkinClicked();
    void Slot_LogoClicked();
    void Slot_StayOnTopClicked();

    void setMute(bool canMute);
    void toggleMuteState();

    void showFullscreenControlPanel();
    void hideFullscreenControlPanel();

    void updateLanguage();
    void updateSkin();

    void hideOSD();

    void increaseVolume();
    void decreaseVolume();
    void seekForward();
    void seekBackward();

    void escKeyPressed();

    void playNext();
    void playPrevious();

    void showOSDLocalTime();

    void help();

    void loadExternalSubtitleFile(const QString &fileName);

    void seekNextChapter();
    void seekPreviousChapter();

    void showPreviewWindow(const int value, const QPoint gpos);
    void hidePreviewWindow();

protected:
    void closeEvent(QCloseEvent *e);
    void timerEvent(QTimerEvent *e);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    //bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void changeEvent(QEvent *event);

private:
    void workaroundRendererSize();
    void createSkinManagerMenu();
    void createLogoMenu();
    void createLogoWidget(QWidget *parent);
    void createJumpList();
    void createTaskbar();
    void createThumbnailToolBar();
    void createFullscreenControlPanel();
    void destroyFullscreenControlPanel();
    void createShortcuts();
    void createFullScreenProgressBar();
    void createTrayIcon();
    void addSimilarFilesToPlayList(const QString &filePath);
    void autoLoadExternalSubtitleFile(const QString &filePath);

private:
    bool mIsReady, mHasPendingPlay;
    bool mbStayOnTop;
    int mCursorTimer, mOSDTimer, mTimeSliderHoverTimer, mTimeSliderLeaveTimer;
    int mRepeateMax;
    QStringList mAudioBackends;
    QVBoxLayout *mpPlayerLayout;

    Slider *mpTimeSlider;
    QMenu *mpMenu;
    QAction *mpVOAction, *mpARAction; //remove mpVOAction if vo.id() is supported
    QAction *mpRepeatEnableAction;
    QWidgetAction *mpRepeatAction;
    QSpinBox *mpRepeatBox;
    QTimeEdit *mpRepeatA, *mpRepeatB;
    QAction *mpAudioTrackAction;
    QMenu *mpAudioTrackMenu;
    QMenu *mpChannelMenu;
    QAction *mpChannelAction;
    QList<QAction*> mVOActions;

    WindowBottom *m_pWindowBottom;

    //QtAV::AVClock *mpClock;
    QtAV::AVPlayer *mpPlayer;
    QtAV::VideoRenderer *mpRenderer;
    QtAV::LibAVFilterVideo *mpVideoFilter;
    QtAV::LibAVFilterAudio *mpAudioFilter;
    QString mFile;
    QString mTitle;

    DecoderConfigPage *mpDecoderConfigPage;
    VideoEQConfigPage *mpVideoEQ;

    PlayList *mpPlayList, *mpHistory;

    StatisticsView *mpStatisticsView;

    OSDFilter *mpOSD;
    QtAV::SubtitleFilter *mpSubtitle;
    QtAV::VideoPreviewWidget *m_preview;
    QtAV::DynamicShaderObject *m_shader;

    QMenu *mpSkinManagerMenu;
    QMenu *mpLogoMenu;

    LogoWidget *mpLogoWidget;

    QWinTaskbarButton *mpTaskbarButton;
    QWinTaskbarProgress *mpTaskbarProgress;
    QWinThumbnailToolBar *mpThumbnailToolBar;
    QWinThumbnailToolButton *mpPlayToolButton;
    QWinThumbnailToolButton *mpNextToolButton;
    QWinThumbnailToolButton *mpPreviousToolButton;
    QWinThumbnailToolButton *mpStopToolButton;
    QWinThumbnailToolButton *mpMuteToolButton;

    StandardDialog *mpFullscreenControlPanel;

    SProgressBar *mpFullScreenProgressBar;

    QSystemTrayIcon *mpTrayIcon;

    ConfigDialog *mpConfigDialog;
    AboutBox *mpAboutBox;
    HelpBox *mpHelpBox;
    DonateBox *mpDonateBox;
};
