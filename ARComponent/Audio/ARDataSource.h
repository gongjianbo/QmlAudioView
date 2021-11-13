#pragma once
#include "ARDataBuffer.h"
#include "ARDevice.h"

/**
 * @brief 管理音频数据
 * player/recorder/view共享该对象
 * @author 龚建波
 * @date 2021-11-12
 */
class ARDataSource : public QObject, public ARCallback
{
    Q_OBJECT
public:
    explicit ARDataSource(QObject *parent = nullptr);
    ~ARDataSource();

    /// 播放or导出
    qint64 readData(char *data, qint64 maxSize) override;
    /// 录音or读文件
    qint64 writeData(const char *data, qint64 maxSize) override;

    /// input/output录制播放需要的QIODevice参数
    ARDataBuffer *buffer();
    /// 结束播放，计数归0
    void stop();
    /// 音频数据参数
    QAudioFormat getFormat() const;
    void setFormat(const QAudioFormat &format);
    /// 是否有数据
    bool isEmpty() const;
    /// 清空数据
    void clear();
    /// 音频数据
    QByteArray &data();
    const QByteArray &data() const;
    void setData(const QByteArray &data);

private:
    /// 根据格式计算音频数据时长
    void calcDuration();
    void setDuration(qint64 duration);

signals:
    void readFinished();
    void dataChanged();
    void formathChanged();
    void durationChanged(qint64 duration);

private:
    /// QAudioInput/Output处理数据时回调IODevice的接口
    ARDataBuffer *audioBuffer{ nullptr };
    /// 音频数据
    QByteArray audioData;
    /// 数据格式，录制或读文件前设置
    QAudioFormat audioFormat;

    /// 音频时长，ms
    qint64 audioDuration{ 0 };
    /// 输出数据计数，对应read/write接口
    qint64 outputCount{ 0 };
};

