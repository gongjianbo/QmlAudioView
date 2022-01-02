#include "SimpleAudioRecorder.h"
#include <cmath>
#include <QtMath>
#include <QTime>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>

SimpleAudioDevice::SimpleAudioDevice(SimpleAudioRecorder *recorder, QObject *parent)
    : QIODevice(parent), recorderPtr(recorder)
{
    Q_ASSERT(recorderPtr != nullptr);
}

qint64 SimpleAudioDevice::readData(char *data, qint64 maxSize)
{
    return recorderPtr->readData(data, maxSize);
}

qint64 SimpleAudioDevice::writeData(const char *data, qint64 maxSize)
{
    return recorderPtr->writeData(data, maxSize);
}

SimpleAudioRecorder::SimpleAudioRecorder(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //作为QAudioInput/Output的构造参数，输入输出时回调write/write接口
    audioDevice = new SimpleAudioDevice(this, this);
    audioDevice->open(QIODevice::ReadWrite);

    //采样精度和声道数暂时默认
    audioFormat.setSampleRate(16000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
}

SimpleAudioRecorder::~SimpleAudioRecorder()
{
    stop();
    audioDevice->close();
}

SimpleAudioRecorder::RecorderState SimpleAudioRecorder::getWorkState() const
{
    return workState;
}

void SimpleAudioRecorder::setWorkState(RecorderState state)
{
    if (workState != state)
    {
        workState = state;
        emit workStateChanged();
    }
}

bool SimpleAudioRecorder::getHasData() const
{
    return hasData;
}

void SimpleAudioRecorder::setHasData(bool has)
{
    if (hasData != has)
    {
        hasData = has;
        emit hasDataChanged();
    }
}

qint64 SimpleAudioRecorder::getDuration() const
{
    return audioDuration;
}

void SimpleAudioRecorder::updateDuration()
{
    //根据音频数据的参数和数据长度进行计算
    const int sample_rate = audioFormat.sampleRate();
    const int sample_byte = audioFormat.sampleSize() / 8;
    const int channel_count = audioFormat.channelCount();
    qint64 duration = 0;
    if (audioData.size() > 0 && sample_rate > 0 && sample_byte > 0 && channel_count > 0) {
        //时长=采样总数/每秒的采样数
        //s time*1000=ms time
        duration = (audioData.size() / sample_byte) / (1.0 * channel_count * sample_rate) * 1000;
    }
    if (audioDuration != duration) {
        audioDuration = duration;
        emit durationChanged();
    }
}

QString SimpleAudioRecorder::getDurationString() const
{
    return QTime(0, 0).addMSecs(audioDuration).toString("hh:mm:ss");
}

qint64 SimpleAudioRecorder::getPosition() const
{
    return audioPostion;
}

void SimpleAudioRecorder::updatePosition()
{
    if (getWorkState() == Playing || getWorkState() == PlayPause)
    {
        const int sample_rate = audioFormat.sampleRate();
        audioPostion = ((playCount / 2) / (1.0 * sample_rate) * 1000);
    }
    else
    {
        //未播放时positon为0
        audioPostion = 0; // getDuration();
    }
    emit positionChanged();
}

QString SimpleAudioRecorder::getPositionString() const
{
    return QTime(0, 0).addMSecs(audioPostion).toString("hh:mm:ss");
}

qint64 SimpleAudioRecorder::readData(char *data, qint64 maxSize)
{
    if (!data || maxSize < 1)
        return 0;
    //如果是选区播放，可以将截至位置减去播放位置
    const int data_size = audioData.count() - outputCount;
    if (data_size <= 0)
    {
        //qDebug()<<__FUNCTION__<<"finish";
        /// stateChanged没有触发停止，但return 0 会触发 IdleState 状态
        /// 现在改为通过IdleState状态判断结束
        // 定时时间大于notifyInterval，使播放完整
        //const int sample_rate = audioFormat.sampleRate();
        // 时长=采样总数/每秒的采样数
        // s time*1000=ms time
        //qint64 duration = (audioOutput->bufferSize() / 2) / (1.0 * sample_rate) * 1000;
        // 这里播放结束时会进入多次，所以需要保存一个标志，使只定时一次
        //QTimer::singleShot(duration + 30, [this]{ stop(); });
        return 0;
    }

    const int read_size = (data_size >= maxSize) ? maxSize : data_size;
    memcpy(data, audioData.constData() + outputCount, read_size);
    outputCount += read_size;
    // refresh(); 这个回调间隔太大了，不适合用来刷新
    return read_size;
}

qint64 SimpleAudioRecorder::writeData(const char *data, qint64 maxSize)
{
    //默认为单声道，16bit
    audioData.append(data, maxSize);
    setHasData(!audioData.isEmpty());
    updateSamplePath();
    updateDuration();
    updatePosition();
    refresh(); //新的数据到来就刷新绘制
    return maxSize;
}

void SimpleAudioRecorder::paint(QPainter *painter)
{
    // series区域的宽高
    const int view_width = (width() - leftPadding - rightPadding);
    const int view_height = (height() - topPadding - bottomPadding);
    // series零点坐标
    const int wave_x = leftPadding;
    const int wave_y = view_height / 2 + topPadding;

    //背景色
    painter->setPen(Qt::NoPen);
    // painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setBrush(QColor(34, 34, 34));
    painter->drawRect(0, 0, width(), height());
    // painter->setRenderHint(QPainter::Antialiasing,false);
    painter->setBrush(Qt::NoBrush);
    painter->fillRect(leftPadding, rightPadding, view_width, view_height, QColor(10, 10, 10));

    //网格,等分的横线，中间为红色
    painter->translate(wave_x, wave_y);
    painter->setPen(QColor(200, 10, 10));
    painter->drawLine(0, 0, view_width, 0);
    int y_px = 0;
    painter->setPen(QColor(50, 50, 50));
    for (int i = 1; i <= 3; i++)
    {
        y_px = i * view_height / 2 / 3;
        painter->drawLine(0, y_px, view_width, y_px);
        painter->drawLine(0, -y_px, view_width, -y_px);
    }
    painter->translate(-wave_x, -wave_y);

    //有数据时才绘制曲线
    if (!audioData.isEmpty())
    {
        //绘制波形
        painter->setPen(QColor(67, 217, 150));
        painter->translate(wave_x, wave_y);
        painter->drawPath(samplePath);
        painter->translate(-wave_x, -wave_y);

        //画游标
        painter->setPen(QColor(200, 10, 10));
        const int play_pos = double(playCount) / audioData.count() * view_width + leftPadding + 1;
        painter->drawLine(play_pos, topPadding,
                          play_pos, height() - bottomPadding);
    }

    //纵轴幅度
    painter->translate(wave_x, wave_y);
    QString y_text;
    painter->setPen(QColor(200, 200, 200));
    painter->drawText(-5 - painter->fontMetrics().horizontalAdvance("0"),
                      painter->fontMetrics().height() / 2.5, "0");
    for (int i = 1; i <= 3; i++)
    {
        //取反是因为Qt屏幕坐标系是左上角为0，右下角正方向
        y_px = -i * view_height / 2 / 3;
        y_text = QString::number(i * 1200);
        painter->drawText(-5 - painter->fontMetrics().horizontalAdvance(y_text),
                          y_px + painter->fontMetrics().height() / 2.5,
                          y_text);
        y_text = QString::number(-i * 1200);
        painter->drawText(-5 - painter->fontMetrics().horizontalAdvance(y_text),
                          -y_px + painter->fontMetrics().height() / 2.5,
                          y_text);
    }
    painter->translate(-wave_x, -wave_y);
    painter->setPen(QColor(200, 200, 200));
    painter->drawLine(leftPadding, topPadding, leftPadding, topPadding + view_height);

    //横轴时间，略
    painter->drawLine(leftPadding, topPadding + view_height,
                      leftPadding + view_width, topPadding + view_height);
}

void SimpleAudioRecorder::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    updateSamplePath();
    refresh();
}

