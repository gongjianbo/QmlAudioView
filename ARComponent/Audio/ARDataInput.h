#pragma once
#include <QObject>
#include <QAudioInput>

#include "ARDefine.h"
#include "ARDevice.h"
#include "ARDataBuffer.h"

/**
 * @brief 音频输入，录制或者读文件
 * @author 龚建波
 * @date 2021-1-13
 */
class ARDataInput : public QObject
{
    Q_OBJECT
public:
    explicit ARDataInput(QObject *parent = nullptr);
    ~ARDataInput();

    /**
     * @brief 开始录音
     * @param buffer 回调处理数据
     * @param device 输入设备信息
     * @param format 音频参数QAudioFormat，内部获取需要的成员值
     * @return =true则操作正常，开始录音
     */
    bool startRecord(ARDataBuffer *buffer, const QAudioDeviceInfo &device, const QAudioFormat &format);
    /// 结束录制
    void stopRecord();
    /// 暂停录制
    void suspendRecord();
    /// 暂停后恢复
    void resumeRecord();
    /// 结束并释放
    void freeRecord();

    /**
     * @brief 从文件读取数据
     * 目前不带解析器，只能解析44字节定长wav-pcm格式头
     * （与本组件生成的wav文件格式一致）
     * @param data 引用返回读取的数据
     * @param format 文件数据格式参数，引用返回
     * @param filepath 文件路径
     * @return =true则操作成功
     */
    bool loadFromFile(QByteArray &data, QAudioFormat &format, const QString &filepath);

signals:
    void stateChanged(QAudio::State state);
    void notify();

private:
    /// 输入
    QAudioInput *audioInput{ nullptr };
    /// 最新设置的deviceinfo
    QAudioDeviceInfo inputDevice;
    /// 当前使用的deviceinfo，start时不相同则重新new
    QAudioDeviceInfo currentDevice;
    /// 最新设置的format
    QAudioFormat inputFormat;
    /// 当前使用的录制format
    QAudioFormat currentFormat;

    friend class ARRecorder;
};
