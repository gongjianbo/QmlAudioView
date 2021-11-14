#include "ARDataInput.h"

#include <QFile>
#include <QDebug>

ARDataInput::ARDataInput(ARDataSource *source, QObject *parent)
    : QObject(parent)
    , audioSource(source)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new ARDataBuffer(this, this);
    audioBuffer->open(QIODevice::WriteOnly);

    connect(audioSource, &ARDataSource::durationChanged, this, &ARDataInput::setDuration);
}

ARDataInput::~ARDataInput()
{
    freeRecord();
}

qint64 ARDataInput::writeData(const char *data, qint64 maxSize)
{
    //双声道时数据为一左一右连续
    QByteArray new_data = QByteArray(data, maxSize);
    audioSource->appendData(new_data);
    return maxSize;
}

qint64 ARDataInput::getDuration() const
{
    return inputDuration;
}

void ARDataInput::setDuration(qint64 duration)
{
    if (inputDuration != duration) {
        inputDuration = duration;
        emit durationChanged(duration);
    }
}

bool ARDataInput::startRecord(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << "record" << device.deviceName() << format;
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
    audioBuffer->reset();
    audioInput->start(audioBuffer);
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
        audioBuffer->close();

        audioInput->deleteLater();
        audioInput = nullptr;
    }
}

bool ARDataInput::loadFromFile(QByteArray &data, QAudioFormat &format, const QString &filepath)
{
    stopRecord();

    QFile file(filepath);
    if (file.exists() && file.size() > 44 && file.open(QIODevice::ReadOnly))
    {
        ARWavHead head;
        file.read((char*)&head, 44);
        QByteArray pcm_data;
        if (ARWavHead::isValidWavHead(head)) {
            //暂时为全部读取
            pcm_data = file.readAll();
        }
        file.close();

        //采样率等置为相同参数
        if (pcm_data.count() > 0 && pcm_data.count() % head.numChannels == 0) {
            format.setSampleRate(head.sampleRate);
            format.setChannelCount(head.numChannels);
            format.setSampleSize(head.bitsPerSample);
            data = pcm_data;
            return true;
        }
    }
    return false;
}
