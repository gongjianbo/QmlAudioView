#ifndef AUDIORECORDEROPERATE_H
#define AUDIORECORDEROPERATE_H

#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "AudioRecorderDefine.h"
#include "AudioRecorderBuffer.h"
//#include "AudioRecorderInput.h"
//#include "AudioRecorderOutput.h"

/**
 * @brief 线程中进行的录制操作
 * @author 龚建波
 * @date 2021-1-12
 * @note
 * 1.输入输出使用默认的推拉模式start
 * 如果播放还是有卡顿的话替换为SDL
 * 2.绘图还是在view完成，绘制的数据及播放进度通过信号槽发送
 */
class AudioRecorderOperate : public QObject
        , public AudioRecorderCallback
{
    Q_OBJECT
public:
    explicit AudioRecorderOperate(QObject *parent = nullptr);

    //当前是否有数据
    bool getHasRecordData() const { return hasRecordData; }
    void setHasRecordData(bool has);

    //获取到的录音数据
    qint64 writeData(const char *data, qint64 maxSize) override;
    //导出缓存数据
    qint64 readData(char *data, qint64 maxSize) override;

signals:
    void hasRecordDataChanged(bool has);

private:
    //QAudioInput/Output处理数据时回调IODevice的接口
    AudioRecorderBuffer *audioBuffer=nullptr;
    //是否有录制的数据
    bool hasRecordData=false;
};

#endif // AUDIORECORDEROPERATE_H
