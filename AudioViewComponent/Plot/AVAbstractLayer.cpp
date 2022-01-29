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
    if (z != zOrder) {
        z = zOrder;
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

QFont AVAbstractLayer::getFont() const
{
    return font;
}

void AVAbstractLayer::setFont(const QFont ft)
{
    font = ft;
    fontEnable = true;
    emit fontChanged();
    emit layerChanged();
}

bool AVAbstractLayer::getFontEnable() const
{
    return fontEnable;
}

QColor AVAbstractLayer::getBgColor() const
{
    return bgColor;
}

void AVAbstractLayer::setBgColor(const QColor &color)
{
    if (bgColor != color) {
        bgColor = color;
        emit bgColorChanged();
        emit layerChanged();
    }
}

QColor AVAbstractLayer::getTextColor() const
{
    return textColor;
}

void AVAbstractLayer::setTextColor(const QColor &color)
{
    if (textColor != color) {
        textColor = color;
        emit textColorChanged();
        emit layerChanged();
    }
}

QColor AVAbstractLayer::getLineColor() const
{
    return lineColor;
}

void AVAbstractLayer::setLineColor(const QColor &color)
{
    if (lineColor != color) {
        lineColor = color;
        emit lineColorChanged();
        emit layerChanged();
    }
}

bool AVAbstractLayer::processEvent(QEvent *event, QPoint pos)
{
    //先遍历子节点
    for (AVAbstractLayer *layer : layers)
    {
        //processEvent返回true就不继续传递了
        if (layer->getAcceptEvent() &&
                layer->posInArea(pos) &&
                layer->processEvent(event, pos)) {
            return true;
        }
    }
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return mousePressEvent(static_cast<QMouseEvent*>(event));
    case QEvent::MouseMove:
        return mouseMoveEvent(static_cast<QMouseEvent*>(event));
    case QEvent::MouseButtonRelease:
        return mouseReleaseEvent(static_cast<QMouseEvent*>(event));
    case QEvent::MouseButtonDblClick:
        return mouseDoubleClickEvent(static_cast<QMouseEvent*>(event));
    case QEvent::Wheel:
        return wheelEvent(static_cast<QWheelEvent*>(event));
    case QEvent::HoverEnter:
        return hoverEnterEvent(static_cast<QHoverEvent*>(event));
    case QEvent::HoverMove:
        return hoverMoveEvent(static_cast<QHoverEvent*>(event));
    case QEvent::HoverLeave:
        return hoverLeaveEvent(static_cast<QHoverEvent*>(event));
    default:
        break;
    }
    return false;
}

bool AVAbstractLayer::mousePressEvent(QMouseEvent *)
{
    return false;
}

bool AVAbstractLayer::mouseMoveEvent(QMouseEvent *)
{
    return false;
}

bool AVAbstractLayer::mouseReleaseEvent(QMouseEvent *)
{
    return false;
}

bool AVAbstractLayer::mouseDoubleClickEvent(QMouseEvent *)
{
    return false;
}

bool AVAbstractLayer::wheelEvent(QWheelEvent *)
{
    return false;
}

bool AVAbstractLayer::hoverEnterEvent(QHoverEvent *)
{
    return false;
}

bool AVAbstractLayer::hoverMoveEvent(QHoverEvent *)
{
    return false;
}

bool AVAbstractLayer::hoverLeaveEvent(QHoverEvent *)
{
    return false;
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
