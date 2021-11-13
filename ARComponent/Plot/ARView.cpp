#include "ARView.h"

#include <cmath>
#include <algorithm>

#include <QGuiApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QDebug>

ARView::ARView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setAcceptHoverEvents(true);

    init();
}

ARView::~ARView()
{
    free();
}

void ARView::setWorkState(ARSpace::WorkState state)
{
    if (workState != state) {
        workState = state;
        emit workStateChanged(state);
    }
}

void ARView::record(int sampleRate, int sampleSize,
                    int channelCount, const QString &deviceName)
{
    //暂停则继续
    //非停止状态则先停止
    switch (getWorkState()) {
    case ARSpace::Recording: return;
    case ARSpace::RecordPaused: {
        resumeRecord();
    } return;
    default: {
        stop();
    } break;
    }

    recorder->stopRecord();
    //状态在recorder里修改
    source.clear();
    QAudioFormat format = source.getFormat();
    format.setSampleRate(sampleRate);
    format.setSampleSize(sampleSize);
    format.setChannelCount(channelCount);
    source.setFormat(format);

    const QAudioDeviceInfo device_info = device.getInputInfo(deviceName);
    if (recorder->startRecord(source.buffer(), device_info, format)) {
        //切换为录制状态
        setWorkState(ARSpace::Recording);
    }
    else {
        qDebug() << "start record failed";
    }
}

void ARView::suspendRecord()
{
    if (getWorkState() != ARSpace::Recording)
        return;
    recorder->suspendRecord();
    setWorkState(ARSpace::RecordPaused);
}

void ARView::resumeRecord()
{
    if (getWorkState() != ARSpace::RecordPaused)
        return;
    recorder->resumeRecord();
    setWorkState(ARSpace::Recording);
}

void ARView::play(const QString &deviceName)
{
    //暂停则继续
    //非停止状态则先停止
    switch (getWorkState()) {
    case ARSpace::Playing: return;
    case ARSpace::PlayPaused: {
        resumePlay();
    } return;
    default: {
        stop();
    } break;
    }

    player->stopPlay();
    if (source.isEmpty())
        return;
    const QAudioDeviceInfo device_info = device.getOutputInfo(deviceName);
    if (player->startPlay(source.buffer(), device_info, source.getFormat())) {
        //切换为播放状态
        setWorkState(ARSpace::Playing);
    }
    else {
        qDebug() << "start play failed";
    }
}

void ARView::suspendPlay()
{
    if (getWorkState() != ARSpace::Playing)
        return;
    player->suspendPlay();
    setWorkState(ARSpace::PlayPaused);
}

void ARView::resumePlay()
{
    if (getWorkState() != ARSpace::PlayPaused)
        return;
    player->resumePlay();
    setWorkState(ARSpace::Playing);
}

void ARView::stop()
{
    recorder->stopRecord();
    player->stopPlay();
    source.stop();
    setWorkState(ARSpace::Stopped);
}

bool ARView::saveToFile(const QString &filepath)
{
    stop();
    //目前source和input/output还没直接关联
    return player->saveToFile(source.getData(), source.getFormat(), filepath);
}

bool ARView::loadFromFile(const QString &filepath)
{
    stop();
    QByteArray data;
    QAudioFormat format = source.getFormat();
    //目前source和input/output还没直接关联
    const bool ret = recorder->loadFromFile(data, format, filepath);
    if (ret) {
        source.clear();
        source.setFormat(format);
        source.setData(data);
    }
    return ret;
}

void ARView::paint(QPainter *painter)
{
    painter->fillRect(0, 0, width(), height(), Qt::gray);
}

void ARView::init()
{
    recorder = new ARDataInput(this);
    player = new ARDataOutput(this);

    //播放数据取完了就结束
    connect(&source, &ARDataSource::readFinished, this, &ARView::stop);

    connect(recorder, &ARDataInput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "input state changed" << state;
    });

    connect(player, &ARDataOutput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "output state changed" << state;
    });
}

void ARView::free()
{
    stop();
}
