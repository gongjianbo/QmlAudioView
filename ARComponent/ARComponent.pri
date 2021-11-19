INCLUDEPATH += $$PWD/Audio
include($$PWD/Audio/Audio.pri)

INCLUDEPATH += $$PWD/Plot
include($$PWD/Plot/Plot.pri)

HEADERS += \
    $$PWD/ARDefine.h \
    $$PWD/ARRegister.h \
    $$PWD/ARView.h

SOURCES += \
    $$PWD/ARDefine.cpp \
    $$PWD/ARView.cpp
