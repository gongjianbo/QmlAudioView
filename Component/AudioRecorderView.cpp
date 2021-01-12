#include "AudioRecorderView.h"

#include <cmath>

#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QDebug>

AudioRecorderView::AudioRecorderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //刷新延时
    updateTimer.setSingleShot(true);
    connect(&updateTimer,&QTimer::timeout,this,[this]{
        update();
    });

    //不可见后stop
    connect(this,&AudioRecorderView::visibleChanged,this,[this]{
        if(!isVisible()){
            //stop();
        }
    });
}

AudioRecorderView::~AudioRecorderView()
{
    //stop();
}

void AudioRecorderView::setRecordState(AudioRecorderView::RecordState state)
{
    if(recordState!=state)
    {
        const RecordState old_state=getRecordState();
        recordState=state;
        //录制完成后展示全谱
        if(old_state==Record){
            //updateDataSample();
        }
        emit recordStateChanged();
        refresh();
    }
}

void AudioRecorderView::setDisplayMode(AudioRecorderView::DisplayMode mode)
{
    if(displayMode!=mode){
        displayMode=mode;
        emit displayModeChanged();
        refresh();
    }
}

void AudioRecorderView::paint(QPainter *painter)
{
    painter->fillRect(0,0,width(),height(),QColor(100,120,140));
}

void AudioRecorderView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry,oldGeometry);
    //updateDataSample();
    //calculateSpace(plotAreaHeight());
    refresh();
}

void AudioRecorderView::refresh()
{
    //大于xx ms立即刷新，否则定时器刷新
    if(updateElapse.elapsed()>30){
        updateTimer.stop();
        update();
        updateElapse.restart();
    }else{
        //未结束则重新start
        updateTimer.start(30);
    }
}
