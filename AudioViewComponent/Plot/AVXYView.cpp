#include "AVXYView.h"

#include <QDebug>

AVXYView::AVXYView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , layout(new AVXYLayout(this))
{
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(14);

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setKeepMouseGrab(true);
}

AVXYLayout *AVXYView::getLayout()
{
    return layout;
}

QFont AVXYView::getFont() const
{
    return font;
}

void AVXYView::setFont(const QFont ft)
{
    font = ft;
    emit fontChanged();
    refresh();
}

QColor AVXYView::getBgColor() const
{
    return bgColor;
}

void AVXYView::setBgColor(const QColor &color)
{
    if (bgColor != color) {
        bgColor = color;
        emit bgColorChanged();
        refresh();
    }
}

void AVXYView::paint(QPainter *painter)
{
    painter->setFont(getFont());
    painter->fillRect(boundingRect(), getBgColor());
    //遍历绘制每个layer
    for (AVAbstractLayer *layer : layers) {
        painter->save();
        //只有在该layer的字体修改后才生效
        if (layer->getFontEnable()) {
            painter->setFont(layer->getFont());
        }
        layer->draw(painter);
        painter->restore();
    }
}

void AVXYView::classBegin()
{
    connect(layout, &AVXYLayout::layoutChanged, this, &AVXYView::refresh);
    connect(layout, &AVXYLayout::relayoutRequest, this, [this] {
        layout->relayout(boundingRect().toRect());
    });
}

void AVXYView::componentComplete()
{
    for (QObject *child : this->children())
    {
        if (AVAbstractLayer *layer = qobject_cast<AVAbstractLayer *>(child)) {
            layout->addLayer(layer);
            layers.push_back(layer);

            connect(layer, &AVAbstractLayer::layerChanged, this, &AVXYView::refresh);
        }
    }
    //根据z值升序排列，这样z值大的就会绘制到表面
    std::sort(layers.begin(), layers.end(),
              [this] (AVAbstractLayer *left, AVAbstractLayer *right){
        return bool(left->getZ() < right->getZ());
    });
    layout->relayout(boundingRect().toRect());
}

void AVXYView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    layout->relayout(boundingRect().toRect());
}

bool AVXYView::event(QEvent *ev)
{
    switch (ev->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        eventPos = static_cast<QMouseEvent*>(ev)->pos();
        goto Event_Tag;
    case QEvent::Wheel:
        eventPos = static_cast<QWheelEvent*>(ev)->position().toPoint();
        goto Event_Tag;
    case QEvent::HoverEnter:
    case QEvent::HoverMove:
    case QEvent::HoverLeave:
        eventPos = static_cast<QHoverEvent*>(ev)->pos();
        goto Event_Tag;
    default:
        break;
    }
    return QQuickPaintedItem::event(ev);

    //前面cast后获取pos，用于判定位置
Event_Tag:
    ev->accept();
    for (AVAbstractLayer *layer : layers)
    {
        //processEvent返回true就不继续传递了
        if (layer->getAcceptEvent() &&
                layer->posInArea(eventPos) &&
                layer->processEvent(ev, eventPos)) {
            break;
        }
    }
    return true;
}

void AVXYView::refresh()
{
    update();
}
