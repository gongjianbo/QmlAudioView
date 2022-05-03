#include "AVAbstractSeries.h"

AVAbstractSeries::AVAbstractSeries(QObject *parent)
    : AVAbstractLayer(parent)
{
    z = 1;
    position = AVGlobal::PosCenter;
}

AVAbstractAxis *AVAbstractSeries::getXAxis()
{
    return xAxis;
}

void AVAbstractSeries::setXAxis(AVAbstractAxis *axis)
{
    if (xAxis != axis) {
        xAxis = axis;
        onSetXAxis(axis);
        emit xAxisChanged();
        emit layerChanged();
    }
}

AVAbstractAxis *AVAbstractSeries::getYAxis()
{
    return yAxis;
}

void AVAbstractSeries::setYAxis(AVAbstractAxis *axis)
{
    if (yAxis != axis) {
        yAxis = axis;
        onSetYAxis(axis);
        emit yAxisChanged();
        emit layerChanged();
    }
}

QColor AVAbstractSeries::getSeriesColor() const
{
    return seriesColor;
}

void AVAbstractSeries::setSeriesColor(const QColor &color)
{
    if (seriesColor != color) {
        seriesColor = color;
        emit seriesColorChanged();
        emit layerChanged();
    }
}

void AVAbstractSeries::onSetXAxis(AVAbstractAxis *axis)
{
    Q_UNUSED(axis)
}

void AVAbstractSeries::onSetYAxis(AVAbstractAxis *axis)
{
    Q_UNUSED(axis)
}
