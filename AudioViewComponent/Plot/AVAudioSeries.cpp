#include "AVAudioSeries.h"

#include <QDebug>

AVAudioSeries::AVAudioSeries(QObject *parent)
    : AVAbstractSeries(parent)
{
    acceptEvent = true;
}

AVDataSource *AVAudioSeries::getAudioSource()
{
    return audioSource;
}

void AVAudioSeries::setAudioSource(AVDataSource *source)
{
    if (audioSource == source) {
        return;
    }
    if (audioSource) {
        audioSource->disconnect(this);
    }
    audioSource = source;
    if (audioSource) {
        if (xValue) {
            qint64 point_count = audioSource->getSampleCount(true);
            xValue->setMaxLimit(point_count);
            xValue->setMaxValue(point_count);
        }
        connect(audioSource, &AVDataSource::dataChanged, this, &AVAudioSeries::onDataChange);
    }
    emit audioSourceChanged();
    calcSeries();
}

qint64 AVAudioSeries::getPointerFrame() const
{
    return pointerFrame;
}

void AVAudioSeries::setPointerFrame(qint64 frame)
{
    if (pointerFrame != frame) {
        pointerFrame = frame;
        emit pointerFrameChanged();
        emit layerChanged();
    }
}

QColor AVAudioSeries::getPointerColor() const
{
    return pointerColor;
}

void AVAudioSeries::setPointerColor(const QColor &color)
{
    if (pointerColor != color) {
        pointerColor = color;
        emit pointerColorChanged();
        emit layerChanged();
    }
}

bool AVAudioSeries::wheelEvent(QWheelEvent *event)
{
    return xValue ? xValue->wheelEvent(event) : false;
}

void AVAudioSeries::draw(QPainter *painter)
{
    if (!xValue || !yValue || !audioSource) {
        return;
    }

    //只绘制区域内
    painter->setClipRect(rect);
    painter->setPen(seriesColor);
    painter->drawPath(samplePath);

    if (!audioSource->isEmpty()) {
        //绘制播放进度指示器
        painter->setPen(pointerColor);
        double pointer_x = rect.left() + xValue->valueToPx(pointerFrame);
        painter->drawLine(pointer_x, rect.top(), pointer_x, rect.bottom());
    }
    painter->setClipping(false);
}

void AVAudioSeries::onSetXAxis(AVAbstractAxis *axis)
{
    if (xValue) {
        xValue->disconnect(this);
    }
    xValue = qobject_cast<AVValueAxis*>(axis);
    if (xValue) {
        qint64 point_count = 0;
        if (audioSource) {
            point_count = audioSource->getSampleCount(true);
        }
        xValue->setMaxLimit(point_count);
        xValue->setMaxValue(point_count);
        connect(xValue, &AVValueAxis::minValueChanged, this, &AVAudioSeries::calcSeries, Qt::QueuedConnection);
        connect(xValue, &AVValueAxis::maxValueChanged, this, &AVAudioSeries::calcSeries, Qt::QueuedConnection);
    }
}

void AVAudioSeries::onSetYAxis(AVAbstractAxis *axis)
{
    if (yValue) {
        yValue->disconnect(this);
    }
    yValue = qobject_cast<AVValueAxis*>(axis);
    if (yValue) {
        connect(yValue, &AVValueAxis::minValueChanged, this, &AVAudioSeries::calcSeries, Qt::QueuedConnection);
        connect(yValue, &AVValueAxis::maxValueChanged, this, &AVAudioSeries::calcSeries, Qt::QueuedConnection);
    }
}

void AVAudioSeries::geometryChanged(const QRect &newRect)
{
    calcSeries();
}

void AVAudioSeries::onDataChange()
{
    if (!xValue || !yValue || !audioSource) {
        return;
    }
    const qint64 point_count = audioSource->getSampleCount(true);
    //minlimit 和 minvalue 先预置为 0
    xValue->setMaxLimit(point_count);
    xValue->setMaxValue(point_count);
    calcSeries();
}

void AVAudioSeries::calcSeries()
{
    calcSample();
    calcPath();
    emit layerChanged();
}

void AVAudioSeries::calcSample()
{
    sampleIndexs.clear();
    if (!xValue || !yValue || !audioSource) {
        return;
    }
    const qint64 point_count = audioSource->getSampleCount(true);
    //先处理小于两个点的情况
    if (point_count <= 0) {
        return;
    } else if (point_count == 1) {
        sampleIndexs.push_back(0);
        return;
    }
    const qint64 min_xval = qRound64(xValue->getMinValue());
    const qint64 max_xval = qRound64(xValue->getMaxValue());
    const qint64 x_range = max_xval - min_xval;
    //目前仅处理16bit/单声道
    const std::vector<char> &audio_data = audioSource->getData();
    const short *data_ptr = (const short *)audio_data.data();
    const double w = rect.width();
    //const double h = rect.height();
    //除以2是因为太稀疏了，和audition看起来不一样
    qint64 x_step = std::ceil(x_range / w) / 2;
    if (x_step < 1) {
        x_step = 1;
    }
    else if (x_step > x_range) {
        x_step = x_range;
    }
    short cur_max = 0;
    short cur_min = 0;
    int index_max = 0;
    int index_min = 0;
    int first_x = 0;
    int last_x = 0;
    //头尾各增加一个，防止左右未贴边
    sampleIndexs.push_back(0);
    //频率轴min-max范围内的数据
    for (qint64 i = 1; i < point_count; i+= x_step)
    {
        if (i < min_xval) {
            first_x = i;
            continue;
        } else if (i > max_xval) {
            last_x = i;
            break;
        }
        cur_max = data_ptr[i];
        cur_min = data_ptr[i];
        index_max = i;
        index_min = i;
        for (qint64 j = i; j < i + x_step && j < point_count; j++)
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

        //根据先后顺序存最大最小，相等就存一个
        if (index_max < index_min)
        {
            sampleIndexs.push_back(index_max);
        }
        sampleIndexs.push_back(index_min);
        if (index_max > index_min)
        {
            sampleIndexs.push_back(index_max);
        }
    }
    //头尾各增加一个，防止左右未贴边
    if (first_x > 1) {
        sampleIndexs[0] = first_x;
    }
    if (last_x > 0 && last_x < point_count) {
        sampleIndexs.push_back(last_x);
    }
}

void AVAudioSeries::calcPath()
{
    samplePath = QPainterPath();
    if (!xValue || !yValue || !audioSource || sampleIndexs.isEmpty()) {
        return;
    }

    //const double x_range = xValue->getMaxValue() - xValue->getMinValue();
    //const double y_range = yValue->getMaxValue() - yValue->getMinValue();
    //坐标轴轴适应
    //const double x_scale = getRect().width() / (double)x_range;
    //const double y_scale = -getRect().height() / (double)y_range;
    //目前仅处理16bit/单声道
    const std::vector<char> &audio_data = audioSource->getData();
    const short *data_ptr = (const short *)audio_data.data();

    double left = getRect().left();
    double bottom = getRect().bottom();
    double x_calc = left + xValue->valueToPx(sampleIndexs.first());
    double y_calc = bottom - yValue->valueToPx(data_ptr[sampleIndexs.first()]);
    samplePath.moveTo(x_calc, y_calc);
    for (int i = 1; i < sampleIndexs.size(); i ++)
    {
        x_calc = left + xValue->valueToPx(sampleIndexs.at(i));
        y_calc = bottom - yValue->valueToPx(data_ptr[sampleIndexs.at(i)]);
        samplePath.lineTo(x_calc, y_calc);
    }
}

