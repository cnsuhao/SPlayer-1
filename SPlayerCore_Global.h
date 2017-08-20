#pragma once

#include <QtCore/qglobal.h>

#if defined(SPLAYERCORE_LIBRARY)
#  define SPLAYERCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SPLAYERCORESHARED_EXPORT Q_DECL_IMPORT
#endif
