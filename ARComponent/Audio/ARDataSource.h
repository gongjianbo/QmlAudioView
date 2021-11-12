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


signals:
    void readFinished();
    void dataChanged();

public:
    /// QAudioInput/Output处理数据时回调IODevice的接口
    ARDataBuffer *audioBuffer{ nullptr };
    /// 音频数据
    QByteArray audioData;

    /// 输出数据计数，对应read/write接口
    qint64 outputCount{ 0 };
};

