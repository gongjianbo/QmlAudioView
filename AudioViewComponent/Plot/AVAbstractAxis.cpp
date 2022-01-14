#include "AVAbstractAxis.h"

AVAbstractAxis::AVAbstractAxis(QObject *parent)
    : AVAbstractLayer(parent)
{

}

QVector<int> AVAbstractAxis::getTickPos() const
{
    return tickPos;
}

QVector<QString> AVAbstractAxis::getTickLabel() const
{
    return tickLabel;
}
