#pragma once
#include <QObject>
#include "AVAbstractLayer.h"
#include "AVAbstractAxis.h"

/**
 * @brief 曲线等数据绘制的基类
 * @author 龚建波
 * @date 2022-01-02
 */
class AVAbstractSeries : public AVAbstractLayer
{
    Q_OBJECT
    /// 坐标轴
    Q_PROPERTY(AVAbstractAxis* xAxis READ getXAxis WRITE setXAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AVAbstractAxis* yAxis READ getYAxis WRITE setYAxis NOTIFY yAxisChanged)
    /// 波形曲线颜色
    Q_PROPERTY(QColor seriesColor READ getSeriesColor WRITE setSeriesColor NOTIFY seriesColorChanged)
public:
    explicit AVAbstractSeries(QObject *parent = nullptr);

    // 关联的x轴
    AVAbstractAxis *getXAxis();
    void setXAxis(AVAbstractAxis *axis);

    // 关联的y轴
    AVAbstractAxis *getYAxis();
    void setYAxis(AVAbstractAxis *axis);

    QColor getSeriesColor() const;
    void setSeriesColor(const QColor &color);

protected:
    virtual void onSetXAxis(AVAbstractAxis *axis);
    virtual void onSetYAxis(AVAbstractAxis *axis);

signals:
    void xAxisChanged();
    void yAxisChanged();
    void seriesColorChanged();

protected:
    // 坐标轴
    AVAbstractAxis *xAxis{nullptr};
    AVAbstractAxis *yAxis{nullptr};

    // 曲线颜色
    QColor seriesColor{QColor(0, 255, 170)};
};
