#include "ARDataSource.h"
#include <QDebug>

ARDataSource::ARDataSource(QObject *parent)
    : QObject(parent)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new ARDataBuffer(this, this);
    audioBuffer->open(QIODevice::ReadWrite);

    //采样精度和声道数暂时默认16\1
    //默认参数可以放到全局配置
    audioFormat.setSampleRate(16000);
    audioFormat.setSampleSize(16);
    audioFormat.setChannelCount(1);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
}

ARDataSource::~ARDataSource()
{
    audioBuffer->close();
}

qint64 ARDataSource::readData(char *data, qint64 maxSize)
{
    if (!data || maxSize < 1)
        return 0;
    const int data_size = audioData.count() - outputCount;
    if (data_size <= 0) {
        //stateChanged没有触发停止，懒得判断notify了
        QTimer::singleShot(1, [this] { readFinished(); });
        return 0;
    }

    const int read_size = (data_size >= maxSize) ? maxSize : data_size;
    memcpy(data, audioData.constData() + outputCount, read_size);
    outputCount += read_size;
    //refresh(); 这个间隔回调太大了，不适合用来刷新播放进度
    return read_size;
}

qint64 ARDataSource::writeData(const char *data, qint64 maxSize)
{
    //双声道时数据为一左一右连续
    QByteArray new_data = QByteArray(data, maxSize);
    audioData.append(new_data);
    //计算当前时长
    calcDuration();
    emit dataChanged();
    return maxSize;
}

qint64 ARDataSource::getDuration() const
{
    return audioDuration;
}

ARDataBuffer *ARDataSource::buffer()
{
    return audioBuffer;
}

void ARDataSource::stop()
{
    outputCount = 0;
}

QAudioFormat ARDataSource::getFormat() const
{
    return audioFormat;
}

void ARDataSource::setFormat(const QAudioFormat &format)
{
    if (audioFormat != format) {
        audioFormat = format;
        calcDuration();
        emit formathChanged();
    }
}

bool ARDataSource::isEmpty() const
{
    return audioData.isEmpty();
}

void ARDataSource::clear()
{
    outputCount = 0;
    audioData.clear();
    setDuration(0);
    emit dataChanged();
}

QByteArray &ARDataSource::getData()
{
    return audioData;
}

const QByteArray &ARDataSource::getData() const
{
    return audioData;
}

void ARDataSource::setData(const QByteArray &data)
{
    outputCount = 0;
    audioData = data;
    calcDuration();
    emit dataChanged();
}

void ARDataSource::calcDuration()
{
    //更新时长信息
    const int sample_rate = audioFormat.sampleRate();
    const int sample_byte = audioFormat.sampleSize() / 8;
    const int channel_count = audioFormat.channelCount();
    qint64 duration = 0;
    if (sample_rate > 0 && audioData.size() > 0) {
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        duration = (audioData.size() / sample_byte) / (1.0 * channel_count * sample_rate) * 1000;
    }
    setDuration(duration);
}

void ARDataSource::setDuration(qint64 duration)
{
    if (audioDuration != duration) {
        audioDuration = duration;
        qDebug()<<"duration"<<duration;
        emit durationChanged(duration);
    }
}
