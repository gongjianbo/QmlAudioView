#pragma once
#include <QQmlEngine>
#include <QQmlContext>
#include "Common/AVGlobal.h"
#include "Plot/AVSimpleView.h"

//qt5 register QML component
//注册为QML组件
namespace AudioView {

inline void registerComponent(QQmlEngine*)
{
    qRegisterMetaType<QAudioFormat>("QAudioFormat");
    qRegisterMetaType<AVGlobal::WorkState>("AVGlobal::WorkState");
    qRegisterMetaType<AVGlobal::ErrorType>("AVGlobal::ErrorType");

    qmlRegisterUncreatableType<AVGlobal>("AudioView", 1, 0, "AVGlobal",
                                         "can not instantiate AVGlobal in qml");
    qmlRegisterType<AVSimpleView>("AudioView", 1, 0, "AVSimpleView");
}

}

