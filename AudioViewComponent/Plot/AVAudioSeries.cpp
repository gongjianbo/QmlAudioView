#include "AVAudioSeries.h"

AVAudioSeries::AVAudioSeries(QObject *parent)
    : AVAbstractSeries(parent)
{

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
        connect(audioSource, &AVDataSource::dataChanged, this, &AVAudioSeries::onDataChange);
    }
    emit audioSourceChanged();
    onDataChange();
}

void AVAudioSeries::draw(QPainter *painter)
{
    if (!audioSource) {
        return;
    }
    // todo 测试绘制，抽样待完成
    const qreal w = rect.width();
    const qreal h = rect.height();
    if (w < 1 || h < 1) {
        return;
    }
    painter->setPen(lineColor);

    //目前测试16bit/单声道
    const std::vector<char> &audio_data = audioSource->getData();
    const qint64 sample_count = audioSource->size() / 2;
    const short *data_ptr = (const short *)audio_data.data();

    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step = std::ceil(sample_count / (double)rect.width()) / 2;
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
    painter->translate(rect.x(), rect.y() + h / 2);
    painter->drawPath(samplePath);
}

void AVAudioSeries::onDataChange()
{
    if (!xAxis || !yAxis || !audioSource) {
        return;
    }
    qint64 sample_count = audioSource->getSampleCount(true);
    xAxis->setMaxLimit(sample_count);
    xAxis->setMaxValue(sample_count);
    emit layerChanged();
}
