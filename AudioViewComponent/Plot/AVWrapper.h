#pragma once
#include "Common/AVGlobal.h"
#include "AVAbstractLayer.h"

/**
 * @brief 类似QBoxLayout的布局，用于并列展示多个layer
 * @author 龚建波
 * @date 2022-01-03
 */
class AVWrapper : public AVAbstractLayer
{
    Q_OBJECT
    /// 方向，对应坐标系xy轴
    Q_PROPERTY(AVGlobal::Direction direction READ getDirection WRITE setDirection NOTIFY directionChanged)
    /// 间隔
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing NOTIFY spacingChanged)
public:
    explicit AVWrapper(QObject *parent = nullptr);

    AVGlobal::Direction getDirection() const;
    void setDirection(AVGlobal::Direction direct);

    int getSpacing() const;
    void setSpacing(int size);

    // 宽高
    int contentHeight() const override;
    int contentWidth() const override;
    // 绘制
    void draw(QPainter *painter) override;

protected:
    // 对应QQuickItem::geometryChanged
    void geometryChanged(const QRect &newRect) override;

signals:
    void directionChanged();
    void spacingChanged();

private:
    // 布局方向
    AVGlobal::Direction direction{AVGlobal::LeftToRight};
    // 间隔
    int spacing{0};
};
