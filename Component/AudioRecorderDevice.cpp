#include "AudioRecorderDevice.h"

#include <Windows.h>
//#include <Dbt.h>

#include <QTime>
#include <QTimer>
#include <QMetaType>
#include <QCoreApplication>
#include <QtConcurrentRun>
#include <QDebug>

AudioRecorderDevice::AudioRecorderDevice(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<QAudioDeviceInfo>>("QList<QAudioDeviceInfo>");

    //connect(&futureWatcher,&QFutureWatcher::finished,this,[this](){});
    //更新了设备列表
    connect(this,&AudioRecorderDevice::inputDeviceInfosUpdated,
            this,&AudioRecorderDevice::setInputDeviceInfos);
    connect(this,&AudioRecorderDevice::outputDeviceInfosUpdated,
            this,&AudioRecorderDevice::setOutputDeviceInfos);
    //延时刷新
    updateTimer.setSingleShot(true);
    connect(&updateTimer,&QTimer::timeout,this,&AudioRecorderDevice::updateDeviceInfos);

    //注册到qApp过滤native事件
    qApp->installNativeEventFilter(this);
    //初始化设备信息
    resetCurrentInput();
    resetCurrentOutput();
    updateDeviceInfos();
}

AudioRecorderDevice::~AudioRecorderDevice()
{
    //貌似析构的时候自动会remove
    qApp->removeNativeEventFilter(this);
    if(!futureWatcher.isFinished())
        futureWatcher.waitForFinished();
}

void AudioRecorderDevice::setInputDeviceNames(const QStringList &names)
{
    if(inputDeviceNames!=names){
        inputDeviceNames=names;
        emit inputDeviceNamesChanged();
    }
}

void AudioRecorderDevice::setOutputDeviceNames(const QStringList &names)
{
    if(outputDeviceNames!=names){
        outputDeviceNames=names;
        emit outputDeviceNamesChanged();
    }
}

bool AudioRecorderDevice::setCurrentInputName(const QString &name)
{
    for(auto &info:enableInputDeviceInfos){
        if(info.deviceName()==name){
            currentInputDeviceInfo=info;
            return true;
        }
    }
    return false;
}

bool AudioRecorderDevice::setCurrentInputIndex(int index)
{
    if(index>=0&&index<enableInputDeviceInfos.count()){
        currentInputDeviceInfo=enableInputDeviceInfos.at(index);
        return true;
    }
    return false;
}

void AudioRecorderDevice::resetCurrentInput()
{
    currentInputDeviceInfo=QAudioDeviceInfo::defaultInputDevice();
}

bool AudioRecorderDevice::setCurrentOutputName(const QString &name)
{
    for(auto &info:enableOutputDeviceInfos){
        if(info.deviceName()==name){
            currentOutputDeviceInfo=info;
            return true;
        }
    }
    return false;
}

bool AudioRecorderDevice::setCurrentOutputIndex(int index)
{
    if(index>=0&&index<enableOutputDeviceInfos.count()){
        currentOutputDeviceInfo=enableOutputDeviceInfos.at(index);
        return true;
    }
    return false;
}

void AudioRecorderDevice::resetCurrentOutput()
{
    currentOutputDeviceInfo=QAudioDeviceInfo::defaultOutputDevice();
}

void AudioRecorderDevice::setInputDeviceInfos(const QList<QAudioDeviceInfo> &infos)
{
    enableInputDeviceInfos.clear();
    QStringList name_list;
    for(auto &info:infos)
    {
        //暂时固定过滤16k的
        if(info.supportedSampleRates().contains(16000)){
            enableInputDeviceInfos.push_back(info);
            name_list.push_back(info.deviceName());
        }
    }
    setInputDeviceNames(name_list);
}

void AudioRecorderDevice::setOutputDeviceInfos(const QList<QAudioDeviceInfo> &infos)
{
    enableOutputDeviceInfos.clear();
    QStringList name_list;
    for(auto &info:infos)
    {
        //暂时固定过滤16k的
        if(info.supportedSampleRates().contains(16000)){
            enableOutputDeviceInfos.push_back(info);
            name_list.push_back(info.deviceName());
        }
    }
    setOutputDeviceNames(name_list);
}

bool AudioRecorderDevice::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        //设备插拔，插入会触发两次，拔出会触发一次
        if(msg&&msg->message == WM_DEVICECHANGE)
        {
            qDebug()<<"device changed ...";
            updateTimer.start(500); //延迟500ms刷新设备列表
            //for(auto &info:QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
            //    qDebug()<<info.deviceName()<<info.supportedSampleRates();
            //同步阻塞会有警告，可能是此时系统设备信息还未更新完成
            //警告：Could not invoke audio interface activation.
            //(因为应用程序正在发送一个输入同步呼叫，所以无法执行传出的呼叫。)
            //一些详细信息需要注册，这里改为change就去查询输入列表是否变更
            //qDebug()<<"change"<<msg->wParam;
            //switch(msg->wParam)
            //{
            //default: break;
            //    //设备插入
            //case DBT_DEVICEARRIVAL:
            //    qDebug()<<"in";
            //    break;
            //    //设备移除
            //case DBT_DEVICEREMOVECOMPLETE:
            //    qDebug()<<"out";
            //    break;
            //}
        }
    }
    return false;
}

void AudioRecorderDevice::updateDeviceInfos()
{
    if(!futureWatcher.isFinished()){
        qDebug()<<"update device info failed, futureWatcher is running";
        return;
    }

    QFuture<void> future = QtConcurrent::run([this]{
        emit inputDeviceInfosUpdated(QAudioDeviceInfo::availableDevices(QAudio::AudioInput));
        emit outputDeviceInfosUpdated(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
    });
    futureWatcher.setFuture(future);
}
