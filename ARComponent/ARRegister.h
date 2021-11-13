#pragma once
#include <QQmlEngine>
#include <QQmlContext>
#include "ARView.h"

//qt5 register QML component
//注册为QML组件
struct ARRegister
{
    static void registerComponent(QQmlEngine*)
    {
        qRegisterMetaType<QAudioDeviceInfo>("QAudioDeviceInfo");
        qRegisterMetaType<QList<QAudioDeviceInfo>>("QList<QAudioDeviceInfo>");
        qRegisterMetaType<ARSpace::WorkState>("ARSpace::WorkState");

        qmlRegisterType<ARView>("AR.Component", 3, 0, "RecorderView");
        qmlRegisterUncreatableType<ARSpace>("AR.Component", 3, 0, "ARSpace",
                                            "can not instantiate RecorderSpace in qml");
        qmlRegisterUncreatableType<ARDevice>("AR.Component", 3, 0, "RecorderDevice",
                                             "can not instantiate RecorderDevice in qml");
    }
};

