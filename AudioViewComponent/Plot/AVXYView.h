#pragma once
#include <QQuickPaintedItem>
#include <QEvent>
#include "Common/AVGlobal.h"
#include "AVAbstractLayer.h"
#include "AVXYLayout.h"

/**
 * @brief 直角坐标系的图表绘制区域
 * @author 龚建波
 * @date 2022-01-03
 */
class AVXYView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AVXYLayout* layout MEMBER layout CONSTANT)
public:
    explicit AVXYView(QQuickItem *parent = nullptr);

    // 绘制
    void paint(QPainter *painter) override;

protected:
    void classBegin() override;
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:

public slots:
    // 刷新
    void refresh();

private:
    // 布局计算
    AVXYLayout *layout;
    // layer节点，根据z值排序表，z值大的后绘制使之在表层
    QList<AVAbstractLayer*> layers;
};


