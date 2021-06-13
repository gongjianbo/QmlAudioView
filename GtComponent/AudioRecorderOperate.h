#ifndef AUDIORECORDEROPERATE_H
#define AUDIORECORDEROPERATE_H

#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "AudioRecorderDefine.h"
#include "AudioRecorderDevice.h"
#include "AudioRecorderBuffer.h"
#include "AudioRecorderInput.h"
#include "AudioRecorderOutput.h"

/**
 * @brief 线程中进行的录制操作
 * @author 龚建波
 * @date 2021-1-12
 * @note
 * 1.输入输出目前使用默认的推拉模式start
 * 2.绘图还是在view完成，绘制的数据及播放进度通过信号槽发送
 * @todo
 * ui改为定时器滚动，这样就不受数据获取不及时的影响
 */
class AudioRecorderOperate : public QObject
        , public AudioRecorderCallback
{
    Q_OBJECT
public:
    explicit AudioRecorderOperate(QObject *parent = nullptr);
    ~AudioRecorderOperate();

    //录制状态
    AudioRecorder::RecordState getRecordState() const { return recordState; }
    void setRecordState(AudioRecorder::RecordState state);

    //获取到的录音数据
    qint64 writeData(const char *data, qint64 maxSize) override;
    //导出缓存数据
    qint64 readData(char *data, qint64 maxSize) override;

    //计算音频时长
    void calcDuration();
    //计算播放时长
    void calcPosition();
    //播放进度字节游标
    void setAudioCursor(qint64 cursor);

signals:
    void recordStateChanged(AudioRecorder::RecordState state);
    void dataChanged(const QByteArray &newData);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void cursorChanged(qint64 cursor);
    void loadFileFinished(const QString &filepath,bool result);
    void saveFileFinished(const QString &filepath,bool result);

public slots:
    void init();
    void stop(bool update=false);
    //录制
    void doRecord(const QAudioDeviceInfo &device, const QAudioFormat &format);
    //停止录制/播放
    void doStop();
    //播放
    void doPlay(const QAudioDeviceInfo &device);
    //暂停播放
    void doSuspendPlay();
    //暂停恢复
    void doResumePlay();
    //读文件
    void doLoadFile(const QString &filepath);
    //写文件
    void doSaveFile(const QString &filepath);

private:
    //QAudioInput/Output处理数据时回调IODevice的接口
    AudioRecorderBuffer *audioBuffer=nullptr;
    //音频输入
    AudioRecorderInput *audioInput=nullptr;
    //音频输出
    AudioRecorderOutput *audioOutput=nullptr;
    //当前状态
    AudioRecorder::RecordState recordState=AudioRecorder::Stop;

    //数据缓冲区
    QByteArray audioData;
    //输出数据计数，对应read/write接口
    qint64 outputCount=0;
    //播放数据计数，对应ui游标/audioOutput->processedUSecs()
    qint64 audioCursor=0;
    //数据时长ms
    qint64 audioDuration=0;
    //播放或者录制时长ms
    qint64 audioPostion=0;
};

#endif // AUDIORECORDEROPERATE_H
