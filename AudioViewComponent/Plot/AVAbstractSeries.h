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
    Q_PROPERTY(AVAbstractAxis* xAxis READ getXAxis WRITE setXAxis NOTIFY xAxisChanged)
    Q_PROPERTY(AVAbstractAxis* yAxis READ getYAxis WRITE setYAxis NOTIFY yAxisChanged)
public:
    explicit AVAbstractSeries(QObject *parent = nullptr);

    // 关联的x轴
    AVAbstractAxis *getXAxis();
    void setXAxis(AVAbstractAxis *axis);

    // 关联的y轴
    AVAbstractAxis *getYAxis();
    void setYAxis(AVAbstractAxis *axis);

protected:
    virtual void onSetXAxis(AVAbstractAxis *axis);
    virtual void onSetYAxis(AVAbstractAxis *axis);

signals:
    void xAxisChanged();
    void yAxisChanged();

protected:
    AVAbstractAxis *xAxis{nullptr};
    AVAbstractAxis *yAxis{nullptr};
};
