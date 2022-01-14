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
