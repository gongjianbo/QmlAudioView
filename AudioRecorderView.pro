QT += core
QT += gui
QT += widgets
QT += quick
QT += multimedia
QT += concurrent

CONFIG += c++11 utf8_source
QMAKE_CFLAGS_WARN_ON = /W3
QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
QMAKE_CXXFLAGS += /sdl
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/GtComponent
include($$PWD/GtComponent/GtComponent.pri)

SOURCES += \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
