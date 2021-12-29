#include "AVSimpleView.h"
#include <cmath>
#include <QtMath>
#include <QTime>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QDebug>

AVSimpleView::AVSimpleView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //数据
    dataSource = new AVDataSource(this);
    connect(dataSource, &AVDataSource::dataChanged, this, [this](){
        update();
    });

    //输入
    dataInput = new AVDataInput(dataSource, this);
    connect(dataInput, &AVDataInput::durationChanged, this, [this](){
        emit durationChanged();
        update();
    });

    //输出
    dataOutput = new AVDataOutput(dataSource, this);
    connect(dataOutput, &AVDataOutput::positionChanged, this, [this](){
        emit positionChanged();
        update();
    });
}

AVSimpleView::~AVSimpleView()
{
    stop();
}

QString AVSimpleView::getDurationString() const
{
    return QTime(0, 0).addMSecs(dataInput->getDuration()).toString("hh:mm:ss");
}

QString AVSimpleView::getPositionString() const
{
    return QTime(0, 0).addMSecs(dataOutput->getPosition()).toString("hh:mm:ss");
}

void AVSimpleView::paint(QPainter *painter)
{
    const qreal w = width();
    const qreal h = height();
    if (w < 1 || h < 1) {
        return;
    }

    painter->fillRect(0, 0, w, h, QColor(30, 30, 30));
    painter->setPen(Qt::white);

    //目前测试16bit/单声道
    const std::vector<char> &audio_data = dataSource->getData();
    const qint64 sample_count = dataSource->size() / 2;
    const short *data_ptr = (const short *)audio_data.data();

    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step = std::ceil(sample_count / (double)width()) / 2;
    if (x_step < 1) {
        x_step = 1;
    }
    else if (x_step > sample_count) {
        x_step = sample_count;
    }
    //坐标轴轴适应
    const double x_scale = w / (double)sample_count;
    const double y_scale = -h / (double)0x10000;


    QPainterPath samplePath;
    samplePath.moveTo(0, 0);
    short cur_max = 0;
    short cur_min = 0;
    int index_max = 0;
    int index_min = 0;
    if (sample_count > 0) {
        samplePath.moveTo(0, data_ptr[0] * y_scale);
    }
    //分段找最大最小作为该段的抽样点
    for (int i = 0; i < sample_count; i += x_step)
    {
        cur_max = data_ptr[i];
        cur_min = data_ptr[i];
        index_max = i;
        index_min = i;
        for (int j = i; j < i + x_step && j < sample_count; j++)
        {
            //遍历找这一段的最大最小值
            if (cur_max < data_ptr[j])
            {
                cur_max = data_ptr[j];
                index_max = j;
            }
            if (cur_min > data_ptr[j])
            {
                cur_min = data_ptr[j];
                index_min = j;
            }
        }
        QPointF pt_min{index_min * x_scale, cur_min * y_scale};
        QPointF pt_max{index_max * x_scale, cur_max * y_scale};
        //根据先后顺序存最大最小，相等就存一个
        if (index_max < index_min)
        {
            samplePath.lineTo(pt_max);
        }
        samplePath.lineTo(pt_min);
        if (index_max > index_min)
        {
            samplePath.lineTo(pt_max);
        }
    }
    painter->translate(0, h/2);
    painter->drawPath(samplePath);

    painter->translate(0, -h/2);
    painter->setPen(Qt::red);

    const qint64 cur_index = dataOutput->getCurrentIndex();
    if(sample_count > 0)
        qDebug()<<OS_OBJECT(cur_index)<<OS_OBJECT(sample_count);
    const int cur_pos = cur_index/2.0/sample_count*w ;
    painter->drawLine(cur_pos, 0, cur_pos, h);
}

void AVSimpleView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void AVSimpleView::record()
{
    dataOutput->stopPlay();
    dataInput->startRecord(QAudioDeviceInfo::defaultInputDevice(), dataSource->getFormat());
}

void AVSimpleView::play()
{
    dataInput->stopRecord();
    dataOutput->startPlay(QAudioDeviceInfo::defaultInputDevice(), dataSource->getFormat());
}

void AVSimpleView::suspendPlay()
{
    dataOutput->suspendPlay();
}

void AVSimpleView::resumePlay()
{
    dataOutput->resumePlay();
}

void AVSimpleView::stop()
{
    dataInput->stopRecord();
    dataOutput->stopPlay();
}
