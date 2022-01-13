#pragma once
#include <QQmlEngine>
#include <QQmlContext>
#include "Common/AVGlobal.h"
#include "Plot/AVSimpleView.h"
#include "Plot/AVAbstractLayer.h"
#include "Plot/AVAbstractAxis.h"
#include "Plot/AVAbstractSeries.h"
#include "Plot/AVWrapper.h"
#include "Plot/AVXYView.h"
#include "Plot/AVValueAxis.h"

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
    qmlRegisterUncreatableType<AVXYLayout>("AudioView", 1, 0, "AVXYLayout",
                                           "can not instantiate AVXYLayout in qml");
    qmlRegisterType<AVSimpleView>("AudioView", 1, 0, "AVSimpleView");
    qmlRegisterType<AVXYView>("AudioView", 1, 0, "AVXYView");
    qmlRegisterType<AVWrapper>("AudioView", 1, 0, "AVWrapper");
    qmlRegisterType<AVValueAxis>("AudioView", 1, 0, "AVValueAxis");
}

}

