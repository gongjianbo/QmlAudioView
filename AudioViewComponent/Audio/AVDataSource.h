#pragma once
#include "Common/AVGlobal.h"
#include "AVDataBuffer.h"

/**
 * @brief 管理音频数据
 * player/recorder/view等共享该对象，操作同一数据块
 * @author 龚建波
 * @date 2021-12-28
 * @details
 * 音频数据的存储由QByteArray替换为std::vector，在Qt6之前QByteArray等容器有2GB限制
 * 接口参数使用qint64而不是无符号数，是为了和Qt相关类的接口保持一致（可以定义一个宏）
 */
class AVDataSource : public QObject
{
    Q_OBJECT
    /// 音频数据参数
    Q_PROPERTY(QAudioFormat format READ getFormat WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(int sampleRate READ getSampleRate WRITE setSampleRate NOTIFY formatChanged)
    Q_PROPERTY(int sampleSize READ getSampleSize WRITE setSampleSize NOTIFY formatChanged)
    Q_PROPERTY(int channelCount READ getChannelCount WRITE setChannelCount NOTIFY formatChanged)
    /// 音频数据时长，单位ms毫秒
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
public:
    explicit AVDataSource(QObject *parent = nullptr);
    ~AVDataSource();

    /// 音频数据参数
    QAudioFormat getFormat() const;
    void setFormat(const QAudioFormat &format);

    /// 采样率参数
    int getSampleRate() const;
    void setSampleRate(int sampleRate);

    /// 采样精度参数
    int getSampleSize() const;
    void setSampleSize(int sampleSize);

    /// 通道数
    int getChannelCount() const;
    void setChannelCount(int channelCount);

    /// 音频数据时长，单位ms毫秒
    qint64 getDuration() const;

    /// 是否有数据
    bool isEmpty() const;
    /// 清空数据
    void clear();
    /// 数据长度size_type无符号类型转为qint64和其他接口兼容
    qint64 size() const;
    /// 最大存储长度
    qint64 maxSize() const;
    /// 音频数据
    std::vector<char> &getData();
    const std::vector<char> &getData() const;
    void setData(const std::vector<char> &data);
    void appendData(const std::vector<char> &data);
    void setData(const char *data, qint64 len);
    void appendData(const char *data, qint64 len);
    /// 采样点数，如果singleChannel=true,表示只算单个通道的采样点数，否则为所有通道的
    qint64 getSampleCount(bool singleChannel = true) const;

private:
    /// 根据格式计算音频数据时长
    void calcDuration();

signals:
    void readFinished();
    void dataChanged();
    void formatChanged();
    void durationChanged(qint64 duration);

private:
    /// 数据格式，录制或读文件前设置
    QAudioFormat audioFormat;
    /// 音频数据
    std::vector<char> audioData;

    /// 音频时长，ms
    qint64 audioDuration{ 0 };
};
