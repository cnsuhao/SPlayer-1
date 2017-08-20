#include "Common.h"
#include <QMimeDatabase>
#include <QSettings>
#include <QtDebug>
#include <QTranslator>
#include "common/Config.h"
#include <QCoreApplication>
#include <QtGlobal>
#include "Style/SStyle.h"

Common::Common()
{

}

QStringList Common::supportedMimeTypes()
{
    return (QStringList() << QString::fromLatin1("video/3gpp")
                          << QString::fromLatin1("video/x-flv")
                          << QString::fromLatin1("video/x-matroska")
                          << QString::fromLatin1("application/vnd.rn-realmedia")
                          << QString::fromLatin1("video/dl")
                          << QString::fromLatin1("video/dv")
                          << QString::fromLatin1("video/fli")
                          << QString::fromLatin1("video/m4v")
                          << QString::fromLatin1("video/vnd.mpegurl")
                          << QString::fromLatin1("video/webm")
                          << QString::fromLatin1("video/x-ms-wmv")
                          << QString::fromLatin1("video/x-ms-asf")
                          << QString::fromLatin1("video/x-msvideo")
                          << QString::fromLatin1("video/isivideo")
                          << QString::fromLatin1("video/x-la-asf")
                          << QString::fromLatin1("video/x-m4v")
                          << QString::fromLatin1("video/x-mng")
                          << QString::fromLatin1("video/quicktime")
                          << QString::fromLatin1("video/x-sgi-movie")
                          << QString::fromLatin1("video/mpeg")
                          << QString::fromLatin1("video/mp4")
                          << QString::fromLatin1("video/x-pv-pvx")
                          << QString::fromLatin1("video/vnd.rn-realvideo")
                          << QString::fromLatin1("video/vdo")
                          << QString::fromLatin1("video/vivo")
                          << QString::fromLatin1("video/x-ms-wm")
                          << QString::fromLatin1("video/x-ms-wmx")
                          << QString::fromLatin1("video/wavelet")
            << QString::fromLatin1("video/x-ms-wvx"));
}

QStringList Common::supportedSuffixes()
{
    QStringList mSupportedSuffixes;
    QMimeDatabase mMimeDatabase;
    const QStringList mSupportedMimeTypes = Common::supportedMimeTypes();
    for (const QString &mFileType : mSupportedMimeTypes)
    {
        const QStringList mSuffixes = mMimeDatabase.mimeTypeForName(mFileType).suffixes();
        for (QString mSuffix : mSuffixes)
        {
            mSuffix.prepend(QString::fromLatin1("*."));
            mSupportedSuffixes << mSuffix;
        }
    }
    return mSupportedSuffixes;
}

bool Common::associateFileTypes()
{
    QString displayName = QGuiApplication::applicationDisplayName();
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    if (settings.status() != QSettings::NoError)
    {
        qWarning() << "Cannot access registry key" << key;
        return false;
    }
    settings.setValue(QString::fromLatin1("FriendlyAppName"), displayName);

    settings.beginGroup(QString::fromLatin1("SupportedTypes"));
    QMimeDatabase mimeDatabase;
    const QStringList mSupportedMimeTypes = Common::supportedMimeTypes();
    for (const QString &fileType : mSupportedMimeTypes)
    {
        const QStringList suffixes = mimeDatabase.mimeTypeForName(fileType).suffixes();
        for (QString suffix : suffixes)
        {
            suffix.prepend('.');
            settings.setValue(suffix, QString());
        }
    }
    settings.endGroup();

    settings.beginGroup(QString::fromLatin1("shell"));
    settings.beginGroup(QString::fromLatin1("open"));
    settings.setValue(QString::fromLatin1("FriendlyAppName"), displayName);
    settings.beginGroup(QString::fromLatin1("Command"));
    settings.setValue(QString::fromLatin1(".")
                      , QLatin1Char('"') + QDir::toNativeSeparators(filePath)
                      + QString::fromLatin1("\" \"%1\""));

    qDebug() << "File association registry key has been set.";

    return true;
}

