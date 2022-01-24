#include "AVAbstractAxis.h"

AVAbstractAxis::AVAbstractAxis(QObject *parent)
    : AVAbstractLayer(parent)
{
    z = 1;
    implicitWidth = 55;
    implicitHeight = 30;
}

double AVAbstractAxis::getMinLimit() const
{
    return 0.0;
}

void AVAbstractAxis::setMinLimit(double limit)
{
    Q_UNUSED(limit)
}

double AVAbstractAxis::getMaxLimit() const
{
    return 0.0;
}

void AVAbstractAxis::setMaxLimit(double limit)
{
    Q_UNUSED(limit)
}

double AVAbstractAxis::getMinRange() const
{
    return 0.0;
}

void AVAbstractAxis::setMinRange(double limit)
{
    Q_UNUSED(limit)
}

double AVAbstractAxis::getMinValue() const
{
    return 0.0;
}

void AVAbstractAxis::setMinValue(double value)
{
    Q_UNUSED(value)
}

double AVAbstractAxis::getMaxValue() const
{
    return 0.0;
}

void AVAbstractAxis::setMaxValue(double value)
{
    Q_UNUSED(value)
}

double AVAbstractAxis::pxToValue(double px) const
{
    Q_UNUSED(px)
    return 0.0;
}

double AVAbstractAxis::valueToPx(double value) const
{
    Q_UNUSED(value)
    return 0.0;
}

QVector<int> AVAbstractAxis::getTickPos() const
{
    return tickPos;
}

QVector<QString> AVAbstractAxis::getTickLabel() const
{
    return tickLabel;
}
