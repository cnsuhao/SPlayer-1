#pragma once

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QColor>
#include <QFont>

//TODO: use hash to simplify api
/*
 * MVC model. signals from Config notify ui update. signals from ui does not change Config unless ui changes applyed by XXXPage.apply()
 * signals from ui will emit Config::xxxChanged() with the value in ui. ui cancel the change also emit it with the value stores in Config.
 * apply() will change the value in Config
 */

class Config : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)
    // last file opened by file dialog
    Q_PROPERTY(QString lastFile READ lastFile WRITE setLastFile NOTIFY lastFileChanged)
    Q_PROPERTY(qreal forceFrameRate READ forceFrameRate WRITE setForceFrameRate NOTIFY forceFrameRateChanged)
    Q_PROPERTY(QStringList decoderPriorityNames READ decoderPriorityNames WRITE setDecoderPriorityNames NOTIFY decoderPriorityNamesChanged)
    Q_PROPERTY(bool zeroCopy READ zeroCopy WRITE setZeroCopy NOTIFY zeroCopyChanged)
    Q_PROPERTY(QString captureDir READ captureDir WRITE setCaptureDir NOTIFY captureDirChanged)
    Q_PROPERTY(QString captureFormat READ captureFormat WRITE setCaptureFormat NOTIFY captureFormatChanged)
    Q_PROPERTY(int captureQuality READ captureQuality WRITE setCaptureQuality NOTIFY captureQualityChanged)
    Q_PROPERTY(QStringList subtitleEngines READ subtitleEngines WRITE setSubtitleEngines NOTIFY subtitleEnginesChanged)
    Q_PROPERTY(bool subtitleAutoLoad READ subtitleAutoLoad WRITE setSubtitleAutoLoad NOTIFY subtitleAutoLoadChanged)
    Q_PROPERTY(bool subtitleEnabled READ subtitleEnabled WRITE setSubtitleEnabled NOTIFY subtitleEnabledChanged)
    Q_PROPERTY(QFont subtitleFont READ subtitleFont WRITE setSubtitleFont NOTIFY subtitleFontChanged)
    Q_PROPERTY(QColor subtitleColor READ subtitleColor WRITE setSubtitleColor NOTIFY subtitleColorChanged)
    Q_PROPERTY(QColor subtitleOutlineColor READ subtitleOutlineColor WRITE setSubtitleOutlineColor NOTIFY subtitleOutlineColorChanged)
    Q_PROPERTY(bool subtitleOutline READ subtitleOutline WRITE setSubtitleOutline NOTIFY subtitleOutlineChanged)
    Q_PROPERTY(int subtitleBottomMargin READ subtitleBottomMargin WRITE setSubtitleBottomMargin NOTIFY subtitleBottomMarginChanged)
    Q_PROPERTY(qreal subtitleDelay READ subtitleDelay WRITE setSubtitleDelay NOTIFY subtitleDelayChanged)
    // font properties for libass engine
    Q_PROPERTY(QString assFontFile READ assFontFile WRITE setAssFontFile NOTIFY assFontFileChanged)
    Q_PROPERTY(QString assFontsDir READ assFontsDir WRITE setAssFontsDir NOTIFY assFontsDirChanged)
    Q_PROPERTY(bool assFontFileForced READ isAssFontFileForced WRITE setAssFontFileForced NOTIFY assFontFileForcedChanged)

    Q_PROPERTY(bool previewEnabled READ previewEnabled WRITE setPreviewEnabled NOTIFY previewEnabledChanged)
    Q_PROPERTY(int previewWidth READ previewWidth WRITE setPreviewWidth NOTIFY previewWidthChanged)
    Q_PROPERTY(int previewHeight READ previewHeight WRITE setPreviewHeight NOTIFY previewHeightChanged)
    Q_PROPERTY(bool EGL READ isEGL WRITE setEGL NOTIFY EGLChanged)
    Q_PROPERTY(OpenGLType openGLType READ openGLType WRITE setOpenGLType NOTIFY openGLTypeChanged)
    Q_PROPERTY(QString ANGLEPlatform READ getANGLEPlatform WRITE setANGLEPlatform NOTIFY ANGLEPlatformChanged)
    Q_PROPERTY(bool avformatOptionsEnabled READ avformatOptionsEnabled WRITE setAvformatOptionsEnabled NOTIFY avformatOptionsEnabledChanged)
    Q_PROPERTY(qreal timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(int bufferValue READ bufferValue WRITE setBufferValue NOTIFY bufferValueChanged)
    Q_ENUMS(OpenGLType)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

    Q_PROPERTY(bool userShaderEnabled READ userShaderEnabled WRITE setUserShaderEnabled NOTIFY userShaderEnabledChanged)
    Q_PROPERTY(bool intermediateFBO READ intermediateFBO WRITE setIntermediateFBO NOTIFY intermediateFBOChanged)
    Q_PROPERTY(QString fragHeader READ fragHeader WRITE setFragHeader NOTIFY fragHeaderChanged)
    Q_PROPERTY(QString fragSample READ fragSample WRITE setFragSample NOTIFY fragSampleChanged)
    Q_PROPERTY(QString fragPostProcess READ fragPostProcess WRITE setFragPostProcess NOTIFY fragPostProcessChanged)

    Q_PROPERTY(QString skin READ getSkin WRITE setSkin NOTIFY skinChanged)
    Q_PROPERTY(int lastVolume READ getLastVolume WRITE setLastVolume NOTIFY lastVolumeChanged)
    Q_PROPERTY(bool saveHistoryState READ getSaveHistoryState WRITE setSaveHistoryState NOTIFY saveHistoryStateChanged)
    Q_PROPERTY(QString videoRenderer READ getVideoRenderer WRITE setVideoRenderer NOTIFY videoRendererChanged)
    Q_PROPERTY(bool showLocalTime READ getShowLocalTime WRITE setShowLocalTime NOTIFY showLocalTimeChanged)

public:
    enum OpenGLType // currently only for windows
    {
        Auto,
        Desktop,
        OpenGLES,
        Software
    };

    static Config& instance();
    static void setName(const QString& name); // config file base name
    static QString getName();
    /*!
     * \brief defaultConfigFile
     * Config file name is $appname.ini. Must call Config::setName() first
     */
    static QString defaultConfigFile();
    static QString defaultDir();
    Q_INVOKABLE bool reset();
    void reload();
    //void loadFromFile(const QString& file);

    QString lastFile() const;
    Config& setLastFile(const QString& value);

    qreal forceFrameRate() const;
    Config& setForceFrameRate(qreal value);
    // in priority order. the same order as displayed in ui
    QStringList decoderPriorityNames() const;
    Config& setDecoderPriorityNames(const QStringList& names);

    bool zeroCopy() const;
    Config& setZeroCopy(bool value);

    QString captureDir() const;
    Config& setCaptureDir(const QString& dir);

    /*!
     * \brief captureFormat
     *  can be "yuv" to capture yuv image without convertion. the suffix is the yuv format, e.g. "yuv420p", "nv12"
     *  or can be "jpg", "png"
     * \return
     */
    QString captureFormat() const;
    Config& setCaptureFormat(const QString& format);
    // only works for non-yuv capture. value: -1~100, -1: default
    int captureQuality() const;
    Config& setCaptureQuality(int quality);

    QStringList subtitleEngines() const;
    Config& setSubtitleEngines(const QStringList& value);
    bool subtitleAutoLoad() const;
    Config& setSubtitleAutoLoad(bool value);
    bool subtitleEnabled() const;
    Config& setSubtitleEnabled(bool value);

    QFont subtitleFont() const;
    Config& setSubtitleFont(const QFont& value);
    bool subtitleOutline() const;
    Config& setSubtitleOutline(bool value);
    QColor subtitleColor() const;
    Config& setSubtitleColor(const QColor& value);
    QColor subtitleOutlineColor() const;
    Config& setSubtitleOutlineColor(const QColor& value);
    int subtitleBottomMargin() const;
    Config& setSubtitleBottomMargin(int value);

    qreal subtitleDelay() const;
    Config& setSubtitleDelay(qreal value);

    QString assFontFile() const;
    Config& setAssFontFile(const QString& value);
    QString assFontsDir() const;
    Config& setAssFontsDir(const QString& value);
    bool isAssFontFileForced() const;
    Config& setAssFontFileForced(bool value);

    bool previewEnabled() const;
    Config& setPreviewEnabled(bool value);
    int previewWidth() const;
    Config& setPreviewWidth(int value);
    int previewHeight() const;
    Config& setPreviewHeight(int value);

    QVariantHash avformatOptions() const;
    bool avformatOptionsEnabled() const;
    Config& setAvformatOptionsEnabled(bool value);
    int analyzeDuration() const;
    Config& analyzeDuration(int ad);
    unsigned int probeSize() const;
    Config& probeSize(unsigned int ps);
    bool reduceBuffering() const;
    Config& reduceBuffering(bool y);
    QString avformatExtra() const;
    Config& avformatExtra(const QString& text);

    QString avfilterVideoOptions() const;
    Config& avfilterVideoOptions(const QString& options);
    bool avfilterVideoEnable() const;
    Config& avfilterVideoEnable(bool e);

    QString avfilterAudioOptions() const;
    Config& avfilterAudioOptions(const QString& options);
    bool avfilterAudioEnable() const;
    Config& avfilterAudioEnable(bool e);

    // currently only for xcb
    bool isEGL() const;
    Config& setEGL(bool value);
    // can be "Desktop", "OpenGLES", "Software"
    OpenGLType openGLType() const;
    Config& setOpenGLType(OpenGLType value);

    QString getANGLEPlatform() const;
    Config& setANGLEPlatform(const QString &value);

    // ms >0. default 30000ms
    qreal timeout() const;
    Config& setTimeout(qreal value);

    bool abortOnTimeout() const;
    Config& setAbortOnTimeout(bool value);

    // <0: auto
    int bufferValue() const;
    Config& setBufferValue(int value);

    QString language() const;
    Config& setLanguage(const QString& value);

    Q_INVOKABLE QVariant operator ()(const QString& key) const;
    Q_INVOKABLE Config& operator ()(const QString& key, const QVariant& value);

    /// history will not be clear in reset()
    QVariantList history() const;
    // {url: urlString, start: ms, duration: ms}
    Q_INVOKABLE void addHistory(const QVariantMap& value);
    Q_INVOKABLE void removeHistory(const QString& url);
    Q_INVOKABLE void clearHistory();

    Config& setUserShaderEnabled(bool value);
    bool userShaderEnabled() const;
    Config& setIntermediateFBO(bool value);
    bool intermediateFBO() const;
    Config& setFragHeader(const QString& text);
    QString fragHeader() const;
    Config& setFragSample(const QString& text);
    QString fragSample() const;
    Config& setFragPostProcess(const QString& text);
    QString fragPostProcess() const;

    QString getSkin() const;
    Config& setSkin(const QString& newSkin);
    bool getIsMuted() const;
    Config& setIsMuted(const bool& isMuted);
    int getLastVolume() const;
    Config& setLastVolume(const int& mLastVolume);
    bool getIsAlwaysCheckAssoc() const;
    Config& setIsAlwaysCheckAssoc(const bool& isAlwaysCheck);
    bool getSaveHistoryState() const;
    Config& setSaveHistoryState(const bool& shouldSave);
    QString getVideoRenderer() const;
    Config& setVideoRenderer(const QString& newVideoRenderer);
    bool getShowLocalTime() const;
    Config& setShowLocalTime(const bool& shouldShow);

public:
    Q_SIGNAL void changed();
    Q_SIGNAL void userShaderEnabledChanged();
    Q_SIGNAL void intermediateFBOChanged();
    Q_SIGNAL void fragHeaderChanged();
    Q_SIGNAL void fragSampleChanged();
    Q_SIGNAL void fragPostProcessChanged();

    Q_SIGNAL void lastFileChanged();
    //keyword 'signals' maybe protected. we need call the signals in other classes. Q_SIGNAL is empty
    Q_SIGNAL void forceFrameRateChanged();
    Q_SIGNAL void decodingThreadsChanged(int n);
    Q_SIGNAL void decoderPriorityNamesChanged();
    Q_SIGNAL void registeredDecodersChanged(const QVector<int>& r);
    Q_SIGNAL void zeroCopyChanged();
    Q_SIGNAL void captureDirChanged(const QString& dir);
    Q_SIGNAL void captureFormatChanged(const QString& fmt);
    Q_SIGNAL void captureQualityChanged(int quality);
    Q_SIGNAL void avfilterVideoChanged();
    Q_SIGNAL void avfilterAudioChanged();
    Q_SIGNAL void subtitleEnabledChanged();
    Q_SIGNAL void subtitleAutoLoadChanged();
    Q_SIGNAL void subtitleEnginesChanged();
    Q_SIGNAL void subtitleFontChanged();
    Q_SIGNAL void subtitleColorChanged();
    Q_SIGNAL void subtitleOutlineChanged();
    Q_SIGNAL void subtitleOutlineColorChanged();
    Q_SIGNAL void subtitleBottomMarginChanged();
    Q_SIGNAL void subtitleDelayChanged();
    Q_SIGNAL void assFontFileChanged();
    Q_SIGNAL void assFontsDirChanged();
    Q_SIGNAL void assFontFileForcedChanged();
    Q_SIGNAL void previewEnabledChanged();
    Q_SIGNAL void previewWidthChanged();
    Q_SIGNAL void previewHeightChanged();
    Q_SIGNAL void EGLChanged();
    Q_SIGNAL void openGLTypeChanged();
    Q_SIGNAL void ANGLEPlatformChanged();
    Q_SIGNAL void avformatOptionsEnabledChanged();
    Q_SIGNAL void bufferValueChanged();
    Q_SIGNAL void timeoutChanged();
    Q_SIGNAL void abortOnTimeoutChanged();
    Q_SIGNAL void languageChanged();
    Q_SIGNAL void historyChanged();

    Q_SIGNAL void skinChanged();
    Q_SIGNAL void lastVolumeChanged();
    Q_SIGNAL void saveHistoryStateChanged();
    Q_SIGNAL void videoRendererChanged();
    Q_SIGNAL void showLocalTimeChanged();

protected:
    explicit Config(QObject *parent = nullptr);
    ~Config();

public Q_SLOTS:
    void save();

private:
    class Data;
    Data *mpData;
};
