#pragma once
#include <QObject>
#include "AVAbstractLayer.h"
#include "AVAbstractAxis.h"

/**
 * @brief series区域底层的网格
 * @author 龚建波
 * @date 2022-01-14
 */
class AVXYGrid : public AVAbstractLayer
{
    Q_OBJECT
    Q_PROPERTY(AVAbstractAxis* xAxis READ getXAxis WRITE setXAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AVAbstractAxis* yAxis READ getYAxis WRITE setYAxis NOTIFY yAxisChanged)
public:
    explicit AVXYGrid(QObject *parent = nullptr);

    // 关联的x轴，绘制时从该刻度轴取刻度线位置
    AVAbstractAxis *getXAxis();
    void setXAxis(AVAbstractAxis *axis);

    // 关联的y轴，绘制时从该刻度轴取刻度线位置
    AVAbstractAxis *getYAxis();
    void setYAxis(AVAbstractAxis *axis);

    // 绘制
    void draw(QPainter *painter) override;

signals:
    void xAxisChanged();
    void yAxisChanged();

private:
    AVAbstractAxis *xAxis{nullptr};
    AVAbstractAxis *yAxis{nullptr};
};
