#include "AVAbstractAxis.h"

AVAbstractAxis::AVAbstractAxis(QObject *parent)
    : AVAbstractLayer(parent)
{
    z = 1;
    implicitWidth = 55;
    implicitHeight = 30;
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
