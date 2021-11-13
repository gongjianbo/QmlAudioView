#pragma once
#include "ARDataOutput.h"
#include "ARDataSource.h"
#include "ARDevice.h"

/**
 * @brief 音频播放
 * @author 龚建波
 * @date 2021-11-12
 */
class ARPlayer : public QObject
{
    Q_OBJECT
public:
    explicit ARPlayer(ARDataSource *source, QObject *parent = nullptr);
    ~ARPlayer();

    /// 状态
    ARSpace::WorkState getState() const;
    void setState(ARSpace::WorkState state);

    /// 开始播放
    void play(const QAudioDeviceInfo &device);
    /// 暂停播放
    void suspend();
    /// 暂停恢复
    void resume();
    /// 停止播放
    void stop();

private:
    /// 当前状态
    ARSpace::WorkState playState{ ARSpace::Stopped };
    /// 数据
    ARDataSource *sourcePtr{ nullptr };
    /// 音频输出
    ARDataOutput *audioOutput{ nullptr };
};

