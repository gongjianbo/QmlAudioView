#pragma once
#include <QObject>
#include <QAudioOutput>

#include "AudioRecorderDefine.h"
#include "AudioRecorderDevice.h"
#include "AudioRecorderBuffer.h"

/**
 * @brief 音频播放和导出
 * @author 龚建波
 * @date 2021-1-13
 */
class AudioRecorderOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorderOutput(QObject* parent = nullptr);
    ~AudioRecorderOutput();

    //开始播放
    //buffer:回调处理数据
    //device:输出设备名称
    //format:音频参数QAudioFormat，内部获取需要的成员值
    bool startPlay(AudioRecorderBuffer* buffer, const QAudioDeviceInfo& device, const QAudioFormat& format);
    //结束播放
    void stopPlay();
    //暂停播放
    void suspendPlay();
    //暂停后恢复
    void resumePlay();
    //释放
    void freePlay();

    //保存到文件
    bool saveToFile(const QByteArray data, const QAudioFormat& format, const QString& filepath);

signals:
    void stateChanged(QAudio::State state);
    void notify();

private:
    //输入
    QAudioOutput* audioOutput{ nullptr };
    //最新设置的deviceinfo
    QAudioDeviceInfo outputDevice;
    //当前使用的deviceinfo
    QAudioDeviceInfo currentDevice;
    //最新设置的format
    QAudioFormat outputFormat;
    //当前使用的输出format
    QAudioFormat currentFormat;

    friend class AudioRecorderOperate;
};
