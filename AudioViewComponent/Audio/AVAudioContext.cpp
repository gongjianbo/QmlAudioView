#include "AVAudioContext.h"
#include <QDebug>

AVAudioContext::AVAudioContext(QObject *parent)
    : QObject(parent)
{
    //数据
    audioSource = new AVDataSource(this);
    connect(audioSource, &AVDataSource::dataChanged, this, [this](){

    });

    //输入
    audioInput = new AVDataInput(this);
    audioInput->setAudioSource(audioSource);
    connect(audioInput, &AVDataInput::durationChanged, this, [this](){

    });

    //输出
    audioOutput = new AVDataOutput(this);
    audioOutput->setAudioSource(audioSource);
    connect(audioOutput, &AVDataOutput::positionChanged, this, [this](){

    });
    connect(audioOutput, &AVDataOutput::playFinished, this, &AVAudioContext::stop);
}

AVDataInput *AVAudioContext::getAudioInput()
{
    return audioInput;
}

AVDataOutput *AVAudioContext::getAudioOutput()
{
    return audioOutput;
}

AVDataSource *AVAudioContext::getAudioSource()
{
    return audioSource;
}

AVGlobal::WorkState AVAudioContext::getState() const
{
    return workState;
}

void AVAudioContext::setState(AVGlobal::WorkState state)
{
    if (state == workState) {
        return;
    }
    const auto old_state = workState;
    workState = state;
    emit stateChanged(state, old_state);
}

void AVAudioContext::record()
{
    if (getState() == AVGlobal::Recording) {
        //重复点录制则返回
        return;
    }
    else if (getState() == AVGlobal::RecordPaused) {
        //暂停则恢复录制
        resume();
        return;
    }
    else if (getState() != AVGlobal::Stopped) {
        //在播放则先停止
        audioOutput->stopPlay();
    }
    //开始录制
    audioInput->startRecord(QAudioDeviceInfo::defaultInputDevice(), audioSource->getFormat());
    setState(AVGlobal::Recording);
}

void AVAudioContext::play()
{
    if (getState() == AVGlobal::Playing) {
        //重复点播放则返回
        return;
    }
    else if (getState() == AVGlobal::PlayPaused) {
        //暂停则恢复播放
        resume();
        return;
    }
    else if (getState() != AVGlobal::Stopped) {
        //在录制则先停止
        audioInput->stopRecord();
    }
    //开始播放
    audioOutput->startPlay(QAudioDeviceInfo::defaultOutputDevice(), audioSource->getFormat());
    setState(AVGlobal::Playing);
}

void AVAudioContext::suspend()
{
    if (getState() == AVGlobal::Recording) {
        //在录制则暂停录制
        audioInput->suspendRecord();
        setState(AVGlobal::RecordPaused);
    }
    else if (getState() == AVGlobal::Playing) {
        //在播放则暂停播放
        audioOutput->suspendPlay();
        setState(AVGlobal::PlayPaused);
    }
}

void AVAudioContext::resume()
{
    if (getState() == AVGlobal::RecordPaused) {
        //继续录制
        audioInput->resumeRecord();
        setState(AVGlobal::Recording);
    }
    else if (getState() == AVGlobal::PlayPaused) {
        //继续播放
        audioOutput->resumePlay();
        setState(AVGlobal::Playing);
    }
}

void AVAudioContext::stop()
{
    if (getState() == AVGlobal::Recording || getState() == AVGlobal::RecordPaused) {
        //在录制则停止录制
        audioInput->stopRecord();
    }
    else if (getState() == AVGlobal::Playing || getState() == AVGlobal::PlayPaused) {
        //在播放则停止播放
        audioOutput->stopPlay();
    }
    setState(AVGlobal::Stopped);
}

bool AVAudioContext::loadFile(const QString &filepath)
{
    //停止当前播放-播放位置置零-加载数据
    stop();
    //setPosition(0);
    const bool ret = audioInput->loadFile(filepath);
    qDebug() << __FUNCTION__ << ret << filepath;
    return ret;
}

bool AVAudioContext::saveFile(const QString &filepath)
{
    const bool ret = audioOutput->saveFile(filepath);
    qDebug() << __FUNCTION__ << ret << filepath;
    return ret;
}
