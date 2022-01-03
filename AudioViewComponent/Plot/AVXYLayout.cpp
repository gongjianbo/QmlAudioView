#include "AVXYLayout.h"
#include <QDebug>

AVXYLayout::AVXYLayout(QObject *parent)
    : QObject(parent)
{

}

int AVXYLayout::getLeftPadding() const
{
    return leftPadding;
}

void AVXYLayout::setLeftPadding(int padding)
{
    if (leftPadding != padding) {
        leftPadding = padding;
        emit leftPaddingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getRightPadding() const
{
    return rightPadding;
}

void AVXYLayout::setRightPadding(int padding)
{
    if (rightPadding != padding) {
        rightPadding = padding;
        emit rightPaddingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getTopPadding() const
{
    return topPadding;
}

void AVXYLayout::setTopPadding(int padding)
{
    if (topPadding != padding) {
        topPadding = padding;
        emit topPaddingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getBottomPadding() const
{
    return bottomPadding;
}

void AVXYLayout::setBottomPadding(int padding)
{
    if (bottomPadding != padding) {
        bottomPadding = padding;
        emit bottomPaddingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getLeftSpacing() const
{
    return leftSpacing;
}

void AVXYLayout::setLeftSpacing(int spacing)
{
    if (leftSpacing != spacing) {
        leftSpacing = spacing;
        emit leftSpacingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getRightSpacing() const
{
    return rightSpacing;
}

void AVXYLayout::setRightSpacing(int spacing)
{
    if (rightSpacing != spacing) {
        rightSpacing = spacing;
        emit rightSpacingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getTopSpacing() const
{
    return topSpacing;
}

void AVXYLayout::setTopSpacing(int spacing)
{
    if (topSpacing != spacing) {
        topSpacing = spacing;
        emit topSpacingChanged();
        emit layoutChanged();
    }
}

int AVXYLayout::getBottomSpacing() const
{
    return bottomSpacing;
}

void AVXYLayout::setBottomSpacing(int spacing)
{
    if (bottomSpacing != spacing) {
        bottomSpacing = spacing;
        emit bottomSpacingChanged();
        emit layoutChanged();
    }
}

void AVXYLayout::addLayer(AVAbstractLayer *layer)
{
    if (!layer) {
        return;
    }
    QList<AVAbstractLayer*> *group_ptr;
    switch (layer->getPosition()) {
    case AVGlobal::PosLeft:
        group_ptr = &leftLayers;
        break;
    case AVGlobal::PosRight:
        group_ptr = &rightLayers;
        break;
    case AVGlobal::PosTop:
        group_ptr = &topLayers;
        break;
    case AVGlobal::PosBottom:
        group_ptr = &bottomLayers;
        break;
    case AVGlobal::PosCenter:
        group_ptr = &centerLayers;
        break;
    default:
        return;
    }
    //防止重复添加
    if (!group_ptr->contains(layer)) {
        group_ptr->push_back(layer);
        connect(layer, &AVAbstractLayer::visibleChanged, this, &AVXYLayout::relayoutRequest);
    }
}

void AVXYLayout::relayout(const QRect &viewRect)
{
    // 无效的范围，返回
    if (viewRect.isNull() || viewRect.isEmpty()) {
        return;
    }

    const int left_width = calcLayersSize(true, leftPadding, leftSpacing, leftLayers);
    const int right_width = calcLayersSize(true, rightPadding, rightSpacing, rightLayers);
    const int top_height = calcLayersSize(false, topPadding, topSpacing, topLayers);
    const int bottom_height = calcLayersSize(false, bottomPadding, bottomSpacing, bottomLayers);
    int center_minwidth = 50;
    int center_minheight = 50;
    if (viewRect.width() < left_width + right_width + center_minwidth ||
            viewRect.height() < top_height + bottom_height + center_minheight) {
        return;
    }
    QRect center_rect = QRect(0, 0, viewRect.width(), viewRect.height())
            .adjusted(left_width, top_height, -right_width, -bottom_height);
    QRect left_rect = QRect(leftPadding, center_rect.y(),
                            left_width - leftPadding - leftSpacing, center_rect.height());
    for (auto sub : leftLayers) {
        if (sub->getVisible()) sub->setRect(left_rect);
    }
    QRect right_rect = QRect(center_rect.x() + center_rect.width() + rightSpacing, center_rect.y(),
                             right_width - rightPadding - rightSpacing, center_rect.height());
    for (auto sub : rightLayers) {
        if (sub->getVisible()) sub->setRect(right_rect);
    }
    QRect top_rect = QRect(center_rect.x(), topPadding,
                           center_rect.width(), top_height - topPadding - topSpacing);
    for (auto sub : topLayers) {
        if (sub->getVisible()) sub->setRect(top_rect);
    }
    QRect bottom_rect = QRect(center_rect.x(), center_rect.y() + center_rect.height() + bottomSpacing,
                              center_rect.width(), bottom_height - bottomPadding - bottomSpacing);
    for (auto sub : bottomLayers) {
        if (sub->getVisible()) sub->setRect(bottom_rect);
    }
    for (auto sub : centerLayers) {
        if (sub->getVisible()) sub->setRect(center_rect);
    }
    emit layoutChanged();
    //qDebug()<<"layout"<<top_rect<<bottom_rect<<left_rect<<right_rect<<center_rect;
}

int AVXYLayout::calcLayersSize(bool horizontal, int padding, int spacing,
                               const QList<AVAbstractLayer *> &layers) const
{
    //qDebug()<<horizontal<<padding<<spacing<<layers.size();
    int px = padding;
    if (layers.size() == 1)
    {
        if (layers.first()->getVisible()) {
            px += spacing + (horizontal
                             ? layers.first()->contentWidth()
                             : layers.first()->contentHeight());
        }
    }
    else if (layers.size() > 1)
    {
        int max_px = 0;
        for (auto sub : layers)
        {
            if (sub->getVisible() && sub->contentWidth() > max_px) {
                max_px = (horizontal
                          ? sub->contentWidth()
                          : sub->contentHeight());
            }
        }
        if (max_px > 0) {
            px += spacing + max_px;
        }
    }
    return px;
}
