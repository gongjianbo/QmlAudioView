#pragma once
#include <QObject>
#include "AVAbstractLayer.h"

/**
 * @brief 曲线等数据绘制的基类
 * @author 龚建波
 * @date 2022-01-02
 */
class AVAbstractSeries : public AVAbstractLayer
{
    Q_OBJECT
public:
    explicit AVAbstractSeries(QObject *parent = nullptr);

signals:

};
