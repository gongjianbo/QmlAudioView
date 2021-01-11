#include "AudioRecorderHelper.h"

#include <Windows.h>
//#include <Dbt.h>

#include <QCoreApplication>

AudioRecorderHelper::AudioRecorderHelper(QObject *parent)
    : QObject(parent)
{
    thread=new QThread(this);
    operate=new AudioRecorderOperate;
    operate->moveToThread(thread);
    connect(thread,&QThread::started,operate,&AudioRecorderOperate::init);
    connect(thread,&QThread::finished,operate,&AudioRecorderOperate::free);

    thread->start();
}

AudioRecorderHelper::~AudioRecorderHelper()
{
    thread->quit();
    thread->wait();
}

AudioRecorderOperate::AudioRecorderOperate(QObject *parent)
    : QObject(parent)
{

}

void AudioRecorderOperate::init()
{
    allInputDevices=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    allOutputDevices=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    currentInputDevice=QAudioDeviceInfo::defaultInputDevice();
    currentOutputDevice=QAudioDeviceInfo::defaultOutputDevice();
    //注册到qApp过滤native事件
    qApp->installNativeEventFilter(this);
}

void AudioRecorderOperate::free()
{
    //貌似析构的时候自动会remove
    qApp->removeNativeEventFilter(this);
    deleteLater();
}

void AudioRecorderOperate::setCurrentInputDevice(const QAudioDeviceInfo &device)
{
    std::lock_guard<std::mutex> guard(datatMutex);
    Q_UNUSED(guard);
    currentInputDevice=device;
}

void AudioRecorderOperate::setCurrentOutputDevice(const QAudioDeviceInfo &device)
{
    std::lock_guard<std::mutex> guard(datatMutex); guard;
    Q_UNUSED(guard);
    currentOutputDevice=device;
}

bool AudioRecorderOperate::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        //设备插拔
        if(msg&&msg->message == WM_DEVICECHANGE)
        {
            //同步阻塞会有警告
            //Could not invoke audio interface activation.
            //(因为应用程序正在发送一个输入同步呼叫，所以无法执行传出的呼叫。)
            QTimer::singleShot(10,[this]{
                //一些详细信息需要注册，这里改为change就去查询输入列表是否变更
                //qDebug()<<"change"<<msg->wParam;
                //switch(msg->wParam)
                //{
                //default: break;
                //    //设备插入
                //case DBT_DEVICEARRIVAL:
                //    qDebug()<<"in";
                //    break;
                //    //设备移除
                //case DBT_DEVICEREMOVECOMPLETE:
                //    qDebug()<<"out";
                //    break;
                //}

                //使用中的设备不存在，停止，状态放到ui逻辑判断
                checkInputDevice();
                checkOutputDevice();
            });
        }
    }
    return false;
}

void AudioRecorderOperate::checkInputDevice()
{
    //检测输入设备信息是否改变
    QList<QAudioDeviceInfo> all_input=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if(all_input!=allInputDevices){
        allInputDevices=all_input;
        emit inputDevicesChanged(all_input);

        for(auto &info:all_input)
        {
            if(info.deviceName()==currentInputDevice.deviceName()
                    &&info.supportedSampleRates()==currentInputDevice.supportedSampleRates()){
                return;
            }
        }
        emit currentInputChanged();
    }
}

void AudioRecorderOperate::checkOutputDevice()
{
    //检测输出设备信息是否改变
    QList<QAudioDeviceInfo> all_output=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if(all_output!=allOutputDevices){
        allOutputDevices=all_output;
        emit outputDevicesChanged(all_output);

        for(auto &info:all_output)
        {
            if(info.deviceName()==currentOutputDevice.deviceName()
                    &&info.supportedSampleRates()==currentOutputDevice.supportedSampleRates()){
                return;
            }
        }
        emit currentOutputChanged();
    }
}
