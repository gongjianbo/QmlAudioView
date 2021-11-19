#include "ARAbstractAxis.h"

ARAbstractAxis::ARAbstractAxis(QObject *parent)
    : ARAbstractLayer(parent)
{
    z = 2;
    implicitSize = QSize(50, 30);
}
