#pragma once
#include "ARAbstractLayer.h"

/**
 * @brief 坐标轴绘制基类
 * @author 龚建波
 * @date 2020-04-16
 * @details
 * x轴刻度对应横向的grid网格线绘制
 * y轴刻度对应竖向的grid网格线绘制
 */
class ARAbstractAxis : public ARAbstractLayer
{
    Q_OBJECT
public:
    explicit ARAbstractAxis(QObject *parent = nullptr);
};

