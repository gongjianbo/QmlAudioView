#include "AVWrapper.h"
#include <QDebug>

AVWrapper::AVWrapper(QObject *parent)
    : AVAbstractLayer(parent)
{

}

AVGlobal::Direction AVWrapper::getDirection() const
{
    return direction;
}

void AVWrapper::setDirection(AVGlobal::Direction direct)
{
    if (direction != direct) {
        direction = direct;
        emit directionChanged();
        emit layerChanged();
    }
}

int AVWrapper::getSpacing() const
{
    return spacing;
}

void AVWrapper::setSpacing(int size)
{
    if (spacing != size) {
        spacing = size;
        emit spacingChanged();
        emit layerChanged();
    }
}

int AVWrapper::contentHeight() const
{
    if (layers.isEmpty()) {
        return implicitHeight;
    }

    int px = 0;
    //和Direction相同就累加，否则取最大者
    if (getDirection() == AVGlobal::TopToBottom)
    {
        int sub_size = 0;
        for (auto sub : layers)
        {
            if (sub->getVisible()) {
                sub_size ++;
                px += sub->contentHeight();
            }
        }
        if (sub_size > 0) {
            px += getSpacing() * (sub_size - 1);
        }
    }
    else
    {
        for (auto sub : layers)
        {
            if (sub->getVisible() && sub->contentHeight() > px) {
                px = sub->contentHeight();
            }
        }
    }
    return px;
}

int AVWrapper::contentWidth() const
{
    if (layers.isEmpty()) {
        return implicitWidth;
    }

    int px = 0;
    //和Direction相同就累加，否则取最大者
    if (getDirection() == AVGlobal::LeftToRight)
    {
        int sub_size = 0;
        for (auto sub : layers)
        {
            if (sub->getVisible()) {
                sub_size ++;
                px += sub->contentWidth();
            }
        }
        if (sub_size > 0) {
            px += getSpacing() * (sub_size - 1);
        }
    }
    else
    {
        for (auto sub : layers)
        {
            if (sub->getVisible() && sub->contentWidth() > px) {
                px = sub->contentWidth();
            }
        }
    }
    return px;
}

void AVWrapper::draw(QPainter *painter)
{
    //qDebug()<<rect;
    //painter->fillRect(rect, QColor(255, 255, 255, 50));
    for (AVAbstractLayer *layer : layers) {
        if (layer->getVisible()) layer->draw(painter);
    }
}

void AVWrapper::geometryChanged(const QRect &newRect)
{
    rect = newRect;
    int px = 0;
    for (auto sub : layers)
    {
        if (!sub->getVisible()) {
            continue;
        }
        if (getDirection() == AVGlobal::LeftToRight) {
            const int temp = sub->contentWidth();
            sub->setRect(QRect(newRect.x() + px, newRect.y(),
                               temp, newRect.height()));
            px += temp + getSpacing();
        }
        else {
            const int temp = sub->contentHeight();
            sub->setRect(QRect(newRect.x(), newRect.y() + px,
                               newRect.width(), temp));
            px += temp + getSpacing();
        }
    }
    emit layerChanged();
}
