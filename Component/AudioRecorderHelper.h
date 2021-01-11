#ifndef AUDIORECORDERHELPER_H
#define AUDIORECORDERHELPER_H

#include <mutex>

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QAbstractNativeEventFilter>

#include "AudioRecorderDevice.h"

class AudioRecorderOperate;

/**
 * @brief 管理线程中的Operate对象
 * @author 龚建波
 * @date 2021-1-11
 */
class AudioRecorderHelper : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorderHelper(QObject *parent = nullptr);
    ~AudioRecorderHelper();

    AudioRecorderOperate *getOperate() { return operate; }

private:
    QThread *thread;
    AudioRecorderOperate *operate;
};

/**
 * @brief 本类置于线程中进行操作
 * @author 龚建波
 * @date 2021-1-11
 * @details
 * 1. 监测设备插拔
 */
class AudioRecorderOperate : public QObject
        , public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit AudioRecorderOperate(QObject *parent = nullptr);

    void init();
    void free();

    //更新当前使用的输入设备信息
    void setCurrentInputDevice(const QAudioDeviceInfo &device);
    //更新当前使用的输出设备信息
    void setCurrentOutputDevice(const QAudioDeviceInfo &device);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

private:
    //插拔设备时去检测设备信息
    void checkInputDevice();
    void checkOutputDevice();

signals:
    //设备信息列表
    void inputDevicesChanged(const QList<QAudioDeviceInfo> &devices);
    void outputDevicesChanged(const QList<QAudioDeviceInfo> &devices);
    //设备改变
    void currentInputChanged();
    void currentOutputChanged();

private:
    std::mutex datatMutex;
    //可用的输入设备列表
    QList<QAudioDeviceInfo> allInputDevices;
    //可用的输出设备列表
    QList<QAudioDeviceInfo> allOutputDevices;

    //当前使用的输入设备信息
    QAudioDeviceInfo currentInputDevice;
    //当前使用的输出设备信息
    QAudioDeviceInfo currentOutputDevice;
};

#endif // AUDIORECORDERHELPER_H