void Common::unassociateFileTypes()
{
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();
    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    settings.clear();
    qDebug() << "File association registry key has been removed.";
}

bool Common::isFileTypesAssociated()
{
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();
    const QString key = QString::fromLatin1("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
    QSettings settings(key, QSettings::NativeFormat);
    if (settings.contains(QString::fromLatin1("FriendlyAppName")))
    {
        qDebug() << "Video file association check : associated.";
        return true;
    }
    qDebug() << "Video file association check : not associated.";
    return false;
}

bool Common::executeProgramWithAdministratorPrivilege(const QString &exePath
                                                          , const QString &exeParam)
{
    if (exePath.isEmpty())
    {
        qDebug() << "[UAC] : Empty file path.";
        return false;
    }

    SHELLEXECUTEINFO execInfo{sizeof(SHELLEXECUTEINFO)};
    execInfo.lpVerb = TEXT("runas");
    execInfo.lpFile = reinterpret_cast<const wchar_t *>(exePath.utf16());
    execInfo.nShow = SW_HIDE;
    execInfo.lpParameters = reinterpret_cast<const wchar_t *>(exeParam.utf16());

    if(!ShellExecuteEx(&execInfo))
    {
        DWORD dwStatus = GetLastError();
        if(dwStatus == ERROR_CANCELLED)
        {
            qDebug() << "[UAC] : User denied to give admin privilege.";
        }
        else if(dwStatus == ERROR_FILE_NOT_FOUND)
        {
            qDebug() << "[UAC] : File not found -- " << exePath.toUtf8().constData();
        }
        return false;
    }
    return true;
}

void Common::load_qm(const QString &lang)
{
    QString l(Config::instance().language());
    if (!lang.isEmpty())
    {
        l = lang;
    }
    if (l.toLower() == QLatin1String("system"))
    {
        l = QLocale::system().name();
    }
    QTranslator *QtTs = new QTranslator(qApp);
    if (QtTs->load(":/i18n/Qt_" + l))
    {
        qApp->installTranslator(QtTs);
    }
    else
    {
        delete QtTs;
    }
    QTranslator *QtAVTs = new QTranslator(qApp);
    if (QtAVTs->load(":/i18n/QtAV_" + l))
    {
        qApp->installTranslator(QtAVTs);
    }
    else
    {
        delete QtAVTs;
    }
    QTranslator *SPlayerTs = new QTranslator(qApp);
    if (SPlayerTs->load(":/i18n/SPlayer_" + l))
    {
        qApp->installTranslator(SPlayerTs);
    }
    else
    {
        delete SPlayerTs;
    }
}

void Common::set_opengl_backend()
{
    QString gl;
    switch (Config::instance().openGLType())
    {
    case Config::Desktop:
        gl = QLatin1String("desktop");
        break;
    case Config::OpenGLES:
        gl = QLatin1String("es");
        break;
    case Config::Software:
        gl = QLatin1String("software");
        break;
    default:
        gl = QLatin1String("software");
        break;
    }
    if (gl == QLatin1String("es") || gl == QLatin1String("angle")
            || gl == QLatin1String("opengles"))
    {
        gl = QLatin1String("es_");
        gl.append(Config::instance().getANGLEPlatform().toLower());
    }
    if (gl.startsWith(QLatin1String("es")))
    {
        qApp->setAttribute(Qt::AA_UseOpenGLES);
#ifdef QT_OPENGL_DYNAMIC
        qputenv("QT_OPENGL", "angle");
#endif
        if (gl.endsWith(QLatin1String("d3d11")))
        {
            qputenv("QT_ANGLE_PLATFORM", "d3d11");
        }
        else if (gl.endsWith(QLatin1String("d3d9")))
        {
            qputenv("QT_ANGLE_PLATFORM", "d3d9");
        }
        else if (gl.endsWith(QLatin1String("warp")))
        {
            qputenv("QT_ANGLE_PLATFORM", "warp");
        }
    }
    else if (gl == QLatin1String("desktop"))
    {
        qApp->setAttribute(Qt::AA_UseDesktopOpenGL);
    }
    else if (gl == QLatin1String("software"))
    {
        qApp->setAttribute(Qt::AA_UseSoftwareOpenGL);
    }
}

bool Common::createMutex(HANDLE *m_pMutexHandleOut, int *m_iMutexStateOut)
{
    *m_pMutexHandleOut = CreateMutex(NULL, TRUE
                 , reinterpret_cast<const wchar_t *>(QString::fromLatin1(AppID).utf16()));
    DWORD dwRet = GetLastError();
    if (*m_pMutexHandleOut)
    {
        if (ERROR_ALREADY_EXISTS == dwRet)
        {
            qDebug() << "[Mutex] : Application already running.";
            /*QString splayerCommand;
            if (app.arguments().count() > 1)
            {
                splayerCommand = app.arguments().at(1);
            }
            else
            {
                splayerCommand = QString::fromLatin1("ActiveMainWindow");
            }
            HWND hwnd = NULL;
            do
            {
               LPWSTR path = (LPWSTR)QString::fromLatin1(AppDisplayName).utf16();  //path = L"SendMessage"
               hwnd = ::FindWindowW(NULL, path);
            } while (hwnd == reinterpret_cast<HWND>(window.effectiveWinId())); // 忽略自己
            if (::IsWindow(hwnd))
            {
                QByteArray data = splayerCommand.toUtf8();

                COPYDATASTRUCT copydata;
                copydata.dwData = CUSTOM_SPLAYER_TYPE;  // 用户定义数据
                copydata.lpData = data.data();  //数据大小
                copydata.cbData = data.size();  // 指向数据的指针

                HWND sender = reinterpret_cast<HWND>(window.effectiveWinId());

                ::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(&copydata));
            }
            CloseHandle(*m_pMutexHandleOut);
            return 0;*/
            *m_iMutexStateOut = -1;
            return false;
        }
    }
    else
    {
        qDebug() << "[Mutex] : Failed to create mutex. Program exit.";
        CloseHandle(*m_pMutexHandleOut);
        //return -1;
        *m_iMutexStateOut = -2;
        return false;
    }
    qDebug() << "[Mutex] : Create mutex succeeded.";
    *m_iMutexStateOut = 0;
    return true;
}

bool Common::isDigitStr(const QString &str)
{
    const char *s = str.toUtf8().constData();

    while (*s && *s>='0' && *s<='9')
    {
        s++;
    }

    if (*s)
    {
        //不是纯数字
        return false;
    }
    else
    {
        //纯数字
        return true;
    }
}

void Common::changeSkinByName(const QString &skinName)
{
    if (skinName.isEmpty())
    {
        return;
    }
    QString skinFilePath = skinName;
    skinFilePath = skinFilePath.remove(QRegExp("\\s"));
    skinFilePath = QString::fromLatin1(":/qss/") + skinFilePath.toLower()
            + QString::fromLatin1(".css");
    Common::changeSkinByPath(skinFilePath);
}

void Common::changeSkinByPath(const QString &skinPath)
{
    if (skinPath.isEmpty())
    {
        return;
    }
    SStyle skinManager;
    skinManager.setStyle(skinPath);
    qDebug() << "[Skin manager] : setStyleSheet : " << skinPath.toUtf8().constData();
}

QString Common::getSkinType()
{
    QString skinName = Config::instance().getSkin();
    if (skinName.toLower().contains("dark"))
    {
        return QString::fromLatin1("Dark");
    }
    else
    {
        return QString::fromLatin1("Light");
    }
}

QString Common::getImagePath(const QString &originalImageFilePath)
{
    if (originalImageFilePath.isEmpty())
    {
        return QString();
    }
    QStringList list = originalImageFilePath.split('.');
    QString suffix = list.at(list.count() - 1);
    QString skinFileName = QString();
    for (int i = 0; i <= (list.count() - 2); ++i)
    {
        skinFileName += list.at(i);
    }
    skinFileName = skinFileName + Common::getSkinType().prepend('_') + suffix.prepend('.');
    return skinFileName;
}
