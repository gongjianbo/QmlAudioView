#include "AVAudioContext.h"

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

void AVAudioContext::record()
{
    audioOutput->stopPlay();
    audioInput->startRecord(QAudioDeviceInfo::defaultInputDevice(), audioSource->getFormat());
}

void AVAudioContext::stop()
{
    audioInput->stopRecord();
    audioOutput->stopPlay();
}
