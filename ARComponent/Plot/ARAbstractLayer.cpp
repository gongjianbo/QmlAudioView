#include "ARAbstractLayer.h"

ARAbstractLayer::ARAbstractLayer(QObject *parent)
    : QObject(parent)
{

}

QRect ARAbstractLayer::getRect() const
{
    return rect;
}

void ARAbstractLayer::setRect(const QRect &rectArg)
{
    rect = rectArg;
    geometryChanged(rectArg);
    emit rectChanged();
    emit layerChanged();
}

ARSpace::Position ARAbstractLayer::getPosition() const
{
    return position;
}

void ARAbstractLayer::setPosition(ARSpace::Position positionArg)
{
    if (position != positionArg) {
        position = positionArg;
        emit positionChanged();
    }
}

int ARAbstractLayer::getZ() const
{
    return z;
}

void ARAbstractLayer::setZ(int zArg)
{
    if (z != zArg) {
        z = zArg;
        emit zChanged();
    }
}

int ARAbstractLayer::getImplicitWidth() const
{
    return implicitSize.width();
}

void ARAbstractLayer::setImplicitWidth(int width)
{
    if (implicitSize.width() != width && width > 0) {
        implicitSize.setWidth(width);
        emit implicitWidthChanged();
    }
}

int ARAbstractLayer::getImplicitHeight() const
{
    return implicitSize.height();
}

void ARAbstractLayer::setImplicitHeight(int height)
{
    if (implicitSize.height() != height && height > 0) {
        implicitSize.setHeight(height);
        emit implicitHeightChanged();
    }
}

bool ARAbstractLayer::getAcceptEvent() const
{
    return acceptEvent;
}

void ARAbstractLayer::setAcceptEvent(bool accept)
{
    if (acceptEvent != accept) {
        acceptEvent = accept;
        emit acceptEventChanged();
    }
}

bool ARAbstractLayer::getVisibility() const
{
    return visibility;
}

void ARAbstractLayer::setVisibility(bool enable)
{
    if (visibility != enable) {
        visibility = enable;
        emit visibilityChanged();
        emit layerChanged();

        if (!enable) {
            setVisible(false);
        }
    }
}

bool ARAbstractLayer::getVisible() const
{
    return bool(visibility && visible);
}

void ARAbstractLayer::setVisible(bool enable)
{
    if (visible != enable) {
        visible = enable;
        emit visibleChanged();
        emit layerChanged();
    }
}

bool ARAbstractLayer::mousePress(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool ARAbstractLayer::mouseMove(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool ARAbstractLayer::mouseRelease(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool ARAbstractLayer::mouseDoubleClick(const QPoint &, Qt::KeyboardModifiers)
{
    return false;
}

bool ARAbstractLayer::posInArea(const QPoint &pos) const
{
    if (visibility) {
        return rect.contains(pos);
    }
    return false;
}

void ARAbstractLayer::backDraw(QPainter *)
{

}

void ARAbstractLayer::draw(QPainter *)
{

}

void ARAbstractLayer::faceDraw(QPainter *)
{

}

void ARAbstractLayer::geometryChanged(const QRect &)
{
    emit layerChanged();
}
