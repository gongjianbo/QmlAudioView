#pragma once
#include <QObject>
#include <QAudioOutput>

#include "ARDefine.h"
#include "ARDevice.h"
#include "ARDataBuffer.h"

/**
 * @brief 音频输出，播放或写文件
 * @author 龚建波
 * @date 2021-1-13
 */
class ARDataOutput : public QObject
{
    Q_OBJECT
public:
    explicit ARDataOutput(QObject *parent = nullptr);
    ~ARDataOutput();

    /**
     * @brief 开始播放
     * @param buffer 回调处理数据
     * @param device 输出设备名称
     * @param format 音频参数QAudioFormat，内部获取需要的成员值
     * @return =true则操作正常，开始播放
     */
    bool startPlay(ARDataBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format);
    /// 结束播放
    void stopPlay();
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
    bool saveToFile(const QByteArray data, const QAudioFormat &format, const QString &filepath);

signals:
    void stateChanged(QAudio::State state);
    void notify();

private:
    /// 输入
    QAudioOutput *audioOutput{ nullptr };
    /// 最新设置的deviceinfo
    QAudioDeviceInfo outputDevice;
    /// 当前使用的deviceinfo
    QAudioDeviceInfo currentDevice;
    /// 最新设置的format
    QAudioFormat outputFormat;
    /// 当前使用的输出format
    QAudioFormat currentFormat;

    friend class ARPlayer;
};
