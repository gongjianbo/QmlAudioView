#ifndef COMPONENTREGISTER_H
#define COMPONENTREGISTER_H

#include <QQmlEngine>
#include <QQmlContext>

#include "AudioRecorderView.h"

class ComponentRegister
{
public:
    static void registerType(QQmlContext *)
    {
        //录音组件
        qmlRegisterType<AudioRecorderView>("GongJianBo",1,0,"AudioRecorderView");
        qmlRegisterUncreatableType<AudioRecorderInput>("GongJianBo",1,0,"AudioRecorderInput","can not instantiate AudioRecorderInput in qml");
        qmlRegisterUncreatableType<AudioRecorderOutput>("GongJianBo",1,0,"AudioRecorderOutput","can not instantiate AudioRecorderOutput in qml");
    }
};

#endif // COMPONENTREGISTER_H
