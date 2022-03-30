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
