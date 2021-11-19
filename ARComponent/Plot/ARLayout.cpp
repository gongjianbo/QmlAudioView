#include "ARLayout.h"


ARLayout::ARLayout(QObject *parent)
    : QObject(parent)
{

}

void ARLayout::relayout(const QRect &viewRect)
{
    if (viewRect.isNull())
        return;

}
