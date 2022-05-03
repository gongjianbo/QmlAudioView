#pragma once
#include <QQmlEngine>
#include <QQmlContext>
#include "Common/AVGlobal.h"
#include "Audio/AVDataBuffer.h"
#include "Audio/AVDataInput.h"
#include "Audio/AVDataOutput.h"
#include "Audio/AVDataSource.h"
#include "Audio/AVAudioContext.h"
#include "Plot/AVSimpleView.h"
#include "Plot/AVAbstractLayer.h"
#include "Plot/AVAbstractAxis.h"
#include "Plot/AVAbstractSeries.h"
#include "Plot/AVWrapper.h"
#include "Plot/AVXYView.h"
#include "Plot/AVXYGrid.h"
#include "Plot/AVValueAxis.h"
#include "Plot/AVAudioSeries.h"
#include "Plot/AVAudioThumbnail.h"

//qt5 register QML component
//注册为QML组件
namespace AudioView {

inline void registerComponent(QQmlEngine*)
{
    //common
    qRegisterMetaType<QAudioFormat>("QAudioFormat");
    qRegisterMetaType<AVGlobal::WorkState>("AVGlobal::WorkState");
    qRegisterMetaType<AVGlobal::ErrorType>("AVGlobal::ErrorType");
    qmlRegisterUncreatableType<AVGlobal>("AudioView", 1, 0, "AVGlobal",
                                         "can not instantiate AVGlobal in qml");
    //audio
    qmlRegisterType<AVDataInput>("AudioView", 1, 0, "AVDataInput");
    qmlRegisterType<AVDataOutput>("AudioView", 1, 0, "AVDataOutput");
    qmlRegisterType<AVDataSource>("AudioView", 1, 0, "AVDataSource");
    qmlRegisterType<AVAudioContext>("AudioView", 1, 0, "AVAudioContext");
    //plot
    qmlRegisterType<AVSimpleView>("AudioView", 1, 0, "AVSimpleView");
    qmlRegisterUncreatableType<AVXYLayout>("AudioView", 1, 0, "AVXYLayout",
                                           "can not instantiate AVXYLayout in qml");
    qmlRegisterType<AVXYView>("AudioView", 1, 0, "AVXYView");
    qmlRegisterType<AVXYGrid>("AudioView", 1, 0, "AVXYGrid");
    qmlRegisterType<AVWrapper>("AudioView", 1, 0, "AVWrapper");
    qmlRegisterType<AVValueAxis>("AudioView", 1, 0, "AVValueAxis");
    qmlRegisterType<AVAudioSeries>("AudioView", 1, 0, "AVAudioSeries");
    qmlRegisterType<AVAudioThumbnail>("AudioView", 1, 0, "AVAudioThumbnail");
}

}

