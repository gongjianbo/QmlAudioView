#pragma once
#include "ARAbstractLayer.h"

/**
 * @brief 波形绘制基类
 * @author 龚建波
 * @date 2020-04-16
 */
class ARAbstractSeries : public ARAbstractLayer
{
public:
    explicit ARAbstractSeries(QObject *parent = nullptr);
};

