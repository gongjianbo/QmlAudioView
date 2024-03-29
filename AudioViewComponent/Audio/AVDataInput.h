#pragma once
#include <QObject>
#include <QAudioInput>

#include "Common/AVGlobal.h"
#include "AVDataBuffer.h"
#include "AVDataSource.h"

/**
 * @brief 音频输入，录制或者读文件
 * @author 龚建波
 * @date 2021-1-13
 * @history
 * 2022-01-15 构造去掉AVDataSource参数，并在使用前判断是否为空指针
 *            使之可以在QML中创建，并自由组合
 */
class AVDataInput : public QObject, public AVCallback
{
    Q_OBJECT
    /// 数据存放
    Q_PROPERTY(AVDataSource* audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged)
    /// 音频时长 ms
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
public:
    explicit AVDataInput(QObject *parent = nullptr);
    ~AVDataInput();

    /// 录音or读文件
    qint64 writeData(const char *data, qint64 maxSize) override;

    /// 数据存放
    AVDataSource *getAudioSource();
    void setAudioSource(AVDataSource *source);

    /// 音频数据时长
    qint64 getDuration() const;
    void setDuration(qint64 duration);

    /// 状态
    QAudio::State getState() const;

    /**
     * @brief 开始录音
     * @param device 输入设备信息
     * @param format 音频参数QAudioFormat，内部获取需要的成员值
     * @return =true则操作正常，开始录音
     */
    bool startRecord(const QAudioDeviceInfo &device, const QAudioFormat &format);
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
     * @param filepath 文件路径
     * @return =true则操作成功
     */
    bool loadFile(const QString &filepath);

signals:
    void errorChanged(AVGlobal::ErrorType error);
    void stateChanged(QAudio::State state);
    void audioSourceChanged();
    void durationChanged(qint64 duration);

private slots:
    /// QAudioInput::notify
    void onNotify();

private:
    /// QAudioInput/Output处理数据时回调IODevice的接口
    AVDataBuffer *audioBuffer{ nullptr };
    /// 数据存放
    AVDataSource *audioSource{ nullptr };
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

    /// 音频时长，ms
    qint64 inputDuration{ 0 };
};
