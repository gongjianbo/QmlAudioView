#include "ARBaseView.h"

ARBaseView::ARBaseView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{

}

ARLayout *ARBaseView::getLayout()
{
    return layout;
}

bool ARBaseView::getAcceptClick() const
{
    return acceptClick;
}

void ARBaseView::setAcceptClick(bool accept)
{
    if (acceptClick != accept) {
        acceptClick = accept;
        setAcceptedMouseButtons(acceptClick ? (Qt::AllButtons) : (Qt::NoButton));
        emit acceptClickChanged();
    }
}

bool ARBaseView::getAcceptHover() const
{
    return acceptHover;
}

void ARBaseView::setAcceptHover(bool accept)
{
    if (acceptHover != accept) {
        acceptHover = accept;
        setAcceptHoverEvents(acceptHover);
    }
}

void ARBaseView::paint(QPainter *painter)
{
    //分为三层是为了处理，如多层叠加时浮动提示等情况
    for (ARAbstractLayer *layer : layerList) {
        layer->backDraw(painter);
    }
    for (ARAbstractLayer *layer : layerList) {
        layer->draw(painter);
    }
    for (ARAbstractLayer *layer : layerList) {
        layer->faceDraw(painter);
    }
}

void ARBaseView::classBegin()
{
    acceptClick = true;
    setAcceptedMouseButtons(Qt::AllButtons);
    acceptHover = true;
    setAcceptHoverEvents(true);

    layout = new ARLayout(this);
    connect(layout, &ARLayout::layoutChanged, this, &ARBaseView::refresh);
}

void ARBaseView::componentComplete()
{

}

void ARBaseView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    layout->relayout(boundingRect().toRect());
}

void ARBaseView::mousePressEvent(QMouseEvent *event)
{

}

void ARBaseView::mouseMoveEvent(QMouseEvent *event)
{

}

void ARBaseView::mouseReleaseEvent(QMouseEvent *event)
{

}

void ARBaseView::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void ARBaseView::refresh()
{
    update();
}
