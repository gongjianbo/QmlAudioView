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
    calcDuration();
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

void AudioRecorderOperate::calcDuration()
{
    //更新时长信息
    const int sample_rate=audioInput->inputFormat.sampleRate();
    qint64 duration=0;
    if(sample_rate>0){
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        duration=(audioData.size()/2)/(1.0*sample_rate)*1000;
    }

    if(audioDuration!=duration){
        audioDuration=duration;
        emit durationChanged(duration);
    }
}

void AudioRecorderOperate::calcPosition()
{
    //未播放时positon为0
    qint64 position=0;
    if(getRecordState()==AudioRecorder::Playing||getRecordState()==AudioRecorder::PlayPause){
        const int sample_rate=audioInput->inputFormat.sampleRate();
        position=((audioCursor/2)/(1.0*sample_rate)*1000);
    }
    if(audioPostion!=position){
        audioPostion=position;
        emit positionChanged(position);
    }
}

void AudioRecorderOperate::setAudioCursor(qint64 cursor)
{
    if(audioCursor!=cursor){
        audioCursor=cursor;
        emit cursorChanged(cursor);
    }
}

void AudioRecorderOperate::init()
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer=new AudioRecorderBuffer(this,this);
    audioBuffer->open(QIODevice::ReadWrite);

    //输出输出
    audioInput=new AudioRecorderInput(this);
    audioOutput=new AudioRecorderOutput(this);

    //输入输出状态
    connect(audioInput,&AudioRecorderInput::stateChanged,
            this,[this](QAudio::State state){
        qDebug()<<"input state changed"<<state;
    });
    connect(audioOutput,&AudioRecorderOutput::stateChanged,
            this,[this](QAudio::State state){
        qDebug()<<"output state changed"<<state;
    });
    //更新播放进度游标
    connect(audioOutput,&AudioRecorderOutput::notify,this,[this]{
        if(audioOutput&&audioOutput->audioOutput&&audioDuration){
            //用processedUSecs获取start到当前的us数，但是start后有点延迟
            //进度=已放时间和总时间之比*总字节数，注意时间单位
            //-50是为了补偿时差，音画同步，这个50就是output的NotifyInterval
            //（还有点问题就是快结束的时候尾巴上那点直接结束了，数据少的时候明显点）
            qint64 cursor = (audioOutput->audioOutput->processedUSecs()/1000.0-50)/audioDuration*audioData.count();
            if(cursor>outputCount)
                cursor=outputCount;
            setAudioCursor(cursor);
            calcPosition();
        }
    });
}

void AudioRecorderOperate::doRecord(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    doStop();
    //录制时清空数据缓存
    audioData.clear();

    if(audioInput->startRecord(audioBuffer,device,format)){
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
    outputCount=0;
    setAudioCursor(0);
    switch (getRecordState())
    {
    case AudioRecorder::Record:
        audioInput->stopRecord();
        break;
    case AudioRecorder::Stop: break;
    case AudioRecorder::Playing:
    case AudioRecorder::PlayPause:
        audioOutput->stopPlay();
        break;
    default:
        break;
    }
    setRecordState(AudioRecorder::Stop);
    calcPosition();
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

    const QAudioFormat format=audioInput->inputFormat;
    if(audioOutput->startPlay(audioBuffer,device,format)){
        //切换为播放状态
        setRecordState(AudioRecorder::Playing);
    }else{
        qDebug()<<"播放失败";
    }
}

void AudioRecorderOperate::doSuspendPlay()
{
    if(getRecordState()!=AudioRecorder::Playing)
        return;
    audioOutput->suspendPlay();
    setRecordState(AudioRecorder::PlayPause);
}

void AudioRecorderOperate::doResumePlay()
{
    if(getRecordState()!=AudioRecorder::PlayPause)
        return;
    audioOutput->resumePlay();
    setRecordState(AudioRecorder::Playing);
}
