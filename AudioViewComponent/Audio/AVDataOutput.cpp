#include "AVDataOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

AVDataOutput::AVDataOutput(AVDataSource *source, QObject *parent)
    : QObject(parent)
    , audioSource(source)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new AVDataBuffer(this, this);
    audioBuffer->open(QIODevice::ReadOnly);

    connect(this, &AVDataOutput::notify, this, [this]{
        if (!audioOutput || !audioSource)
            return;
        if (audioSource->isEmpty()) {
            zeroCurrentIndex();
            return;
        }

        //用processedUSecs获取start到当前的us数，但是start后有点延迟
        //进度=已放时间和总时间之比*总字节数，注意时间单位
        //-50是为了补偿时差，音画同步，这个50就是output的NotifyInterval
        //（还有点问题就是快结束的时候尾巴上那点直接结束了，数据少的时候明显点）
        qint64 cur_pos = outputOffset + (audioOutput->processedUSecs() / 1000.0) /
                audioSource->getDuration() * audioSource->size();
        if (cur_pos < 0) {
            cur_pos = 0;
        }
        else if (cur_pos > outputCount) {
            cur_pos = outputCount;
        }
        //减一个采样做下标
        setCurrentIndex(cur_pos - audioSource->getSampleSize() / 8);
    });
}

AVDataOutput::~AVDataOutput()
{
    freePlay();
}

qint64 AVDataOutput::readData(char *data, qint64 maxSize)
{
    if (!data || maxSize < 1)
        return 0;
    const std::vector<char> &audio_data = audioSource->getData();
    const qint64 data_size = audioSource->size() - outputCount;
    if (data_size <= 0) {
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(1, [this] { playFinished(); });
        return 0;
    }

    const qint64 read_size = (data_size >= maxSize) ? maxSize : data_size;
    memcpy(data, audio_data.data() + outputCount, read_size);
    outputCount += read_size;
    //refresh(); 这个间隔回调太大了，不适合用来刷新播放进度
    return read_size;
}

qint64 AVDataOutput::getPosition() const
{
    return playPosition;
}

void AVDataOutput::setPosition(qint64 position)
{
    if (playPosition != position) {
        playPosition = position;
        emit positionChanged(position);
    }
}

qint64 AVDataOutput::getCurrentIndex() const
{
    return playCurrentIndex;
}

void AVDataOutput::setCurrentIndex(qint64 index)
{
    const QAudioFormat format = audioSource->getFormat();
    const qint64 sample_byte = format.sampleSize() / 8;
    //data size为0时index=负数，下一步会重置为index=0
    if (index >= audioSource->size() - sample_byte) {
        index = audioSource->size() - sample_byte;
    }
    if (index < 0) {
        index = 0;
    }
    if (playCurrentIndex != index) {
        playCurrentIndex = index;
        emit currentIndexChanged(index);

        //const QAudioFormat format = audioSource->getFormat();
        const int sample_rate = format.sampleRate();
        //const int sample_byte = format.sampleSize() / 8;
        const int channel_count = format.channelCount();
        //在播放的时候，数组下标比播放位置小一个采样，此处补齐
        index += sample_byte;
        const qint64 position = ((index / sample_byte) / (1.0 * channel_count * sample_rate) * 1000);
        setPosition(position);
    }
}

void AVDataOutput::zeroCurrentIndex()
{
    playCurrentIndex = 0;
    emit currentIndexChanged(0);
    setPosition(0);
}

qint64 AVDataOutput::getStartIndex() const
{
    return playStartIndex;
}

void AVDataOutput::setStartIndex(qint64 index)
{
    if (playStartIndex != index) {
        playStartIndex = index;
        emit startIndexChanged(index);
    }
}

/*void AVDataOutput::setCurrentOffset(qint64 offset)
{
   const QAudioFormat format = audioSource->getFormat();
    if (offset > 0 && format.sampleSize() > 0) {
        audioBuffer->reset();
        const qint64 sample_byte = format.sampleSize() / 8;
        offset -= (offset % sample_byte);
        //data size为0时index=-负数，下一步会重置为index=0
        if (offset >= audioSource->size() - sample_byte) {
            offset = audioSource->size() - sample_byte;
        }
        if (offset < 0) {
            offset = 0;
        }
        outputCount = offset;
        if (!audioOutput || audioOutput->state() == QAudio::StoppedState) {
            outputOffset = offset;
        }
        else {
            outputOffset = outputOffset + offset - playCurrentIndex;
        }
        setCurrentIndex(offset);
    }
}*/

QAudio::State AVDataOutput::getState() const
{
    if (audioOutput) {
        return audioOutput->state();
    }
    return QAudio::StoppedState;
}

bool AVDataOutput::startPlay(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << "play" << device.deviceName() << format;
    endPlay();

    outputDevice = device;
    outputFormat = format;
    //无效的参数
    if (!outputFormat.isValid() || outputDevice.isNull() || audioSource->isEmpty()) {
        qDebug() << "play failed,sample rate:" << outputFormat.sampleRate()
                 << "device null:" << outputDevice.isNull() << outputDevice.supportedSampleRates();
        if (!outputFormat.isValid()) {
            emit errorChanged(AVGlobal::OutputFormatError);
        }
        else if (outputDevice.isNull()) {
            emit errorChanged(AVGlobal::OutputDeviceError);
        }
        else if (audioSource->isEmpty()) {
            emit errorChanged(AVGlobal::OutputEmptyError);
        }
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
        connect(audioOutput, &QAudioOutput::stateChanged, this, &AVDataOutput::stateChanged);
        connect(audioOutput, &QAudioOutput::notify, this, &AVDataOutput::notify);
        //目前用notify来控制进度刷新
        audioOutput->setNotifyInterval(50);
        //缓冲区
        //audioOutput->setBufferSize(12800);
    }
    audioBuffer->reset();
    audioOutput->start(audioBuffer);
    //qDebug()<<audioOutput->bufferSize()<<"buffer";
    if (audioOutput->error() != QAudio::NoError) {
        emit errorChanged(AVGlobal::OutputStartError);
        return false;
    }
    return true;
}

void AVDataOutput::stopPlay()
{
    if (audioOutput) {
        audioOutput->stop();
    }
    outputCount = 0;
    outputOffset = 0;
    setStartIndex(0);
    zeroCurrentIndex();
}

void AVDataOutput::endPlay()
{
    if (audioOutput) {
        audioOutput->stop();
    }
    //如果当前位置已在结尾，则回到播放起始位置重新播放
    //todo判断选区
    if (outputCount >= audioSource->size()) {
        outputCount = 0;
        //outputOffset = 0;
        setStartIndex(0);
        zeroCurrentIndex();
    }
    outputOffset = outputCount;
}

void AVDataOutput::suspendPlay()
{
    if (audioOutput) {
        audioOutput->suspend();
    }
}

void AVDataOutput::resumePlay()
{
    if (audioOutput) {
        audioOutput->resume();
    }
}

void AVDataOutput::freePlay()
{
    if (audioOutput) {
        audioOutput->stop();
        audioBuffer->close();

        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}

/*bool AVDataOutput::saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath)
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
    AVWavHead head(format.sampleRate(), format.sampleSize(),
                   format.channelCount(), data.size());
    file.write((const char*)(&head), sizeof(AVWavHead));
    file.write(data);
    file.close();
    return true;
}*/
