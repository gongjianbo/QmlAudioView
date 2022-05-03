#include "AVAudioThumbnail.h"
#include "AVValueAxis.h"
#include <QDebug>

AVAudioThumbnail::AVAudioThumbnail(QObject *parent)
    : AVAudioSeries(parent)
{
    acceptEvent = true;

    AVValueAxis *x_axis = new AVValueAxis(this);
    x_axis->setPosition(AVGlobal::PosBottom);
    setXAxis(x_axis);

    AVValueAxis *y_axis = new AVValueAxis(this);
    y_axis->setPosition(AVGlobal::PosLeft);
    y_axis->setLimitRange(-32768, 32767, 1000);
    y_axis->setValueRange(-32768, 32767);
    setYAxis(y_axis);
}

AVValueAxis *AVAudioThumbnail::getDisplayXAxis()
{
    return displayXAxis;
}

void AVAudioThumbnail::setDisplayXAxis(AVValueAxis *axis)
{
    if (displayXAxis != axis) {
        displayXAxis = axis;
        emit displayXAxisChanged();
        emit layerChanged();
    }
}

bool AVAudioThumbnail::wheelEvent(QWheelEvent *event)
{
    return false;
}

void AVAudioThumbnail::draw(QPainter *painter)
{
    painter->fillRect(rect, bgColor);
    if (!getXAxis() || !getYAxis() || !getAudioSource() ||
            getAudioSource()->isEmpty()) {
        drawBorder(painter);
        return;
    }
    //波形曲线
    AVAudioSeries::draw(painter);

    //遮罩
    if (displayXAxis) {
        double left = getRect().left();
        double min_x = left + getXAxis()->valueToPx(displayXAxis->getMinValue());
        double max_x = left + getXAxis()->valueToPx(displayXAxis->getMaxValue());
        // todo 导出遮罩相关属性
        painter->setPen(QColor(200, 200, 200));
        painter->setBrush(QColor(255, 255, 255, 60));
        QPainterPath mask_path;
        mask_path.addRoundedRect(min_x, rect.top(), max_x - min_x, rect.height(), 4, 4);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPath(mask_path);
        painter->setRenderHint(QPainter::Antialiasing, false);
    }
    drawBorder(painter);
}

void AVAudioThumbnail::geometryChanged(const QRect &newRect)
{
    if (yAxis) {
        yAxis->setRect(QRect(newRect.x(), newRect.y(),
                             30, newRect.height()));
    }
    if (xAxis) {
        xAxis->setRect(QRect(newRect.x(), newRect.y(),
                             newRect.width(), 30));
    }
    AVAudioSeries::geometryChanged(newRect);
}

void AVAudioThumbnail::drawBorder(QPainter *painter)
{
    //series方向一条边线
    painter->setPen(lineColor);
    if (this->getPosition() == AVGlobal::PosTop) {
        painter->drawLine(getRect().bottomLeft(), getRect().bottomRight());
    }
    else if (this->getPosition() == AVGlobal::PosBottom) {
        painter->drawLine(getRect().topLeft(), getRect().topRight());
    }
}
