#pragma once
#include <QQuickPaintedItem>
#include <QIODevice>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QTimer>
#include <QPainterPath>
// wav格式头的定义，在读写文件时用到
#include "AudioViewComponent/Common/AVWavDefine.h"

class SimpleAudioRecorder;
/**
 * @brief 用于读写数据时进行回调
 * QAudioInput/Output需要QIODevice类来读写数据
 * @author 龚建波
 * @date 2021-12-10
 * @history
 * 2021-12-29 修复播放游标的偏移问题，修复播放reset问题
 */
class SimpleAudioDevice : public QIODevice
{
public:
    explicit SimpleAudioDevice(SimpleAudioRecorder *recorder, QObject *parent);
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    SimpleAudioRecorder *recorderPtr;
};

/**
 * @brief 简易的音频录制组件，没有对各模块进行封装
 * @author 龚建波
 * @date 2021-12-10
 * @details
 * 建议版本，录音格式固定，输入输出设备也是用默认的
 * 16K采样、16位精度、单声道
 */
class SimpleAudioRecorder : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(SimpleAudioRecorder::RecorderState workState READ getWorkState NOTIFY workStateChanged)
    Q_PROPERTY(bool hasData READ getHasData NOTIFY hasDataChanged)
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ getDurationString NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ getPosition NOTIFY positionChanged)
    Q_PROPERTY(QString positionString READ getPositionString NOTIFY positionChanged)
public:
    //状态
    enum RecorderState
    {
        Stop,       //默认停止状态
        Playing,    //播放
        PlayPause,  //播放暂停
        Recording,  //录制
        RecordPause //录制暂停
    };
    Q_ENUM(RecorderState)
public:
    explicit SimpleAudioRecorder(QQuickItem *parent = nullptr);
    ~SimpleAudioRecorder();

    //当前状态
    SimpleAudioRecorder::RecorderState getWorkState() const;
    void setWorkState(SimpleAudioRecorder::RecorderState state);

    //当前是否有数据
    bool getHasData() const;
    void setHasData(bool has);

    //当前数据的总时长ms
    qint64 getDuration() const;
    void updateDuration();
    //将duration毫秒数转为时分秒格式
    QString getDurationString() const;

    //当前播放或者录制的时间ms
    qint64 getPosition() const;
    void updatePosition();
    QString getPositionString() const;

    //录制/播放时的回调接口
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    //更新绘制路径
    void updateSamplePath();

signals:
    void workStateChanged();
    void hasDataChanged();
    void durationChanged();
    void positionChanged();

public slots:
    //刷新ui
    void refresh();
    //停止播放/录制
    void stop();
    //播放
    void play();
    //播放暂停
    void playPause();
    //播放继续
    void playResume();
    //录制
    void record();
    //录制暂停
    void recordPause();
    //录制继续
    void recordResume();
    //保存数据到文件
    void saveFile(const QString &filepath);
    //从文件加载数据
    void loadFile(const QString &filepath);

private:
    //音频输入
    QAudioInput *audioInput{nullptr};
    //音频输出
    QAudioOutput *audioOutput{nullptr};
    // QAudioInput/Output需要QIODevice类来读写数据
    SimpleAudioDevice *audioDevice{nullptr};
    //音频格式，目前为固定
    QAudioFormat audioFormat;
    //当前状态
    SimpleAudioRecorder::RecorderState workState{SimpleAudioRecorder::Stop};
    //输出数据计数
    qint64 outputCount{0};
    //播放数据计数
    qint64 playCount{0};

    //数据缓冲区
    QByteArray audioData;
    bool hasData{false};
    //抽样数据连城的路径
    QPainterPath samplePath;
    //数据时长ms
    qint64 audioDuration{0};
    //播放或者录制时长ms
    qint64 audioPostion{0};

    //四个边距
    //该版本刻度是一体的，所以刻度的宽高也算在padding里
    int leftPadding{55};
    int rightPadding{15};
    int topPadding{15};
    int bottomPadding{15};
};
