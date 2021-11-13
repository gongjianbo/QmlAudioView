#include "ARDataOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

ARDataOutput::ARDataOutput(QObject *parent)
    : QObject(parent)
{

}

ARDataOutput::~ARDataOutput()
{
    freePlay();
}

bool ARDataOutput::startPlay(ARDataBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << "play" << device.deviceName() << format;
    stopPlay();

    outputDevice = device;
    outputFormat = format;
    if (!outputFormat.isValid() || outputDevice.isNull()) {
        qDebug() << "play failed,sample rate:" << outputFormat.sampleRate()
                 << "device null:" << outputDevice.isNull() << outputDevice.supportedSampleRates();
        return false;
    }

    //参数不相等才重新new
    if (audioOutput && (currentDevice != outputDevice || currentFormat != outputFormat)) {
        freePlay();
    }
    if (!audioOutput) {
        //保存当前deviceinfo，下次对比是否相同
        currentDevice = outputDevice;
        currentFormat = outputFormat;
        //QAudioFormat n_format=outputDevice.nearestFormat(outputFormat);
        audioOutput = new QAudioOutput(outputDevice, outputFormat, this);
        connect(audioOutput, &QAudioOutput::stateChanged, this, &ARDataOutput::stateChanged);
        connect(audioOutput, &QAudioOutput::notify, this, &ARDataOutput::notify);
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

void ARDataOutput::stopPlay()
{
    if (audioOutput) {
        audioOutput->stop();
    }
}

void ARDataOutput::suspendPlay()
{
    if (audioOutput) {
        audioOutput->suspend();
    }
}

void ARDataOutput::resumePlay()
{
    if (audioOutput) {
        audioOutput->resume();
    }
}

void ARDataOutput::freePlay()
{
    if (audioOutput) {
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}

bool ARDataOutput::saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath)
{
    //stopPlay();
    if (data.isEmpty())
        return false;
    //qfile不能生成目录
    QFileInfo info(filepath);
    if (!info.dir().exists())
        info.dir().mkpath(info.absolutePath());

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    //暂时全部写入
    ARWavHead head = ARWavHead::createWavHead(
                format.sampleRate(),
                format.sampleSize(),
                format.channelCount(),
                data.size());
    file.write((const char*)(&head), sizeof(ARWavHead));
    file.write(data);
    file.close();
    return true;
}
