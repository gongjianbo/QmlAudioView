#include "ARPlayer.h"
#include <QDebug>

ARPlayer::ARPlayer(ARDataSource *source, QObject *parent)
    : QObject(parent)
    , sourcePtr(source)
    , audioOutput(new ARDataOutput(this))
{
    connect(audioOutput, &ARDataOutput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "output state changed" << state;
    });
    /*//更新播放进度游标
    connect(audioOutput, &ARDataOutput::notify, this, [this] {
        if (audioOutput && audioOutput->audioOutput && audioDuration) {
            //用processedUSecs获取start到当前的us数，但是start后有点延迟
            //进度=已放时间和总时间之比*总字节数，注意时间单位
            //-50是为了补偿时差，音画同步，这个50就是output的NotifyInterval
            //（还有点问题就是快结束的时候尾巴上那点直接结束了，数据少的时候明显点）
            qint64 cursor = cursorOffset + (audioOutput->audioOutput->processedUSecs() / 1000.0 - 50) / audioDuration * audioData.count();
            //qDebug()<<"notify"<<cursor<<cursorOffset<<outputCount;
            if (cursor > outputCount)
                cursor = outputCount;
            setAudioCursor(cursor);
            calcPosition();
        }
    });*/
}

ARPlayer::~ARPlayer()
{

}

ARSpace::WorkState ARPlayer::getState() const
{
    return playState;
}

void ARPlayer::setState(ARSpace::WorkState state)
{
    playState = state;
}

void ARPlayer::play(const QAudioDeviceInfo &device)
{
    if (getState() == ARSpace::WorkState::PlayPaused) {
        resume();
        return;
    }
    stop();

    if (sourcePtr->isEmpty())
        return;

    //todo format
    QAudioFormat format;
    if (audioOutput->startPlay(sourcePtr->buffer(), device, format)) {
        //切换为播放状态
        setState(ARSpace::Playing);
    }
    else {
        qDebug() << "start play failed";
    }
}

void ARPlayer::suspend()
{
    if (getState() != ARSpace::Playing)
        return;
    audioOutput->suspendPlay();
    setState(ARSpace::PlayPaused);
}

void ARPlayer::resume()
{
    if (getState() != ARSpace::PlayPaused)
        return;
    audioOutput->resumePlay();
    setState(ARSpace::Playing);
}

void ARPlayer::stop()
{
    audioOutput->stopPlay();
    sourcePtr->stop();
    setState(ARSpace::Stopped);
}
