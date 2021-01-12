#ifndef AUDIORECORDERVIEW_H
#define AUDIORECORDERVIEW_H

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "AudioRecorderDevice.h"

/**
 * @brief 录音可视化组件
 * @author 龚建波
 * @date 2021-1-12
 * @version v2.0
 * @details
 * 录制可参照Qt示例：audio和audioinput
 * 在操作录制、播放时会先调用stop，重置状态
 *
 * @note
 * 1.在初版逻辑的基础上引入线程进程来录制和播放
 */
class AudioRecorderView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(AudioRecorderView::RecordState recordState READ getRecordState NOTIFY recordStateChanged)
    Q_PROPERTY(AudioRecorderView::DisplayMode displayMode READ getDisplayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(AudioRecorderDevice *deviceInfo READ getDeviceInfo CONSTANT)
public:
    //状态
    enum RecordState
    {
        Stop       //默认停止状态
        ,Playing   //播放
        ,PlayPause //播放暂停
        ,Record    //录制
    };
    Q_ENUMS(RecordState)
    //显示模式
    enum DisplayMode
    {
        FullRange  //绘制全部数据
        ,Tracking  //跟踪最新数据
    };
    Q_ENUMS(DisplayMode)
public:
    explicit AudioRecorderView(QQuickItem *parent = nullptr);
    ~AudioRecorderView();

    //录制状态
    AudioRecorderView::RecordState getRecordState() const { return recordState; }
    void setRecordState(AudioRecorderView::RecordState state);

    //绘制模式，目前仅对录制有效，播放时展示全谱
    AudioRecorderView::DisplayMode getDisplayMode() const { return displayMode; }
    void setDisplayMode(AudioRecorderView::DisplayMode mode);

    //设备信息
    AudioRecorderDevice *getDeviceInfo() { return &deviceInfo; }

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void recordStateChanged();
    void displayModeChanged();

public slots:
    //刷新，调用update
    void refresh();

private:
    //当前状态
    RecordState recordState=Stop;
    //绘制模式
    DisplayMode displayMode=Tracking;
    //刷新定时器
    QTimer updateTimer;
    //刷新间隔
    QElapsedTimer updateElapse;

    //设备信息
    AudioRecorderDevice deviceInfo;
};

#endif // AUDIORECORDERVIEW_H
