#pragma once
#include <QObject>
#include <QTimer>
#include <QAudioDeviceInfo>
#include <QFutureWatcher>
#include <QAbstractNativeEventFilter>

#include "AudioRecorderDefine.h"

/**
 * @brief 管理输入输出设备/参数信息
 * @author 龚建波
 * @date 2021-1-12
 * @details
 * 1.默认以 16k 过滤设备列表
 * 2.如果输入or输出没有选择框，就reset为默认设备信息
 * 3.录制时ui先设置deviceinfo和format
 * 4.读文件或者录制完成根据数据修改format
 * 5.播放时先设置deviceinfo
 *
 * @note
 * 1.获取输入设备名重复是插件问题，在新版本可以判断插件（如5.15）
 * 参见：（Qt4）QTBUG-16841（Qt5）QTBUG-75781
 * 暂时用采样率来过滤输入设备
 * 2.插拔设备后，deviceInfo不相等了
 * 3.设备插拔时，可能会卡死应用，待测试
 */
class AudioRecorderDevice : public QObject,
        public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(QStringList inputDeviceNames READ getInputDeviceNames NOTIFY inputDeviceNamesChanged)
    Q_PROPERTY(QStringList outputDeviceNames READ getOutputDeviceNames NOTIFY outputDeviceNamesChanged)
    Q_PROPERTY(QString currentInputName READ getCurrentInputName NOTIFY currentInputNameChanged)
    Q_PROPERTY(QString currentOutputName READ getCurrentOutputName NOTIFY currentOutputNameChanged)
public:
    explicit AudioRecorderDevice(QObject* parent = nullptr);
    ~AudioRecorderDevice();

    //输入设备名列表
    QStringList getInputDeviceNames() const { return inputDeviceNames; }
    void setInputDeviceNames(const QStringList& names);

    //输出设备名列表
    QStringList getOutputDeviceNames() const { return outputDeviceNames; }
    void setOutputDeviceNames(const QStringList& names);

    //当前选中的输入设备
    QString getCurrentInputName() const { return currentInputDeviceInfo.deviceName(); }
    QAudioDeviceInfo getCurrentInputInfo() const { return currentInputDeviceInfo; }
    Q_INVOKABLE bool setCurrentInputName(const QString& name);
    Q_INVOKABLE bool setCurrentInputIndex(int index);
    Q_INVOKABLE void resetCurrentInput();
    //获取输入设备信息，devicename为空则用currentinfo
    QAudioDeviceInfo getInputInfo(const QString& name) const;

    //当前选中的输出设备
    QString getCurrentOutputName() const { return currentOutputDeviceInfo.deviceName(); }
    QAudioDeviceInfo getCurrentOutputInfo() const { return currentOutputDeviceInfo; }
    Q_INVOKABLE bool setCurrentOutputName(const QString& name);
    Q_INVOKABLE bool setCurrentOutputIndex(int index);
    Q_INVOKABLE void resetCurrentOutput();
    //获取输出设备信息，devicename为空则用currentinfo
    QAudioDeviceInfo getOutputInfo(const QString& name) const;

    //输入设备列表
    void setInputDeviceInfos(const QList<QAudioDeviceInfo>& infos);

    //输出设备列表
    void setOutputDeviceInfos(const QList<QAudioDeviceInfo>& infos);

protected:
    //用来过滤设备插拔事件
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

signals:
    void inputDeviceInfosUpdated(const QList<QAudioDeviceInfo>& infos);
    void outputDeviceInfosUpdated(const QList<QAudioDeviceInfo>& infos);
    void inputDeviceNamesChanged();
    void outputDeviceNamesChanged();
    void currentInputNameChanged();
    void currentOutputNameChanged();

public slots:
    //更新设备信息
    void updateDeviceInfos();

private:
    //多线程操作
    QFutureWatcher<void> futureWatcher;
    //延时刷新，防止短时间重复调用
    QTimer updateTimer;

    //检测到的输入设备列表（插件获取的，qt-win有两个插件，所以会重复）
    QList<QAudioDeviceInfo> allInputDeviceInfos;
    //可用的输入设备
    QList<QAudioDeviceInfo> enableInputDeviceInfos;
    //选中的输入设备
    QAudioDeviceInfo currentInputDeviceInfo;
    //检测到的输出设备列表（插件获取的，qt-win有两个插件，所以会重复）
    QList<QAudioDeviceInfo> allOutputDeviceInfos;
    //可用的输出设备
    QList<QAudioDeviceInfo> enableOutputDeviceInfos;
    //选中的输出设备
    QAudioDeviceInfo currentOutputDeviceInfo;
    //输入设备名列表
    QStringList inputDeviceNames;
    //输出设备名列表
    QStringList outputDeviceNames;
};
