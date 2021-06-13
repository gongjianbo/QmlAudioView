#ifndef AUDIORECORDERINPUT_H
#define AUDIORECORDERINPUT_H

#include <QObject>
#include <QAudioInput>

#include "AudioRecorderDefine.h"
#include "AudioRecorderDevice.h"
#include "AudioRecorderBuffer.h"

/**
 * @brief 音频录制的输入
 * @author 龚建波
 * @date 2021-1-13
 */
class AudioRecorderInput : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorderInput(QObject *parent = nullptr);
    ~AudioRecorderInput();

    //开始录音
    //buffer:回调处理数据
    //device:输入设备信息
    //format:音频参数QAudioFormat，内部获取需要的成员值
    bool startRecord(AudioRecorderBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format);
    //结束录制
    void stopRecord();
    //暂停录制
    void suspendRecord();
    //暂停后恢复
    void resumeRecord();
    //结束并释放
    void freeRecord();
    //从文件读取
    //目前不带解析器，只能解析44字节定长wav-pcm格式头
    //（与本组件生成的wav文件格式一致）
    bool loadFromFile(AudioRecorderBuffer *buffer,const QString &filepath);

signals:
    void stateChanged(QAudio::State state);
    void notify();

private:
    //输入
    QAudioInput *audioInput=nullptr;
    //最新设置的deviceinfo
    QAudioDeviceInfo inputDevice;
    //当前使用的deviceinfo，start时不相同则重新new
    QAudioDeviceInfo currentDevice;
    //最新设置的format
    QAudioFormat inputFormat;
    //当前使用的录制format
    QAudioFormat currentFormat;

    friend class AudioRecorderOperate;
};

#endif // AUDIORECORDERINPUT_H
