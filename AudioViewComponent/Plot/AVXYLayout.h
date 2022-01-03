#pragma once
#include <QObject>
#include "Common/AVGlobal.h"
#include "AVAbstractLayer.h"

/**
 * @brief 直角坐标系的布局计算
 * @author 龚建波
 * @date 2022-01-02
 * @details 目前的策略为：划分中心区域和四个边
 */
class AVXYLayout : public QObject
{
    Q_OBJECT
    /// 左边距
    Q_PROPERTY(int leftPadding READ getLeftPadding WRITE setLeftPadding NOTIFY leftPaddingChanged)
    /// 右边距
    Q_PROPERTY(int rightPadding READ getRightPadding WRITE setRightPadding NOTIFY rightPaddingChanged)
    /// 上边距
    Q_PROPERTY(int topPadding READ getTopPadding WRITE setTopPadding NOTIFY topPaddingChanged)
    /// 下边距
    Q_PROPERTY(int bottomPadding READ getBottomPadding WRITE setBottomPadding NOTIFY bottomPaddingChanged)
    /// 左刻度和中心间隔
    Q_PROPERTY(int leftSpacing READ getLeftSpacing WRITE setLeftSpacing NOTIFY leftSpacingChanged)
    /// 右刻度和中心间隔
    Q_PROPERTY(int rightSpacing READ getRightSpacing WRITE setRightSpacing NOTIFY rightSpacingChanged)
    /// 上刻度和中心间隔
    Q_PROPERTY(int topSpacing READ getTopSpacing WRITE setTopSpacing NOTIFY topSpacingChanged)
    /// 下刻度和中心间隔
    Q_PROPERTY(int bottomSpacing READ getBottomSpacing WRITE setBottomSpacing NOTIFY bottomSpacingChanged)
public:
    explicit AVXYLayout(QObject *parent = nullptr);

    int getLeftPadding() const;
    void setLeftPadding(int padding);
    int getRightPadding() const;
    void setRightPadding(int padding);
    int getTopPadding() const;
    void setTopPadding(int padding);
    int getBottomPadding() const;
    void setBottomPadding(int padding);

    int getLeftSpacing() const;
    void setLeftSpacing(int spacing);
    int getRightSpacing() const;
    void setRightSpacing(int spacing);
    int getTopSpacing() const;
    void setTopSpacing(int spacing);
    int getBottomSpacing() const;
    void setBottomSpacing(int spacing);

    // 添加layer到布局
    void addLayer(AVAbstractLayer *layer);

    // 计算布局
    void relayout(const QRect &viewRect);

protected:
    // 计算边界layer宽高，横向计算宽，竖向计算高
    int calcLayersSize(bool horizontal, int padding, int spacing,
                       const QList<AVAbstractLayer*> &layers) const;

signals:
    void layoutChanged();
    void relayoutRequest();

    void leftPaddingChanged();
    void rightPaddingChanged();
    void topPaddingChanged();
    void bottomPaddingChanged();

    void leftSpacingChanged();
    void rightSpacingChanged();
    void topSpacingChanged();
    void bottomSpacingChanged();

protected:
    // axis与view边框的间隔
    int leftPadding{0};
    int rightPadding{0};
    int topPadding{0};
    int bottomPadding{0};
    // axis与series的间隔
    int leftSpacing{0};
    int rightSpacing{0};
    int topSpacing{0};
    int bottomSpacing{0};
    // 组件
    QList<AVAbstractLayer*> leftLayers;
    QList<AVAbstractLayer*> rightLayers;
    QList<AVAbstractLayer*> topLayers;
    QList<AVAbstractLayer*> bottomLayers;
    QList<AVAbstractLayer*> centerLayers;
};

