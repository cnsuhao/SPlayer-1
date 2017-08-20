#pragma once

#include "PCH.h"
#include <QString>
#include <QStringList>

class Common
{
public:
    Common();

    static QStringList supportedMimeTypes();
    static QStringList supportedSuffixes();
    static bool associateFileTypes();
    static void unassociateFileTypes();
    static bool isFileTypesAssociated();
    static bool executeProgramWithAdministratorPrivilege(const QString &exePath
                                                         , const QString &exeParam);

    static void load_qm(const QString &lang = QLatin1String("system"));
    static void set_opengl_backend();

    static bool createMutex(HANDLE *m_pMutexHandleOut, int *m_iMutexStateOut);

    static bool isDigitStr(const QString &str);

    static void changeSkinByName(const QString &skinName = QString::fromLatin1("Dark Black"));
    static void changeSkinByPath(const QString &skinPath);

    static QString getSkinType();
    static QString getImagePath(const QString &originalImageFilePath);
};
