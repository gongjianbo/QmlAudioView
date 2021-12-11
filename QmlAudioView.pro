QT += core
QT += gui
QT += widgets
QT += quick
QT += multimedia
QT += concurrent

CONFIG += c++11 
CONFIG += utf8_source

DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CFLAGS_WARN_ON = /W3
QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
QMAKE_CXXFLAGS += /sdl

INCLUDEPATH += $$PWD/AudioViewComponent
include($$PWD/AudioViewComponent/AudioViewComponent.pri)

SOURCES += main.cpp \
    SimpleAudioRecorder.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    SimpleAudioRecorder.h
