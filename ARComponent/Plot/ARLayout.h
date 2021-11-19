#pragma once
#include <QObject>
#include "ARAbstractLayer.h"
#include "ARDefine.h"

/**
 * @brief 管理view中的组件布局
 * @author 龚建波
 * @date 2020-04-16
 * @designer
 * 当前设计类似QMainWindow，一共三层
 * 前期最外层不做，先做曲线和坐标轴区域
 * ########### Title/Legend/Label Area ####
 * #......................................#
 * #...####### Axis Area ##############...#
 * #...#..............................#...#
 * #...#...... Series Area ...........#...#
 * #...#..............................#...#
 * #...################################...#
 * #......................................#
 * ########################################
 */
class ARLayout : public QObject
{
    Q_OBJECT
public:
    explicit ARLayout(QObject *parent = nullptr);

    /// 计算布局，在设置layer/改变geometry之后调用
    void relayout(const QRect &viewRect);

signals:
    /// 通知刷新
    void layoutChanged();

private:

    QList<ARAbstractLayer*> centerLayers;
};

