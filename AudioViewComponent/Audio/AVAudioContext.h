#pragma once
#include "AVDataInput.h"
#include "AVDataOutput.h"
#include "AVDataSource.h"

/**
 * @brief 执行录制和播放等操作
 * @author 龚建波
 * @date 2022-01-15
 * @todo 当前input/output不可变更，还不能灵活的组合
 */
class AVAudioContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AVDataInput* audioInput READ getAudioInput NOTIFY audioInputChanged)
    Q_PROPERTY(AVDataOutput* audioOutput READ getAudioOutput NOTIFY audioOutputChanged)
    Q_PROPERTY(AVDataSource* audioSource READ getAudioSource NOTIFY audioSourceChanged)
    Q_PROPERTY(AVGlobal::WorkState state READ getState NOTIFY stateChanged)
public:
    explicit AVAudioContext(QObject *parent = nullptr);

    /// 音频输入-录制or文件读取
    AVDataInput *getAudioInput();
    //void setAudioInput(AVDataInput *input);

    /// 音频输出-播放or写入文件
    AVDataOutput *getAudioOutput();
    //void setAudioOutput(AVDataOutput *output);

    /// 数据存放-使输入/输出/图表可共享数据
    AVDataSource *getAudioSource();
    //void setAudioSource(AVDataSource *source);

    /// 播放/录音等工作状态
    AVGlobal::WorkState getState() const;
    void setState(AVGlobal::WorkState state);

signals:
    void audioInputChanged();
    void audioOutputChanged();
    void audioSourceChanged();
    //todo状态变更后通知刷新
    void stateChanged(AVGlobal::WorkState newState, AVGlobal::WorkState oldState);

public slots:
    /// 录音
    void record();
    /// 播放
    void play();
    /// 暂停播放或录制
    void suspend();
    /// 恢复播放或录制
    void resume();
    /// 停止播放或录制
    void stop();

    /// 读文件
    bool loadFile(const QString &filepath);
    /// 写文件
    bool saveFile(const QString &filepath);

private:
    /// 音频输入-录制or文件读取
    AVDataInput *audioInput{nullptr};
    /// 音频输出-播放or写入文件
    AVDataOutput *audioOutput{nullptr};
    /// 数据存放-使输入/输出/图表可共享数据
    AVDataSource *audioSource{nullptr};

    /// 播放/录音等工作状态
    AVGlobal::WorkState workState{AVGlobal::Stopped};
};
