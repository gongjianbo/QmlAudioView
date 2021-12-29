#pragma once
#include <QObject>
#include <QAudioOutput>

#include "Common/AVGlobal.h"
#include "AVDataBuffer.h"
#include "AVDataSource.h"

/**
 * @brief 音频输出，播放或写文件
 * @author 龚建波
 * @date 2021-1-13
 */
class AVDataOutput : public QObject, public AVCallback
{
    Q_OBJECT
    /// 播放位置，单位ms毫秒
    Q_PROPERTY(qint64 position READ getPosition NOTIFY positionChanged)
public:
    explicit AVDataOutput(AVDataSource *source, QObject *parent = nullptr);
    ~AVDataOutput();

    /// 播放or导出
    qint64 readData(char *data, qint64 maxSize) override;

    /// 播放位置，单位ms毫秒
    qint64 getPosition() const;
    void setPosition(qint64 position);

    /// 播放位置，数据数组下标
    qint64 getCurrentIndex() const;
    void setCurrentIndex(qint64 index);
    /// setCurrentIndex=0时，position为1frame，zero则将两者置0
    void zeroCurrentIndex();

    /// 播放起始位置
    qint64 getStartIndex() const;
    void setStartIndex(qint64 index);

    /// 状态
    QAudio::State getState() const;

    /**
     * @brief 开始播放
     * @param device 输出设备名称
     * @param format 音频参数QAudioFormat，内部获取需要的成员值
     * @return =true则操作正常，开始播放
     */
    bool startPlay(const QAudioDeviceInfo &device, const QAudioFormat &format);
    /// 结束播放，重置指示器位置到0
    void stopPlay();
    /// 开始播放前先结束播放，如果已播放到尾则重置播放起始
    void endPlay();
    /// 暂停播放
    void suspendPlay();
    /// 暂停后恢复
    void resumePlay();
    /// 释放
    void freePlay();

    /**
     * @brief 数据保存到文件
     * @param data 音频数据
     * @param format 数据格式参数
     * @param filepath 文件路径
     * @return =true则操作成功
     */
    //bool saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath);

signals:
    void errorChanged(AVGlobal::ErrorType error);
    void stateChanged(QAudio::State state);
    void notify();
    void playFinished();
    void positionChanged(qint64 position);
    void currentIndexChanged(qint64 index);
    void startIndexChanged(qint64 index);

private:
    /// QAudioInput/Output处理数据时回调IODevice的接口
    AVDataBuffer *audioBuffer{ nullptr };
    /// 数据存放
    AVDataSource *audioSource{ nullptr };

    /// 输出
    QAudioOutput *audioOutput{ nullptr };
    /// 最新设置的deviceinfo
    QAudioDeviceInfo outputDevice;
    /// 当前使用的deviceinfo
    QAudioDeviceInfo currentDevice;
    /// 最新设置的format
    QAudioFormat outputFormat;
    /// 当前使用的输出format
    QAudioFormat currentFormat;

    /// 输出数据计数，对应read/write接口
    qint64 outputCount{ 0 };
    /// 继续播放的偏移
    qint64 outputOffset{ 0 };

    /// 播放点的时间，单位ms毫秒
    qint64 playPosition{ 0 };
    /// 播放点的数据数组下标，当前位置
    qint64 playCurrentIndex{ 0 };
    /// 播放点的数据数组下标，开始播放的位置
    /// 可以通过点击波形图任意位置修改起始点
    qint64 playStartIndex{ 0 };
};
