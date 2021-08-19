#pragma once
#include <QQmlEngine>
#include <QQmlContext>

#include "AudioRecorderView.h"

class ComponentRegister
{
public:
    static void registerType(QQmlContext *)
    {
        //录音组件
        qmlRegisterType<AudioRecorder>("Gt.Component",1,0,"AudioRecorder");
        qmlRegisterType<AudioRecorderView>("Gt.Component",1,0,"AudioRecorderView");
        qmlRegisterUncreatableType<AudioRecorderDevice>("Gt.Component",1,0,"AudioRecorderDevice",
                                                       "can not instantiate AudioRecorderDevice in qml");
        //qmlRegisterUncreatableType<AudioRecorderInput>("Gt.Component",1,0,"AudioRecorderInput",
        //"can not instantiate AudioRecorderInput in qml");
        //qmlRegisterUncreatableType<AudioRecorderOutput>("Gt.Component",1,0,"AudioRecorderOutput",
        //"can not instantiate AudioRecorderOutput in qml");
    }
};

