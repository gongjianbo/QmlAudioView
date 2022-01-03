#include "AVAbstractLayer.h"
#include <QDebug>

AVAbstractLayer::AVAbstractLayer(QObject *parent)
    : QObject(parent)
{

}

QQmlListProperty<QObject> AVAbstractLayer::getChildren()
{
    return QQmlListProperty<QObject>(this, &children);
}

QRect AVAbstractLayer::getRect() const
{
    return rect;
}

void AVAbstractLayer::setRect(const QRect &newRect)
{
    if (rect != newRect) {
        rect = newRect;
        geometryChanged(rect);
        emit rectChanged();
        emit layerChanged();
    }
}

AVGlobal::Position AVAbstractLayer::getPosition() const
{
    return position;
}

void AVAbstractLayer::setPosition(AVGlobal::Position pos)
{
    if (position != pos) {
        position = pos;
        emit positionChanged();
        emit layerChanged();
    }
}

int AVAbstractLayer::getZ() const
{
    return z;
}

void AVAbstractLayer::setZ(int zOrder)
{
    if (z != z) {
        z = z;
        emit zChanged();
        emit layerChanged();
    }
}

int AVAbstractLayer::getImplicitWidth() const
{
    return implicitWidth;
}

void AVAbstractLayer::setImplicitWidth(int width)
{
    if (implicitWidth != width) {
        implicitWidth = width;
        emit implicitWidthChanged();
        emit layerChanged();
    }
}

int AVAbstractLayer::getImplicitHeight() const
{
    return implicitHeight;
}

void AVAbstractLayer::setImplicitHeight(int height)
{
    if (implicitHeight != height) {
        implicitHeight = height;
        emit implicitHeightChanged();
        emit layerChanged();
    }
}

bool AVAbstractLayer::getAcceptEvent() const
{
    return acceptEvent;
}

void AVAbstractLayer::setAcceptEvent(bool accept)
{
    if (acceptEvent != accept) {
        acceptEvent = accept;
        emit acceptEventChanged();
    }
}

bool AVAbstractLayer::getVisibility() const
{
    return visibility;
}

void AVAbstractLayer::setVisibility(bool enable)
{
    if (visibility != enable) {
        visibility = enable;
        emit visibilityChanged();
        emit visibleChanged();
        emit layerChanged();
    }
}

bool AVAbstractLayer::getVisible() const
{
    return bool(visibility && visible);
}

void AVAbstractLayer::setVisible(bool enable)
{
    if (visible != enable) {
        visible = enable;
        emit visibleChanged();
        emit layerChanged();
    }
}

bool AVAbstractLayer::mousePress(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool AVAbstractLayer::mouseMove(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool AVAbstractLayer::mouseRelease(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool AVAbstractLayer::mouseDoubleClick(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool AVAbstractLayer::mouseWheelUp(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool AVAbstractLayer::mouseWheelDown(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

void AVAbstractLayer::mouseEnter(const QPoint &)
{
    emit layerChanged();
}

void AVAbstractLayer::mouseLeave()
{
    emit layerChanged();
}

int AVAbstractLayer::contentHeight() const
{
    return implicitHeight;
}

int AVAbstractLayer::contentWidth() const
{
    return implicitWidth;
}

bool AVAbstractLayer::posInArea(const QPoint &pos, bool onlyVisible) const
{
    if (onlyVisible && !visible) {
        return false;
    }
    if (visibility) {
        return rect.contains(pos);
    }
    return false;
}

void AVAbstractLayer::classBegin()
{

}

void AVAbstractLayer::componentComplete()
{
    for (QObject *sub : children)
    {
        AVAbstractLayer *layer = qobject_cast<AVAbstractLayer*>(sub);
        if (layer) {
            layers.push_back(layer);
            connect(layer, &AVAbstractLayer::visibleChanged, this, &AVAbstractLayer::visibleChanged);
            connect(layer, &AVAbstractLayer::layerChanged, this, &AVAbstractLayer::layerChanged);
        }
    }

    //根据z值升序排列，这样z值大的就会绘制到表面
    std::sort(layers.begin(), layers.end(),
              [this] (AVAbstractLayer *left, AVAbstractLayer *right){
        return bool(left->getZ() < right->getZ());
    });
}

void AVAbstractLayer::geometryChanged(const QRect &)
{
    emit layerChanged();
}
