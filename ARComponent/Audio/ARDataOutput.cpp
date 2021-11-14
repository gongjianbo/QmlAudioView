#include "ARDataOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

ARDataOutput::ARDataOutput(ARDataSource *source, QObject *parent)
    : QObject(parent)
    , audioSource(source)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new ARDataBuffer(this, this);
    audioBuffer->open(QIODevice::ReadOnly);

    connect(this, &ARDataOutput::notify, this, [this]{
        if (!audioOutput)
            return;
        const QByteArray &audio_data = audioSource->getData();
        if (audio_data.isEmpty()) {
            setCurrentIndex(0);
            return;
        }

        //用processedUSecs获取start到当前的us数，但是start后有点延迟
        //进度=已放时间和总时间之比*总字节数，注意时间单位
        //-50是为了补偿时差，音画同步，这个50就是output的NotifyInterval
        //（还有点问题就是快结束的时候尾巴上那点直接结束了，数据少的时候明显点）
        qint64 cur_pos = outputOffset + (audioOutput->processedUSecs() / 1000.0) /
                audioSource->getDuration() * audio_data.count();
        if (cur_pos < 0) {
            cur_pos = 0;
        }
        else if (cur_pos > outputCount) {
            cur_pos = outputCount;
        }
        setCurrentIndex(cur_pos);
    });
}

ARDataOutput::~ARDataOutput()
{
    freePlay();
}

qint64 ARDataOutput::readData(char *data, qint64 maxSize)
{
    if (!data || maxSize < 1)
        return 0;
    const QByteArray &audio_data = audioSource->getData();
    const int data_size = audio_data.count() - outputCount;
    if (data_size <= 0) {
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(1, [this] { playFinished(); });
        return 0;
    }

    const int read_size = (data_size >= maxSize) ? maxSize : data_size;
    memcpy(data, audio_data.constData() + outputCount, read_size);
    outputCount += read_size;
    //refresh(); 这个间隔回调太大了，不适合用来刷新播放进度
    return read_size;
}

qint64 ARDataOutput::getPosition() const
{
    return currentPosition;
}

void ARDataOutput::setPosition(qint64 position)
{
    if (currentPosition != position) {
        currentPosition = position;
        emit positionChanged(position);
    }
}

qint64 ARDataOutput::getCurrentIndex() const
{
    return currentIndex;
}

void ARDataOutput::setCurrentIndex(qint64 index)
{
    if (index < 0) {
        index = 0;
    }
    else if (index >= audioSource->getData().size()) {
        index = audioSource->getData().size() - 1;
    }
    if (currentIndex != index) {
        currentIndex = index;
        emit currentIndexChanged(index);

        const QAudioFormat format = audioSource->getFormat();
        const int sample_rate = format.sampleRate();
        const int sample_byte = format.sampleSize() / 8;
        const int channel_count = format.channelCount();
        const qint64 position = ((index / sample_byte) / (1.0 * channel_count * sample_rate) * 1000);
        setPosition(position);
    }
}

void ARDataOutput::setCurrentOffset(qint64 offset)
{
    const QAudioFormat format = audioSource->getFormat();
    if (offset > 0 && format.sampleSize() > 0) {
        audioBuffer->reset();
        offset -= (offset % (format.sampleSize() / 8));
        if (offset < 0) {
            offset = 0;
        }
        else if (offset >= audioSource->getData().size()) {
            offset = audioSource->getData().size() - 1;
        }
        outputCount = offset;
        if (!audioOutput || audioOutput->state() == QAudio::StoppedState) {
            outputOffset = offset;
        }
        else {
            outputOffset = outputOffset + offset - currentIndex;
        }
        setCurrentIndex(offset);
    }
}

bool ARDataOutput::startPlay(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << "play" << device.deviceName() << format;
    if (audioOutput) {
        audioOutput->stop();
    }

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
    audioBuffer->reset();
    audioOutput->start(audioBuffer);
    //qDebug()<<audioOutput->bufferSize()<<"buffer";
    return true;
}

void ARDataOutput::stopPlay()
{
    outputCount = 0;
    outputOffset = 0;
    setCurrentIndex(0);
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
        audioBuffer->close();

        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}

bool ARDataOutput::saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath)
{
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
