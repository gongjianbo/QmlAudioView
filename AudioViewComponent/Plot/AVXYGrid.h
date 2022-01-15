#pragma once
#include <QObject>
#include "AVAbstractSeries.h"

/**
 * @brief series区域底层的网格
 * @author 龚建波
 * @date 2022-01-14
 * @note 因为始终置于series区域，且同样关联xy axis，故继承自series
 * 同时，不同数据类型处理方式不一样，所以没有DataSource抽象类
 */
class AVXYGrid : public AVAbstractSeries
{
    Q_OBJECT
public:
    explicit AVXYGrid(QObject *parent = nullptr);

    // 绘制
    void draw(QPainter *painter) override;
};
