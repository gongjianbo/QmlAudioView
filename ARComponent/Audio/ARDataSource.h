#pragma once
#include "ARDataBuffer.h"
#include "ARDevice.h"

/**
 * @brief 管理音频数据
 * player/recorder/view共享该对象
 * @author 龚建波
 * @date 2021-11-12
 */
class ARDataSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
public:
    explicit ARDataSource(QObject *parent = nullptr);
    ~ARDataSource();

    /// 音频数据时长
    qint64 getDuration() const;
    void setDuration(qint64 duration);

    /// 音频数据参数
    QAudioFormat getFormat() const;
    void setFormat(const QAudioFormat &format);
    /// 是否有数据
    bool isEmpty() const;
    /// 清空数据
    void clear();
    /// 音频数据
    QByteArray &getData();
    const QByteArray &getData() const;
    void setData(const QByteArray &data);
    void appendData(const QByteArray &data);
    /// 采样点数，如果singleChannel=true,表示只算单个通道的采样点数，否则为所有通道的
    qint64 getSampleCount(bool singleChannel = true) const;

private:
    /// 根据格式计算音频数据时长
    void calcDuration();

signals:
    void readFinished();
    void dataChanged();
    void formathChanged();
    void durationChanged(qint64 duration);

private:
    /// 音频数据
    QByteArray audioData;
    /// 数据格式，录制或读文件前设置
    QAudioFormat audioFormat;

    /// 音频时长，ms
    qint64 audioDuration{ 0 };

    friend class ARView;
};

