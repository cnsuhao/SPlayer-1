#-------------------------------------------------
#
# SPlayerCore.dll, SPlayer main logic code
#
#-------------------------------------------------

TARGET = SPlayerCore

TEMPLATE = lib

QT += \
    core \
    gui \
    svg \
    opengl \
    winextras \
    sql \
    widgets \
    avwidgets

CONFIG += \
    C++11

TRANSLATIONS += \
    $$PWD/Resources/i18n/SPlayer_zh_CN.ts

PRECOMPILED_HEADER = $$PWD/PCH.h

INCLUDEPATH += \
    $$PWD

DEPENDPATH += \
    $$PWD

RESOURCES += \
    $$PWD/Resources/images/images.qrc \
    $$PWD/Resources/qss/qss.qrc \
    $$PWD/Resources/i18n/i18n.qrc \
    $$PWD/Resources/icons/icons.qrc \
    $$PWD/Resources/licenses/licenses.qrc

DESTDIR = bin

QMAKE_INFO_PLIST = $$PWD/common/Info.plist

videos.files = video

videos.path = /

QMAKE_BUNDLE_DATA += \
    videos

RC_ICONS = $$PWD/Resources/icons/SPlayer.ico

RC_LANG = 0x0004

VERSION = 1.1.1.0

QMAKE_TARGET_COMPANY = "wangwenx190"

QMAKE_TARGET_DESCRIPTION = "SPlayer - Enjoy the simple life"

QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2017 by wangwenx190. All rights reserved."

QMAKE_TARGET_PRODUCT = "SPlayer"

LIBS += \
    -lUser32 \
    -lShell32

DEFINES += \
    SPLAYERCORE_LIBRARY \
    QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/SPlayerCore.h \
    $$PWD/SPlayerCore_Global.h \
    $$PWD/common/Config.h \
    $$PWD/common/ScreenSaver.h \
    $$PWD/MainWindow.h \
    $$PWD/ClickableMenu.h \
    $$PWD/StatisticsView.h \
    $$PWD/Slider.h \
    $$PWD/config/ConfigDialog.h \
    $$PWD/config/ConfigPageBase.h \
    $$PWD/config/CaptureConfigPage.h \
    $$PWD/config/VideoEQConfigPage.h \
    $$PWD/config/DecoderConfigPage.h \
    $$PWD/config/MiscPage.h \
    $$PWD/filters/AVFilterSubtitle.h \
    $$PWD/filters/OSD.h \
    $$PWD/filters/OSDFilter.h \
    $$PWD/playlist/PlayListModel.h \
    $$PWD/playlist/PlayListItem.h \
    $$PWD/playlist/PlayListDelegate.h \
    $$PWD/playlist/PlayList.h \
    $$PWD/config/PropertyEditor.h \
    $$PWD/config/AVFormatConfigPage.h \
    $$PWD/config/AVFilterConfigPage.h \
    $$PWD/config/ShaderPage.h \
    $$PWD/StandardDialog/StandardDialog.h \
    $$PWD/StandardDialog/StandardHeader.h \
    $$PWD/Control/BaseWidget.h \
    $$PWD/Control/ObjectFourTuple.h \
    $$PWD/Control/SButton.h \
    $$PWD/WindowManager.h \
    $$PWD/PCH.h \
    $$PWD/Style/SStyle.h \
    $$PWD/StandardDialog/StandardBottom.h \
    $$PWD/LogoWidget.h \
    $$PWD/WindowBottom.h \
    $$PWD/Control/SProgressBar.h \
    $$PWD/common/Common.h \
    $$PWD/AboutBox.h \
    $$PWD/HelpBox.h \
    $$PWD/DonateBox.h \
    $$PWD/Control/SMessageBox.h

SOURCES += \
    $$PWD/SPlayerCore.cpp \
    $$PWD/common/Config.cpp \
    $$PWD/common/ScreenSaver.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/ClickableMenu.cpp \
    $$PWD/StatisticsView.cpp \
    $$PWD/Slider.cpp \
    $$PWD/config/ConfigDialog.cpp \
    $$PWD/config/ConfigPageBase.cpp \
    $$PWD/config/CaptureConfigPage.cpp \
    $$PWD/config/VideoEQConfigPage.cpp \
    $$PWD/config/DecoderConfigPage.cpp \
    $$PWD/config/MiscPage.cpp \
    $$PWD/filters/AVFilterSubtitle.cpp \
    $$PWD/filters/OSD.cpp \
    $$PWD/filters/OSDFilter.cpp \
    $$PWD/playlist/PlayListModel.cpp \
    $$PWD/playlist/PlayListItem.cpp \
    $$PWD/playlist/PlayListDelegate.cpp \
    $$PWD/playlist/PlayList.cpp \
    $$PWD/config/PropertyEditor.cpp \
    $$PWD/config/AVFormatConfigPage.cpp \
    $$PWD/config/AVFilterConfigPage.cpp \
    $$PWD/config/ShaderPage.cpp \
    $$PWD/StandardDialog/StandardDialog.cpp \
    $$PWD/StandardDialog/StandardHeader.cpp \
    $$PWD/Control/BaseWidget.cpp \
    $$PWD/Control/SButton.cpp \
    $$PWD/WindowManager.cpp \
    $$PWD/Style/SStyle.cpp \
    $$PWD/StandardDialog/StandardBottom.cpp \
    $$PWD/LogoWidget.cpp \
    $$PWD/WindowBottom.cpp \
    $$PWD/Control/SProgressBar.cpp \
    $$PWD/common/Common.cpp \
    $$PWD/AboutBox.cpp \
    $$PWD/HelpBox.cpp \
    $$PWD/DonateBox.cpp \
    $$PWD/Control/SMessageBox.cpp
