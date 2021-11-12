#include "ARRecorder.h"
#include <QDebug>

ARRecorder::ARRecorder(ARDataSource *source, QObject *parent)
    : QObject(parent)
    , sourcePtr(source)
    , audioInput(new ARDataInput(this))
{
    connect(audioInput, &ARDataInput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "input state changed" << state;
    });
}

ARRecorder::~ARRecorder()
{

}

void ARRecorder::setState(ARSpace::WorkState state)
{
    recordState = state;
}

void ARRecorder::record(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    //暂停继续
    if (getState() == ARSpace::RecordPaused) {
        resume();
        return;
    }
    stop();

    //录制时清空数据缓存
    sourcePtr->audioData.clear();

    if (audioInput->startRecord(sourcePtr->audioBuffer, device, format)) {
        //切换为录制状态
        setState(ARSpace::Recording);
    }
    else {
        qDebug() << "start record failed";
    }
}

void ARRecorder::suspend()
{
    if (getState() != ARSpace::Recording)
        return;
    audioInput->suspendRecord();
    setState(ARSpace::RecordPaused);
}

void ARRecorder::resume()
{
    if (getState() != ARSpace::RecordPaused)
        return;
    audioInput->resumeRecord();
    setState(ARSpace::Recording);
}

void ARRecorder::stop()
{
    audioInput->stopRecord();
    setState(ARSpace::Stopped);
}
