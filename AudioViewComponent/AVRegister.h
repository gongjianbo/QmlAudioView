#pragma once
#include <QQmlEngine>
#include <QQmlContext>
#include "Common/AVGlobal.h"

//qt5 register QML component
//注册为QML组件
namespace AudioView {

inline void registerComponent(QQmlEngine*)
{
    qRegisterMetaType<AVGlobal::WorkState>("AVGlobal::WorkState");

    qmlRegisterUncreatableType<AVGlobal>("AudioView", 1, 0, "AVGlobal",
                                         "can not instantiate AVGlobal in qml");
}

}