void SimpleAudioRecorder::updateSamplePath()
{
    samplePath = QPainterPath();

    const int data_count = audioData.count();
    if (data_count < 2 && data_count % 2 != 0)
        return;

    //根据模式切换显示的数据范围，暂时固定值
    // s*channel*sampleRate
    int data_show = data_count / 2;
    if (getWorkState() == Recording || getWorkState() == RecordPause)
    {
        const int max_show = 5 * 1 * audioFormat.sampleRate();
        if (data_show > max_show)
            data_show = max_show;
    }
    if (data_count < data_show * 2)
        return;
    const int sample_count = data_show;
    const short *data_ptr = (const short *)audioData.constData() + (data_count / 2 - data_show);
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step = std::ceil(sample_count / (double)width()) / 2;
    if (x_step < 1)
        x_step = 1;
    else if (x_step > sample_count)
        x_step = sample_count;
    //坐标轴轴适应
    const double x_scale = (width() - leftPadding - rightPadding) / (double)sample_count;
    //取反是因为Qt屏幕坐标系是左上角为0，右下角正方向
    const double y_scale = -(height() - topPadding - bottomPadding) / (double)0x10000;

    short cur_max = 0;
    short cur_min = 0;
    int index_max = 0;
    int index_min = 0;
    samplePath.moveTo(0, data_ptr[0] * y_scale);
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
}

void SimpleAudioRecorder::refresh()
{
    update();
}

void SimpleAudioRecorder::stop()
{
    //录制、播放时都会调用stop，所以把一些状态重置放这里
    //(停止的时候audioData的数据保留，在start时才清空)
    outputCount = 0;
    playCount = 0;
    switch (getWorkState())
    {
    case Stop:
        break;
    case Playing:
    case PlayPause:
        if (audioOutput) {
            audioOutput->stop();
        }
        break;
    case Recording:
    case RecordPause:
        if (audioInput) {
            audioInput->stop();
        }
        break;
    default:
        break;
    }
    setWorkState(Stop);
    updateSamplePath();
    updatePosition();
    refresh();
}

