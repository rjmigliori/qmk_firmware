#-------------------------------------------------
#
# Project created by QtCreator 2020-05-02T21:08:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = util
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    monitorwindow.cpp \
    communication.cpp \
    device.cpp \
    hidthread.cpp \
    kbd_defs.cpp \
    signal_level.cpp \
    columntester.cpp \
    rowdactester.cpp

HEADERS += \
        mainwindow.h \
    monitorwindow.h \
    communication.h \
    device.h \
    hidthread.h \
    kbd_defs.h \
    signal_level.h \
    columntester.h \
    rowdactester.h

unix:!macx {
    LIBS += -lhidapi-libusb
    #LIBS += -lhidapi-hidraw
    INCLUDEPATH += /usr/include/hidapi
}

macx {
    INCLUDEPATH += /usr/local/Cellar/hidapi/0.9.0/include/hidapi
    LIBS += -L/usr/local/Cellar/hidapi/0.9.0/lib -lhidapi
}

win32 {
    # Note: at the moment this configuration is for cross-compiling only. I have not tested native windows compilation.
    INCLUDEPATH += /mxe/usr/i686-w64-mingw32.static/include/hidapi
    LIBS += -lhidapi -lsetupapi
}

FORMS += \
        mainwindow.ui \
    monitorwindow.ui \
    signal_level.ui \
    columntester.ui \
    rowdactester.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
