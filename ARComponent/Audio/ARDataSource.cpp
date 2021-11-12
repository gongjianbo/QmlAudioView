#include "ARDataSource.h"

ARDataSource::ARDataSource(QObject *parent)
    : QObject(parent)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new ARDataBuffer(this, this);
    audioBuffer->open(QIODevice::ReadWrite);
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

    emit dataChanged();
    return maxSize;
}
