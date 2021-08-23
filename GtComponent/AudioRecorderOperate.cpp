#include "AudioRecorderOperate.h"

#include <cmath>
#include <algorithm>

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
    //双声道时数据为一左一右连续
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
    const int sample_byte=audioInput->inputFormat.sampleSize()/8;
    const int channel_count=audioInput->inputFormat.channelCount();
    qint64 duration=0;
    if(sample_rate>0){
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        duration=(audioData.size()/sample_byte)/(1.0*channel_count*sample_rate)*1000;
    }
    if(audioDuration!=duration){
        audioDuration=duration;
        emit durationChanged(duration);
    }
}

void AudioRecorderOperate::calcPosition()
{
    //录制时是否需要设置到末尾？
    qint64 position=0;
    if(getRecordState()==AudioRecorder::Stopped||
            getRecordState()==AudioRecorder::Playing||
            getRecordState()==AudioRecorder::PlayPaused){
        const int sample_rate=audioInput->inputFormat.sampleRate();
        const int sample_byte=audioInput->inputFormat.sampleSize()/8;
        const int channel_count=audioInput->inputFormat.channelCount();
        position=((audioCursor/sample_byte)/(1.0*channel_count*sample_rate)*1000);
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
        if(audioCursor<0)
            audioCursor=0;
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
            qint64 cursor=cursorOffset+(audioOutput->audioOutput->processedUSecs()/1000.0-50)/audioDuration*audioData.count();
            //qDebug()<<"notify"<<cursor<<cursorOffset<<outputCount;
            if(cursor>outputCount)
                cursor=outputCount;
            setAudioCursor(cursor);
            calcPosition();
        }
    });
}

void AudioRecorderOperate::stop(bool update)
{
    //录制、播放时都会调用stop，所以把一些状态重置放这里
    //(停止的时候audioData的数据保留，在start时才清空)
    outputCount=0;
    cursorOffset=0;
    setAudioCursor(0);
    switch (getRecordState())
    {
    case AudioRecorder::Stopped: break;
    case AudioRecorder::Playing:
    case AudioRecorder::PlayPaused:
        audioOutput->stopPlay();
        break;
    case AudioRecorder::Recording:
    case AudioRecorder::RecordPaused:
        audioInput->stopRecord();
        break;
    default:
        break;
    }
    if(update){
        setRecordState(AudioRecorder::Stopped);
    }
    calcPosition();
}

void AudioRecorderOperate::doStop()
{
    stop(true);
}

void AudioRecorderOperate::doPlay(qint64 offset, const QAudioDeviceInfo &device)
{
    //暂停继续
    if(getRecordState()==AudioRecorder::PlayPaused){
        doResumePlay();
        return;
    }
    stop();

    if(audioData.isEmpty())
        return;

    const QAudioFormat format=audioInput->inputFormat;
    if(offset>0&&format.sampleSize()>0){
        offset-=(offset%(format.sampleSize()/8*format.channelCount()));
        if(offset<0)
            offset=0;
        outputCount=offset;
        cursorOffset=offset;
        setAudioCursor(offset);
        calcPosition();
    }
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
    setRecordState(AudioRecorder::PlayPaused);
}

void AudioRecorderOperate::doResumePlay()
{
    if(getRecordState()!=AudioRecorder::PlayPaused)
        return;
    audioOutput->resumePlay();
    setRecordState(AudioRecorder::Playing);
}

void AudioRecorderOperate::doRecord(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    //暂停继续
    if(getRecordState()==AudioRecorder::RecordPaused){
        doResumeRecord();
        return;
    }
    stop();
    //录制时清空数据缓存
    audioData.clear();

    if(audioInput->startRecord(audioBuffer,device,format)){
        //切换为录制状态
        setRecordState(AudioRecorder::Recording);
    }else{
        qDebug()<<"录制失败";
    }
}

void AudioRecorderOperate::doSuspendRecord()
{
    if(getRecordState()!=AudioRecorder::Recording)
        return;
    audioInput->suspendRecord();
    setRecordState(AudioRecorder::RecordPaused);
}

void AudioRecorderOperate::doResumeRecord()
{
    if(getRecordState()!=AudioRecorder::RecordPaused)
        return;
    audioInput->resumeRecord();
    setRecordState(AudioRecorder::Recording);
}

void AudioRecorderOperate::doLoadFile(const QString &filepath)
{
    stop();

    audioData.clear();
    audioDuration=0;
    audioPostion=0;
    const bool result=audioInput->loadFromFile(audioBuffer,filepath);
    emit loadFileFinished(filepath,audioInput->inputFormat,result);
}

void AudioRecorderOperate::doSaveFile(const QString &filepath)
{
    stop();

    const bool result=audioOutput->saveToFile(audioData,audioInput->inputFormat,filepath);
    emit saveFileFinished(filepath,audioInput->inputFormat,result);
}

void AudioRecorderOperate::doSaveSlice(const QString &filepath, const QList<AudioSlice> &sliceList)
{
    stop();

    auto align_offset=[this](qint64 offset)->qint64{
        //限定在有效数据范围内
        if(offset>audioData.size()-1)
            offset=audioData.size()-1;
        //如果是16bit-双声道，则需要步进为4字节
        //计算之后得到的是对齐大小起始点的index，作为截止点可能会少取一个采样
        offset-=(offset%(audioInput->inputFormat.sampleSize()/8*audioInput->inputFormat.channelCount()));
        if(offset<0)
            offset=0;
        return offset;
    };

    //多个选区offset可能没有完全贴合
    QByteArray slice_data;
    //对齐数据后对offset start排序，最后再拼接
    QList<AudioSlice> select_slice=sliceList;
    for(AudioSlice &slice:select_slice)
    {
        if(slice.startOffset>slice.endOffset)
            std::swap(slice.startOffset,slice.endOffset);
        slice.startOffset=align_offset(slice.startOffset);
        slice.endOffset=align_offset(slice.endOffset);
    }
    //start从小到大排序
    std::sort(select_slice.begin(),select_slice.end(),
              [](const AudioSlice &left, const AudioSlice &right)->bool{
        return left.startOffset<right.startOffset;
    });
    //拼接选区
    for(const AudioSlice &slice:select_slice)
    {
        slice_data.push_back(audioData.mid(slice.startOffset,slice.endOffset-slice.startOffset));
    }

    const bool result=audioOutput->saveToFile(slice_data,audioInput->inputFormat,filepath);
    emit saveSliceFinished(filepath,audioInput->inputFormat,result);
}

void AudioRecorderOperate::doUpdateCursorOffset(qint64 offset)
{
    const QAudioFormat format=audioInput->inputFormat;
    if(offset>0&&format.sampleSize()>0){
        audioBuffer->reset();
        offset-=(offset%(format.sampleSize()/8));
        if(offset<0)
            offset=0;
        outputCount=offset;
        if(getRecordState()==AudioRecorder::Stopped){
            cursorOffset=offset;
        }else{
            cursorOffset=cursorOffset+offset-audioCursor;
        }
        setAudioCursor(offset);
        calcPosition();
    }
}