void SimpleAudioRecorder::play()
{
    //暂停继续
    if (getWorkState() == PlayPause)
    {
        playResume();
        return;
    }
    stop();

    if (audioData.isEmpty())
        return;

    if (!audioOutput)
    {
        //使用默认的输出设备，即系统当前设置的默认设备
        audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), audioFormat, this);
        connect(audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State state)
        {
            //没有音频数据可供处理时触发IdleState状态
            if (state == QAudio::IdleState) {
                stop();
            }
        });
        connect(audioOutput, &QAudioOutput::notify, this, [this]()
        {
            if (getDuration() > 0) {
                //用processedUSecs获取start到当前的us数，但是start后有点延迟
                //进度=已放时间和总时间之比*总字节数，注意时间单位
                playCount = (audioOutput->processedUSecs() / 1000.0) /
                        audioDuration * audioData.count();
                if (playCount > outputCount) {
                    playCount = outputCount;
                }
                //减temp_offset是为了补偿缓冲区还未播放的时差，音画同步
                int temp_offset = (audioOutput->bufferSize() - audioOutput->bytesFree());
                if (temp_offset < 0) {
                    temp_offset = 0;
                }
                playCount -= temp_offset;
                if (playCount < 0) {
                    playCount = 0;
                }
                updatePosition();
                refresh();
            }
        });
        //目前用notify来控制进度刷新
        audioOutput->setNotifyInterval(30);
    }
    //之前写audioOutput->reset()，多次播放会遇到notify卡顿
    audioDevice->reset();
    audioOutput->start(audioDevice);
    //切换为录制状态
    setWorkState(Playing);
}

void SimpleAudioRecorder::playPause()
{
    if (getWorkState() != Playing)
        return;
    if (audioOutput)
        audioOutput->suspend();
    setWorkState(PlayPause);
}

void SimpleAudioRecorder::playResume()
{
    if (getWorkState() != PlayPause)
        return;
    if (audioOutput)
        audioOutput->resume();
    setWorkState(Playing);
}

void SimpleAudioRecorder::record()
{
    //暂停继续
    if (getWorkState() == RecordPause)
    {
        recordResume();
        return;
    }
    stop();

    //录制时清空数据缓存
    audioData.clear();
    setHasData(false);
    samplePath = QPainterPath();

    if (!audioInput)
    {
        //使用默认的输入设备，即系统当前设置的默认设备
        audioInput = new QAudioInput(QAudioDeviceInfo::defaultInputDevice(), audioFormat, this);
        connect(audioInput, &QAudioInput::stateChanged, this, []() {});
        connect(audioInput, &QAudioInput::notify, this, []() {});
    }
    audioDevice->reset();
    audioInput->start(audioDevice);
    //切换为录制状态
    setWorkState(Recording);
}

void SimpleAudioRecorder::recordPause()
{
    if (getWorkState() != Recording)
        return;
    if (audioInput)
        audioInput->suspend();
    setWorkState(RecordPause);
}

void SimpleAudioRecorder::recordResume()
{
    if (getWorkState() != RecordPause)
        return;
    if (audioInput)
        audioInput->resume();
    setWorkState(Recording);
}

void SimpleAudioRecorder::saveFile(const QString &filepath)
{
    qDebug() << __FUNCTION__ << filepath;
    stop();

    if (audioData.isEmpty())
        return;
    // qfile不能生成目录
    QFileInfo info(filepath);
    if (!info.dir().exists())
        info.dir().mkpath(info.absolutePath());

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly))
    {
        //暂时全部写入
        AVWavHead head(audioFormat.sampleRate(), audioFormat.sampleSize(),
                       audioFormat.channelCount(), audioData.size());
        file.write((const char *)(&head), sizeof(AVWavHead));
        file.write(audioData);
        file.close();
    }
}

void SimpleAudioRecorder::loadFile(const QString &filepath)
{
    qDebug() << __FUNCTION__ << filepath;
    stop();

    //加载时清空数据缓存
    audioData.clear();
    setHasData(false);
    samplePath = QPainterPath();

    QFile file(filepath);
    if (file.exists() && file.size() > 44 &&
            file.open(QIODevice::ReadOnly))
    {
        AVWavHead head;
        file.read((char *)&head, 44);
        QByteArray pcm_data;
        if (head.isValid())
        {
            //暂时为全部读取
            pcm_data = file.readAll();
            file.close();
        }
        //采样率等置为相同参数
        if (pcm_data.count() > 0 && pcm_data.count() % 2 == 0 &&
                head.fmt.sampleRate == audioFormat.sampleRate() &&
                head.fmt.bitsPerSample == audioFormat.sampleSize() &&
                head.fmt.numChannels == audioFormat.channelCount())
        {
            audioData = pcm_data;
            setHasData(!audioData.isEmpty());
            updateSamplePath();
            updateDuration();
            updatePosition();
            refresh();
        }
    }
}
