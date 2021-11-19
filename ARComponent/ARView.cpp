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
        const ARSpace::WorkState old_state = workState;
        workState = state;
        emit workStateChanged(state);

        //录制完成后展示全谱
        if (old_state == ARSpace::Recording) {
            updateSampleData();
        }
        else if (state == ARSpace::Stopped) {
            //结束后更新状态并绘制
            updateSampleData();
        }
        refresh();
    }
}

void ARView::refresh()
{
    update();
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
    case ARSpace::Stopped: break;
    default: {
        stop();
    } break;
    }

    //状态在recorder里修改
    source.clear();
    QAudioFormat format = source.getFormat();
    format.setSampleRate(sampleRate);
    format.setSampleSize(sampleSize);
    format.setChannelCount(channelCount);
    source.setFormat(format);

    const QAudioDeviceInfo device_info = device.getInputInfo(deviceName);
    if (input->startRecord(device_info, format)) {
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
    input->suspendRecord();
    setWorkState(ARSpace::RecordPaused);
}

void ARView::resumeRecord()
{
    if (getWorkState() != ARSpace::RecordPaused)
        return;
    input->resumeRecord();
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
    case ARSpace::Stopped: break;
    default: {
        stop();
    } break;
    }

    if (source.isEmpty())
        return;
    const QAudioDeviceInfo device_info = device.getOutputInfo(deviceName);
    if (output->startPlay(device_info, source.getFormat())) {
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
    output->suspendPlay();
    setWorkState(ARSpace::PlayPaused);
}

void ARView::resumePlay()
{
    if (getWorkState() != ARSpace::PlayPaused)
        return;
    output->resumePlay();
    setWorkState(ARSpace::Playing);
}

void ARView::stop()
{
    input->stopRecord();
    output->stopPlay();
    setWorkState(ARSpace::Stopped);
}

bool ARView::saveToFile(const QString &filepath)
{
    //目前source和input/output还没直接关联
    return output->saveToFile(source.getData(), source.getFormat(), filepath);
}

bool ARView::loadFromFile(const QString &filepath)
{
    stop();
    QByteArray data;
    QAudioFormat format = source.getFormat();
    //目前source和input/output还没直接关联
    const bool ret = input->loadFromFile(data, format, filepath);
    if (ret) {
        source.clear();
        source.setFormat(format);
        source.setData(data);
    }
    return ret;
}

void ARView::paint(QPainter *painter)
{
    //series区域的宽高
    const int view_width = width() -20;
    const int view_height = height() -20;
    //series零点坐标
    const int wave_x = 10;
    const int wave_y = view_height / 2 + 10;

    //背景色
    painter->save();
    painter->setPen(Qt::NoPen);
    //painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setBrush(Qt::gray);
    painter->drawRoundedRect(0, 0, width(), height(), 4, 4);
    //painter->setRenderHint(QPainter::Antialiasing,false);
    painter->setBrush(Qt::NoBrush);
    painter->fillRect(10, 10, view_width, view_height, Qt::white);
    painter->restore();
    //文本字体
    //painter->setFont(textFont);

    //有数据时才绘制曲线
    if (!sampleData.isEmpty()) {
        //绘制波形
        painter->setPen(Qt::black);
        painter->translate(wave_x, wave_y);
        painter->drawLines(sampleData);
        painter->translate(-wave_x, -wave_y);

        //画游标
        painter->setPen(Qt::red);
        const int play_pos = double(output->getCurrentIndex()) / source.getData().count() * view_width + 10;
        painter->drawLine(play_pos, 10,
                          play_pos, height() - 10);
    }
}

void ARView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    updateSampleData();
    refresh();
}

void ARView::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (getWorkState() == ARSpace::Recording ||
            getWorkState() == ARSpace::RecordPaused)
        return;
    const QPoint pos = event->pos();
    //当前没有缩略
    const int posx = pos.x();
    if (!source.isEmpty() && posx >= 10 && posx <= width() - 10) {
        qint64 offset = double(posx - 10) / (width() - 20) *
                source.getSampleCount(false) * (source.getFormat().sampleSize() / 8);
        output->setCurrentOffset(offset);
    }
    refresh();
}

void ARView::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    refresh();
}

void ARView::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    refresh();
}

void ARView::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    refresh();
}

void ARView::init()
{
    input = new ARDataInput(&source, this);
    output = new ARDataOutput(&source, this);

    //数据变化，更新绘制
    connect(&source, &ARDataSource::dataChanged, this, [this] {
        updateSampleData();
        refresh();
    });

    connect(input, &ARDataInput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "input state changed" << state;
    });

    connect(output, &ARDataOutput::stateChanged,
            this, [this](QAudio::State state) {
        qDebug() << "output state changed" << state;
    });

    //播放进度
    connect(output, &ARDataOutput::currentIndexChanged, this, &ARView::refresh);

    //播放数据取完了就结束
    connect(output, &ARDataOutput::playFinished, this, &ARView::stop);
}

