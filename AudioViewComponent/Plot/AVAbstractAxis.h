#pragma once
#include <QObject>
#include "AVAbstractLayer.h"

/**
 * @brief 坐标轴的基类
 * @author 龚建波
 * @date 2022-01-02
 */
class AVAbstractAxis : public AVAbstractLayer
{
    Q_OBJECT
public:
    explicit AVAbstractAxis(QObject *parent = nullptr);

    // 最小值限制
    virtual double getMinLimit() const;
    virtual void setMinLimit(double limit);

    // 最大值限制
    virtual double getMaxLimit() const;
    virtual void setMaxLimit(double limit);

    // 最小范围限制
    virtual double getMinRange() const;
    virtual void setMinRange(double limit);

    // 当前显示的最小刻度
    virtual double getMinValue() const;
    virtual void setMinValue(double value);

    // 当前显示的最大刻度
    virtual double getMaxValue() const;
    virtual void setMaxValue(double value);

    /**
    * @brief 坐标轴像素值转数值
    * @param px 鼠标pos
    * @return 对应的刻度数值
    */
    virtual double pxToValue(double px) const;

    /**
    * @brief 数值转坐标轴像素值
    * @param value 对应的刻度数值
    * @return 鼠标所在point对应的px长度
    */
    virtual double valueToPx(double value) const;

    // 刻度位置
    QVector<int> getTickPos() const;
    // 刻度信息
    QVector<QString> getTickLabel() const;

protected:
    // 刻度像素位置
    QVector<int> tickPos;
    // 刻度值文本
    QVector<QString> tickLabel;
};
