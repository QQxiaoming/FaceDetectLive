QT += core gui widgets multimedia
CONFIG += c++17

include(./lib/QFacefusion/qfacefusion.pri)
include(./lib/QFontIcon/QFontIcon.pri)
include(./lib/QGoodWindow/QGoodWindow/QGoodWindow.pri)
include(./lib/QGoodWindow/QGoodCentralWidget/QGoodCentralWidget.pri)

INCLUDEPATH += \
    util

SOURCES += \
    util/aspectratiopixmaplabel.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    util/aspectratiopixmaplabel.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    res/resource.qrc

win32:!wasm {
    DEFINES += DESKTOP_INTERACTION_MODE
    win32-g++ {
        QMAKE_CXXFLAGS += -Wno-deprecated-copy
    }
    win32-msvc*{
    }

    VERSION = 1.0.0.000
    RC_ICONS = "icons\ico.ico"
    QMAKE_TARGET_PRODUCT = "FaceDetect"
    QMAKE_TARGET_DESCRIPTION = "FaceDetect based on Qt $$[QT_VERSION]"
    QMAKE_TARGET_COPYRIGHT = "GNU General Public License v3.0"
}

unix:!macx:!android:!ios:!wasm {
    DEFINES += DESKTOP_INTERACTION_MODE
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
    QMAKE_RPATHDIR=$ORIGIN
    QMAKE_LFLAGS += -no-pie 
    LIBS += -lutil
}

macx:!ios:!wasm {
    DEFINES += DESKTOP_INTERACTION_MODE
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
    QMAKE_RPATHDIR=$ORIGIN
    ICON = "icons\ico.icns"
    LIBS += -lutil
    QMAKE_INFO_PLIST = Info.plist
}
