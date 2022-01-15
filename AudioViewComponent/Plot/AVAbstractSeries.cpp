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
        emit yAxisChanged();
        emit layerChanged();
    }
}
