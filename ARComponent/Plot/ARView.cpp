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

    //状态在recorder里修改
    //source.clear();
    QAudioFormat format = source.getFormat();
    format.setSampleRate(sampleRate);
    format.setSampleSize(sampleSize);
    format.setChannelCount(channelCount);
    //source.setFormat(format);

    const QAudioDeviceInfo device_info = device.getInputInfo(deviceName);
    recorder->record(device_info, format);

    setWorkState(ARSpace::Recording);
}

void ARView::suspendRecord()
{
    if (getWorkState() != ARSpace::Recording)
        return;
    recorder->suspend();
    setWorkState(ARSpace::RecordPaused);
}

void ARView::resumeRecord()
{
    if (getWorkState() != ARSpace::RecordPaused)
        return;
    recorder->resume();
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

    setWorkState(ARSpace::Playing);
}

void ARView::suspendPlay()
{
    if (getWorkState() != ARSpace::Playing)
        return;
    setWorkState(ARSpace::PlayPaused);
}

void ARView::resumePlay()
{
    if (getWorkState() != ARSpace::PlayPaused)
        return;
    setWorkState(ARSpace::Playing);
}

void ARView::stop()
{
    recorder->stop();
    player->stop();
    setWorkState(ARSpace::Stopped);
}

bool ARView::saveToFile(const QString &filepath)
{
    stop();

    if (source.isEmpty())
        return false;
    //qfile不能生成目录
    QFileInfo info(filepath);
    if (!info.dir().exists())
        info.dir().mkpath(info.absolutePath());

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QAudioFormat format = source.getFormat();
    QByteArray data = source.data();
    //暂时全部写入
    ARWavHead head = ARWavHead::createWavHead(
                format.sampleRate(),
                format.sampleSize(),
                format.channelCount(),
                data.size());
    file.write((const char*)(&head), sizeof(ARWavHead));
    file.write(data);
    file.close();
    return true;
}

bool ARView::loadFromFile(const QString &filepath)
{
    stop();

    return true;
}

void ARView::paint(QPainter *painter)
{
    painter->fillRect(0, 0, width(), height(), Qt::gray);
}

void ARView::init()
{
    player = new ARPlayer(&source, this);
    recorder = new ARRecorder(&source, this);
}

void ARView::free()
{
    stop();
}
