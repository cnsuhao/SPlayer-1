#-------------------------------------------------
#
# SPlayer.exe, the SPlayer main executable file
#
#-------------------------------------------------

contains(QT_ARCH, x86_64) {
    TARGET = SPlayer64
} else {
    TARGET = SPlayer
}

TEMPLATE = app

DESTDIR = bin

LIBS += \
    -L$$DESTDIR\
    -lSPlayerCore1

SOURCES = $$PWD/main.cpp

RC_ICONS = $$PWD/Resources/icons/SPlayer.ico

RC_LANG = 0x0004

VERSION = 1.0.0.0

QMAKE_TARGET_COMPANY = "wangwenx190"

QMAKE_TARGET_DESCRIPTION = "SPlayer - Enjoy the simple life"

QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2017 by wangwenx190. All rights reserved."

QMAKE_TARGET_PRODUCT = "SPlayer"
