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
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer=new AudioRecorderBuffer(this,this);
    audioBuffer->open(QIODevice::ReadWrite);
}

void AudioRecorderOperate::setHasRecordData(bool has)
{
    if(hasRecordData!=has){
        hasRecordData=has;
        emit hasRecordDataChanged(has);
    }
}

qint64 AudioRecorderOperate::writeData(const char *data, qint64 maxSize)
{
    return -1;
}

qint64 AudioRecorderOperate::readData(char *data, qint64 maxSize)
{
    return -1;
}
