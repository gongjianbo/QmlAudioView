#include "AVXYView.h"
#include <QDebug>

AVXYView::AVXYView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , layout(new AVXYLayout(this))
{

}

void AVXYView::paint(QPainter *painter)
{
    painter->fillRect(boundingRect(), Qt::black);
    for (AVAbstractLayer *layer : layers) {
        layer->draw(painter);
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

void AVXYView::refresh()
{
    update();
}

