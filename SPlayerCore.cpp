#include "PCH.h"
#include "SPlayerCore.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QTranslator>
#include <QtDebug>
#include <QFileInfo>
#include <QIcon>
#include <QFont>
#include <QMimeDatabase>
#include <QtAV/AVPlayer.h>
#include <QtAV/VideoOutput.h>
#include <QtAVWidgets>
#include "config/PropertyEditor.h"
#include "MainWindow.h"
#include "WindowManager.h"
#include "common/Config.h"
#include "common/Common.h"
#include "Control/SMessageBox.h"

using namespace QtAV;

static const struct
{
    const char* name;
    VideoRendererId id;
} vid_map[] = {
{ "opengl", VideoRendererId_OpenGLWidget },
{ "gl", VideoRendererId_GLWidget2 },
{ "d2d", VideoRendererId_Direct2D },
{ "gdi", VideoRendererId_GDI },
{ "xv", VideoRendererId_XV },
{ "x11", VideoRendererId_X11 },
{ "qt", VideoRendererId_Widget },
{ 0, 0 }
};

VideoRendererId rendererId_from_opt_name(const QString& name)
{
    for (int i = 0; vid_map[i].name; ++i)
    {
        if (name == QLatin1String(vid_map[i].name))
        {
            return vid_map[i].id;
        }
    }
#ifndef QT_NO_OPENGL
    return VideoRendererId_GLWidget2;
#endif
    return VideoRendererId_Widget;
}

SPlayerCore::SPlayerCore()
{

}

//重载QApplication，使用其消息分发处理;
class SApplication : public QApplication
{
public:
    SApplication(int &argc, char **argv) : QApplication(argc,argv)
    {
        setApplicationName(QString::fromLatin1(AppName));
        setApplicationDisplayName(QString::fromLatin1(AppDisplayName));
        setApplicationVersion(QString::fromLatin1(AppVersion));
        setOrganizationName(QString::fromLatin1(AppPublisher));
        setOrganizationDomain(QString::fromLatin1(AppPublisherURL));
        setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        setAttribute(Qt::AA_EnableHighDpiScaling);
        setFont(QFont("Microsoft YaHei"));
        setWindowIcon(QIcon(":/Icon/SPlayer.ico"));
    }

protected:
    //消息投递;
    virtual bool notify(QObject *object, QEvent *event)
    {
        return QApplication::notify(object,event);
    }

};

int SPlayerCore::exec(int argc, char *argv[])
{
    qDebug() << aboutQtAV_PlainText().toUtf8().constData();

    SApplication app(argc, argv);

    qDebug() << app.arguments();

    if (app.arguments().contains(QString::fromLatin1("--assoc"), Qt::CaseInsensitive))
    {
        if (!Common::associateFileTypes())
        {
            SMessageBox::error(nullptr, QObject::tr("Error")
                                  , QObject::tr("Cannot associate file types."));
            return -1;
        }
        return 0;
    }
    else if (app.arguments().contains(QString::fromLatin1("--unassoc")
                                      , Qt::CaseInsensitive))
    {
        Common::unassociateFileTypes();
        return 0;
    }

    Config::setName(QString::fromLatin1(AppName));

    QDir::setCurrent(qApp->applicationDirPath());

    Common::set_opengl_backend();

    Common::load_qm(Config::instance().language());

    Common::changeSkinByName(Config::instance().getSkin());

    if (Config::instance().getIsAlwaysCheckAssoc())
    {
        if (!Common::isFileTypesAssociated())
        {
            if (SMessageBox::question(nullptr, QObject::tr("Associate file types")
                       , QObject::tr("SPlayer is not associated with video files"
                                     ", do you want to associate them now?")) == 0)
            {
                QString filePath = app.applicationFilePath();
                QString regParam = QString::fromLatin1("--assoc");
                if (!Common::executeProgramWithAdministratorPrivilege(filePath
                                                                          , regParam))
                {
                    SMessageBox::error(nullptr, QObject::tr("Error")
                                          , QObject::tr("Cannot associate file types."));
                }
            }
        }
    }

    qputenv("QTAV_LOG", QString::fromLatin1("off").toLatin1());
    QtAV::setFFmpegLogLevel(QString::fromLatin1("off").toLatin1());

    MainWindow window;
    WindowManager::Instance()->SetMainWindow(&window);
    window.setWindowTitle(QString::fromLatin1(AppDisplayName));

    HANDLE m_hAppMutex;
    int m_iMutexCreateState;
    Common::createMutex(&m_hAppMutex, &m_iMutexCreateState);
    if (m_iMutexCreateState == -1)
    {
        //程序已经运行
        //return 0;
    }
    else if (m_iMutexCreateState == -2)
    {
        //创建Mutex失败
        return -1;
    }
    else
    {
        //Mutex成功创建
    }

    window.setProperty("rendererId"
                       , rendererId_from_opt_name(
                           Config::instance().getVideoRenderer().toLower()));
    window.show();

    if (app.arguments().count() > 1)
    {
        QFileInfo fi(app.arguments().at(1));
        if (fi.exists() && fi.isFile())
        {
            QMimeDatabase mdb;
            if (Common::supportedMimeTypes().contains(mdb.mimeTypeForFile(fi).name()))
            {
                window.play(app.arguments().at(1));
            }
        }
    }

    int ret = app.exec();

    if (m_iMutexCreateState != -2)
    {
        CloseHandle(m_hAppMutex);
    }

    return ret;
}
