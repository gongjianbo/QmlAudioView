#include "ARAbstractSeries.h"

ARAbstractSeries::ARAbstractSeries(QObject *parent)
    : ARAbstractLayer(parent)
{
    z = 1;
    position = ARSpace::PosCenter;
}
