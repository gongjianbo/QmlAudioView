#include "AudioRecorderOperate.h"

#include <cmath>

#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QDebug>

AudioRecorderOperate::AudioRecorderOperate(QObject *parent)
    : QObject(parent)
{
    //init函数已关联thread的start
}

AudioRecorderOperate::~AudioRecorderOperate()
{
    audioBuffer->close();
}

void AudioRecorderOperate::setRecordState(AudioRecorder::RecordState state)
{
    //因为ui是预置的状态，所以operate无论修改没都需要ui同步状态
    //故不进行相等比较
    recordState=state;
    emit recordStateChanged(state);
}

qint64 AudioRecorderOperate::writeData(const char *data, qint64 maxSize)
{
    //默认为单声道，16bit
    QByteArray new_data=QByteArray(data,maxSize);
    audioData.append(new_data);
    emit dataChanged(new_data);
    return maxSize;
}

qint64 AudioRecorderOperate::readData(char *data, qint64 maxSize)
{
    if(!data||maxSize<1)
        return 0;
    const int data_size=audioData.count()-outputCount;
    if(data_size<=0){
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(0,[this]{ doStop(); });
        return 0;
    }

    const int read_size=(data_size>=maxSize)?maxSize:data_size;
    memcpy(data,audioData.constData()+outputCount,read_size);
    outputCount+=read_size;
    //refresh(); 这个间隔太大了
    return read_size;
}

void AudioRecorderOperate::init()
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer=new AudioRecorderBuffer(this,this);
    audioBuffer->open(QIODevice::ReadWrite);
}

void AudioRecorderOperate::doRecord(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    doStop();
    //录制时清空数据缓存
    audioData.clear();

    if(audioInput.startRecord(audioBuffer,device,format)){
        //切换为录制状态
        setRecordState(AudioRecorder::Record);
    }else{
        qDebug()<<"录制失败";
    }
}

void AudioRecorderOperate::doStop()
{
    //录制、播放时都会调用stop，所以把一些状态重置放这里
    //(停止的时候audioData的数据保留，在start时才清空)
    switch (getRecordState())
    {
    case AudioRecorder::Record:
        audioInput.stopRecord();
        break;
    case AudioRecorder::Stop: break;
    case AudioRecorder::Playing:
    case AudioRecorder::PlayPause:
        //audioOutput.stopPlay();
        break;
    default:
        break;
    }
    setRecordState(AudioRecorder::Stop);
}

void AudioRecorderOperate::doPlay(const QAudioDeviceInfo &device)
{
    //暂停继续
    if(getRecordState()==AudioRecorder::PlayPause){
        doResumePlay();
        return;
    }
    doStop();

    if(audioData.isEmpty())
        return;

    //QAudioFormat format=audioInput.inputFormat;
    //if(audioOutput.startPlay(audioIODevice,format,device)){
    //    //切换为播放状态
    //    setRecordState(Playing);
    //}else{
    //    qDebug()<<"播放失败";
    //}
}

void AudioRecorderOperate::doSuspendPlay()
{

}

void AudioRecorderOperate::doResumePlay()
{

}
