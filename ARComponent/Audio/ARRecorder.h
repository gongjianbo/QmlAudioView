#pragma once
#include "ARDataInput.h"
#include "ARDataSource.h"
#include "ARDevice.h"

/**
 * @brief 音频录制
 * @author 龚建波
 * @date 2021-11-12
 */
class ARRecorder : public QObject
{
    Q_OBJECT
public:
    explicit ARRecorder(ARDataSource *source, QObject *parent = nullptr);
    ~ARRecorder();

    /// 状态
    ARSpace::WorkState getState() const { return recordState; }
    void setState(ARSpace::WorkState state);

    /// 开始录制
    void record(const QAudioDeviceInfo &device, const QAudioFormat &format);
    /// 暂停录制
    void suspend();
    /// 恢复录制
    void resume();
    /// 停止播放
    void stop();
    /// todo 读取文件

private:
    /// 当前状态
    ARSpace::WorkState recordState{ ARSpace::Stopped };
    /// 数据
    ARDataSource *sourcePtr{ nullptr };
    /// 音频输入
    ARDataInput *audioInput{ nullptr };
};