void ARView::free()
{
    stop();
}

void ARView::updateSampleData()
{
    sampleData.clear();

    //todo 数据抽样在series中进行

    const QByteArray audio_data = source.getData();
    const QAudioFormat audio_format = source.getFormat();
    //数据字节数
    const int data_count = audio_data.count();
    //采样率hz
    const int sample_rate = audio_format.sampleRate();
    //采样字节大小，这里没有判断为0否，设置时判断
    const int sample_byte = audio_format.sampleSize() / 8;
    //通道数，这里没有判断为0否，设置时判断
    const int channel_count = audio_format.channelCount();
    //数据有效性判断
    //不少于两个采样点；最后一个采样点完整；采样精度至少8位；通道至少1
    if (data_count < (sample_byte * channel_count * 2) ||
            data_count % (sample_byte * channel_count) != 0 ||
            sample_byte < 1 || channel_count < 1)
        return;

    //1s的采样数，乘上sample_byte为1s的字节数
    const qint64 seconds_points = sample_rate * channel_count;
    //数据总的点数（所有通道总和）
    const qint64 data_points = data_count / sample_byte;

    //取可见范围数据的偏移
    int point_offset = 0;
    //x轴内可以绘制的点数个数，不分通道数，计算时用步进跳过另一个通道
    double show_points = 0;
    //判断是否为录制状态
    const bool on_recording = (getWorkState() == ARSpace::Recording ||
                               getWorkState() == ARSpace::RecordPaused);

    if (on_recording) {
        //单通道时Ns的范围滚动
        //最大显示采样点数计算值=[N sample]*[N channel]*[sampleRate]
        const int range_points = 6 * seconds_points; //6s范围滚动
        /*if (recordPoints > range_points)
            point_offset = recordPoints - range_points;
        //从完整范围去掉offset部分就是要绘制的点
        show_points = recordPoints - point_offset;
        xTimeBegin = point_offset * 1000.0 / seconds_points;
        xTimeEnd = recordPoints * 1000.0 / seconds_points;*/
        if (data_points > range_points)
            point_offset = data_points - range_points;
        show_points = data_points - point_offset;
    }
    else {
        show_points = data_points;
        /*xTimeBegin = point_offset * 1000.0 / seconds_points;
        xTimeEnd = data_count / sample_byte * 1000.0 / seconds_points;*/
    }
    const int sample_count = data_points - point_offset;

    //+offset是为了只对show_points显示部分的数据绘制
    //暂时以16byte-1channel-16KHz参数进行测试
    const short* short_ptr = (const short*)audio_data.constData() + point_offset;
    //每一段内循环的步进
    const int sec_step = 1;
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step = std::ceil(sample_count / (double)width()) / 2 / sec_step * sec_step;
    if (x_step < sec_step)
        x_step = sec_step;
    else if (x_step > sample_count)
        x_step = sample_count;
    //坐标轴轴适应
    int leftPadding = 10;
    int rightPadding = 10;
    int topPadding = 10;
    int bottomPadding = 10;
    const double x_scale = (width() - leftPadding - rightPadding) / (double)show_points;
    //这里y轴因子已取反，绘制时无需取反
    //0x10000为short幅度范围
    const double y_scale = -(height() - topPadding - bottomPadding) / (double)0x10000;

    int cur_max = 0;
    int cur_min = 0;
    int index_max = 0;
    int index_min = 0;
    //分段找最大最小作为该段的抽样点
    //双声道时只取了左声道
    for (int i = 0; i < sample_count; i += x_step)
    {
        //根据采样点字节大小来选择类型转换
        cur_max = short_ptr[i];
        cur_min = short_ptr[i];
        index_max = i;
        index_min = i;
        for (int j = i; j < i + x_step && j < sample_count; j += sec_step)
        {
            //遍历找这一段的最大最小值
            if (cur_max < short_ptr[j]) {
                cur_max = short_ptr[j];
                index_max = j;
            }
            if (cur_min > short_ptr[j]) {
                cur_min = short_ptr[j];
                index_min = j;
            }
        }


        QPointF pt_min{ (index_min * x_scale), (cur_min * y_scale) };
        QPointF pt_max{ (index_max * x_scale), (cur_max * y_scale) };
        //根据先后顺序存最大最小，相等就存一个
        if (index_max > index_min) {
            sampleData << pt_min << pt_max;
        }
        else if (index_max < index_min) {
            sampleData << pt_max << pt_min;
        }
        else {
            sampleData << pt_min;
        }
    }
}

