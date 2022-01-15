#include "AVDataSource.h"
#include <QDebug>

AVDataSource::AVDataSource(QObject *parent)
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

AVDataSource::~AVDataSource()
{
}

QAudioFormat AVDataSource::getFormat() const
{
    return audioFormat;
}

void AVDataSource::setFormat(const QAudioFormat &format)
{
    if (audioFormat != format) {
        audioFormat = format;
        qDebug() << __FUNCTION__ << format;
        calcDuration();
        emit formatChanged();
    }
}

int AVDataSource::getSampleRate() const
{
    return audioFormat.sampleRate();
}

void AVDataSource::setSampleRate(int sampleRate)
{
    QAudioFormat format = getFormat();
    if (format.sampleRate() != sampleRate) {
        format.setSampleRate(sampleRate);
        setFormat(format);
    }
}

int AVDataSource::getSampleSize() const
{
    return audioFormat.sampleSize();
}

void AVDataSource::setSampleSize(int sampleSize)
{
    QAudioFormat format = getFormat();
    if (format.sampleSize() != sampleSize) {
        format.setSampleSize(sampleSize);
        setFormat(format);
    }
}

int AVDataSource::getChannelCount() const
{
    return audioFormat.channelCount();
}

void AVDataSource::setChannelCount(int channelCount)
{
    QAudioFormat format = getFormat();
    if (format.channelCount() != channelCount) {
        format.setChannelCount(channelCount);
        setFormat(format);
    }
}

qint64 AVDataSource::getDuration() const
{
    return audioDuration;
}

bool AVDataSource::isEmpty() const
{
    return audioData.empty();
}

void AVDataSource::clear()
{
    audioData.clear();
    emit dataChanged();

    calcDuration();
}

qint64 AVDataSource::size() const
{
    return (qint64)audioData.size();
}

std::vector<char> &AVDataSource::getData()
{
    return audioData;
}

const std::vector<char> &AVDataSource::getData() const
{
    return audioData;
}

void AVDataSource::setData(const std::vector<char> &data)
{
    if (data.size() >= audioData.max_size()) {
        qDebug() << __FUNCTION__ << "data size out of range";
        audioData.clear();
    }
    else {
        audioData = data;
    }
    emit dataChanged();

    calcDuration();
}

void AVDataSource::appendData(const std::vector<char> &data)
{
    if (audioData.size() + data.size() >= audioData.max_size()) {
        qDebug() << __FUNCTION__ << "data size out of range";
        return;
    }
    audioData.insert(audioData.end(), data.cbegin(), data.cend());
    emit dataChanged();

    calcDuration();
}

qint64 AVDataSource::getSampleCount(bool singleChannel) const
{
    //除以采样点大小就是个数
    qint64 sample_count = audioData.size() / (audioFormat.sampleSize() / 8);
    if (singleChannel) {
        sample_count /= audioFormat.channelCount();
    }
    return sample_count;
}

void AVDataSource::calcDuration()
{
    //根据音频数据的参数和数据长度进行计算
    const int sample_rate = audioFormat.sampleRate();
    const int sample_byte = audioFormat.sampleSize() / 8;
    const int channel_count = audioFormat.channelCount();
    qint64 duration = 0;
    if (audioData.size() > 0 && sample_rate > 0 && sample_byte > 0 && channel_count > 0) {
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        duration = (audioData.size() / sample_byte) / (1.0 * channel_count * sample_rate) * 1000;
    }
    //更新时长信息
    if (audioDuration != duration) {
        audioDuration = duration;
        //qDebug()<<__FUNCTION__<<duration;
        emit durationChanged(duration);
    }
}
