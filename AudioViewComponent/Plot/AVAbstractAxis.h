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

signals:

};
