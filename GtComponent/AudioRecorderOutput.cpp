#include "AudioRecorderOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

AudioRecorderOutput::AudioRecorderOutput(QObject *parent) : QObject(parent)
{
    //采样精度和声道数暂时默认16\1
    outputFormat.setSampleRate(16000);
    outputFormat.setChannelCount(1);
    outputFormat.setSampleSize(16);
    outputFormat.setCodec("audio/pcm");
    outputFormat.setByteOrder(QAudioFormat::LittleEndian);
    outputFormat.setSampleType(QAudioFormat::SignedInt);
}

AudioRecorderOutput::~AudioRecorderOutput()
{
    freePlay();
}

bool AudioRecorderOutput::startPlay(AudioRecorderBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    stopPlay();

    outputDevice=device;
    outputFormat=format;
    if(!outputFormat.isValid()||outputDevice.isNull()){
        qDebug()<<"play failed,sample rate:"<<outputFormat.sampleRate()
               <<"device null:"<<outputDevice.isNull()<<outputDevice.supportedSampleRates();
        return false;
    }

    //参数不相等才重新new
    if(audioOutput&&(currentDevice!=outputDevice||currentFormat!=outputFormat)){
        freePlay();
    }
    if(!audioOutput){
        //保存当前deviceinfo，下次对比是否相同
        currentDevice=outputDevice;
        currentFormat=outputFormat;
        //QAudioFormat n_format=outputDevice.nearestFormat(outputFormat);
        audioOutput=new QAudioOutput(outputDevice,outputFormat,this);
        connect(audioOutput,&QAudioOutput::stateChanged,this,&AudioRecorderOutput::stateChanged);
        connect(audioOutput,&QAudioOutput::notify,this,&AudioRecorderOutput::notify);
        //目前用notify来控制进度刷新
        audioOutput->setNotifyInterval(50);
        //缓冲区
        //audioOutput->setBufferSize(12800);
    }
    buffer->reset();
    audioOutput->start(buffer);
    //qDebug()<<audioOutput->bufferSize()<<"buffer";
    return true;
}

void AudioRecorderOutput::stopPlay()
{
    if(audioOutput){
        audioOutput->stop();
    }
}

void AudioRecorderOutput::suspendPlay()
{
    if(audioOutput){
        audioOutput->suspend();
    }
}

void AudioRecorderOutput::resumePlay()
{
    if(audioOutput){
        audioOutput->resume();
    }
}

void AudioRecorderOutput::freePlay()
{
    if(audioOutput){
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput=nullptr;
    }
}

bool AudioRecorderOutput::saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath)
{
    //stopPlay();
    if(data.isEmpty())
        return false;
    //qfile不能生成目录
    QFileInfo info(filepath);
    if(!info.dir().exists())
        info.dir().mkpath(info.absolutePath());

    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    //暂时全部写入
    AudioRecorderWavHead head=AudioRecorderWavHead::createWavHead(
                format.sampleRate(),
                format.sampleSize(),
                format.channelCount(),
                data.size());
    file.write((const char *)(&head),sizeof(AudioRecorderWavHead));
    file.write(data);
    file.close();
    return true;
}
