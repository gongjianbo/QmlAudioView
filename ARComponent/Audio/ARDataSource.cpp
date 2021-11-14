#include "ARDataSource.h"
#include <QDebug>

ARDataSource::ARDataSource(QObject *parent)
    : QObject(parent)
{
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
}

qint64 ARDataSource::getDuration() const
{
    return audioDuration;
}

void ARDataSource::setDuration(qint64 duration)
{
    if (audioDuration != duration) {
        audioDuration = duration;
        qDebug()<<"duration"<<duration;
        emit durationChanged(duration);
    }
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
    audioData = data;
    calcDuration();
    emit dataChanged();
}

void ARDataSource::appendData(const QByteArray &data)
{
    audioData.append(data);
    //计算当前时长
    calcDuration();
    emit dataChanged();
}

qint64 ARDataSource::getSampleCount(bool singleChannel) const
{
    //除以采样点大小就是个数
    qint64 sample_count = audioData.size() / (audioFormat.sampleSize() / 8);
    if (singleChannel) {
        sample_count /= audioFormat.channelCount();
    }
    return sample_count;
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


