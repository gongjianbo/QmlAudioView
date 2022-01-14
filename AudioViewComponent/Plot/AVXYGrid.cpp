#include "AVXYGrid.h"

AVXYGrid::AVXYGrid(QObject *parent)
    : AVAbstractLayer(parent)
{
    position = AVGlobal::PosCenter;
    z = -1;
    lineColor = QColor(120, 120, 120);
}

AVAbstractAxis *AVXYGrid::getXAxis()
{
    return xAxis;
}

void AVXYGrid::setXAxis(AVAbstractAxis *axis)
{
    if (xAxis != axis) {
        xAxis = axis;
        emit xAxisChanged();
        emit layerChanged();
    }
}

AVAbstractAxis *AVXYGrid::getYAxis()
{
    return yAxis;
}

void AVXYGrid::setYAxis(AVAbstractAxis *axis)
{
    if (yAxis != axis) {
        yAxis = axis;
        emit yAxisChanged();
        emit layerChanged();
    }
}

void AVXYGrid::draw(QPainter *painter)
{
    painter->fillRect(rect, bgColor);
    painter->setPen(QPen(lineColor, 1, Qt::DashLine));
    if (xAxis) {
        const int top_pos = rect.top();
        const int bottom_pos = rect.bottom();
        for(const int &offset : xAxis->getTickPos())
        {
            painter->drawLine(QPoint(offset, top_pos),
                              QPoint(offset, bottom_pos));
        }
    }
    if (yAxis) {
        const int left_pos = rect.left();
        const int right_pos = rect.right();
        for(const int &offset : yAxis->getTickPos())
        {
            painter->drawLine(QPoint(left_pos, offset),
                              QPoint(right_pos, offset));
        }
    }
}
