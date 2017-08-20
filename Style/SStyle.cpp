#include "SStyle.h"
#include <QFile>
#include <QCoreApplication>
//#include <QEventLoop>
#include "WindowManager.h"

SStyle::SStyle()
{

}

SStyle::~SStyle()
{

}

void SStyle::setStyle(const QString &qssFile)
{
    static QString str;
    QString fileName = qssFile;
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly))
        {
            QString qss = QLatin1String(file.readAll());
            file.close();
            if (str == qss)
            {
                return;
            }
            str = qss;
            QString paletteColor = str.mid(20, 7);
            qApp->setPalette(QPalette(QColor(paletteColor)));
            qApp->setStyleSheet(str);
            //立即刷新窗口
            //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
    if (WindowManager::Instance()->GetMainWindow())
    {
        WindowManager::Instance()->GetMainWindow()->update();
    }
}
