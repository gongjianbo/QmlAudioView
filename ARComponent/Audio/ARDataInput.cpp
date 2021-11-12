#include "ARDataInput.h"

#include <QFile>
#include <QDebug>

ARDataInput::ARDataInput(QObject *parent)
    : QObject(parent)
{
    //采样精度和声道数暂时默认16\1
    //默认参数可以放到全局配置
    inputFormat.setSampleRate(16000);
    inputFormat.setSampleSize(16);
    inputFormat.setChannelCount(1);
    inputFormat.setCodec("audio/pcm");
    inputFormat.setByteOrder(QAudioFormat::LittleEndian);
    inputFormat.setSampleType(QAudioFormat::SignedInt);
}

ARDataInput::~ARDataInput()
{
    freeRecord();
}

bool ARDataInput::startRecord(ARDataBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    stopRecord();

    inputDevice = device;
    inputFormat = format;
    if (!inputFormat.isValid() || inputDevice.isNull()) {
        qDebug() << "record failed,sample rate:" << inputFormat.sampleRate()
                 << "device null:" << inputDevice.isNull() << inputDevice.supportedSampleRates();
        return false;
    }

    //参数不相等才重新new
    if (audioInput && (currentDevice != inputDevice || currentFormat != inputFormat)) {
        freeRecord();
    }
    if (!audioInput) {
        //保存当前deviceinfo，下次对比是否相同
        currentDevice = inputDevice;
        currentFormat = inputFormat;
        audioInput = new QAudioInput(currentDevice, currentFormat, this);
        connect(audioInput, &QAudioInput::stateChanged, this, &ARDataInput::stateChanged);
        connect(audioInput, &QAudioInput::notify, this, &ARDataInput::notify);
        audioInput->setBufferSize(inputFormat.sampleRate() * inputFormat.channelCount());
    }
    buffer->reset();
    audioInput->start(buffer);
    return true;
}

void ARDataInput::stopRecord()
{
    if (audioInput) {
        audioInput->stop();
    }
}

void ARDataInput::suspendRecord()
{
    if (audioInput) {
        audioInput->suspend();
    }
}

void ARDataInput::resumeRecord()
{
    if (audioInput) {
        audioInput->resume();
    }
}

void ARDataInput::freeRecord()
{
    if (audioInput) {
        audioInput->stop();
        audioInput->deleteLater();
        audioInput = nullptr;
    }
}

bool ARDataInput::loadFromFile(ARDataBuffer *buffer, const QString &filepath)
{
    stopRecord();

    QFile file(filepath);
    if (file.exists() && file.size() > 44 &&
            file.open(QIODevice::ReadOnly)) {
        ARWavHead head;
        file.read((char*)&head, 44);
        QByteArray pcm_data;
        if (ARWavHead::isValidWavHead(head)) {
            //暂时为全部读取
            pcm_data = file.readAll();
            file.close();
        }
        //采样率等置为相同参数
        if (pcm_data.count() > 0 && pcm_data.count() % 2 == 0) {
            inputFormat.setSampleRate(head.sampleRate);
            inputFormat.setChannelCount(head.numChannels);
            inputFormat.setSampleSize(head.bitsPerSample);
            buffer->writeData(pcm_data.constData(), pcm_data.count());
            return true;
        }
    }
    return false;
}
