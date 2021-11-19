#pragma once
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include "ARDefine.h"

/**
 * @brief view中的可视组件基类
 * @author 龚建波
 * @date 2020-04-16
 * @details
 * 绘制提供了三个接口，按绘制顺序为 backDraw/draw/faceDraw
 * 一般的绘制逻辑放到draw接口执行
 * 如果需要加背景就用backDraw（如axis对应的网格线）
 * 如果需要浮于上层就用faceDraw（如hover提示）
 */
class ARAbstractLayer : public QObject
{
    Q_OBJECT
    /// 组件在view位置与大小
    Q_PROPERTY(QRect rect READ getRect NOTIFY rectChanged)
    /// 组件在view停靠位置
    Q_PROPERTY(ARSpace::Position position READ getPosition WRITE setPosition NOTIFY positionChanged)
    /// 组件堆叠顺序，大的浮在最上层（即小的先绘制，大的后绘制可以遮盖）
    Q_PROPERTY(int z READ getZ WRITE setZ NOTIFY zChanged)
    /// 默认宽度
    Q_PROPERTY(int implicitWidth READ getImplicitWidth WRITE setImplicitWidth NOTIFY implicitWidthChanged)
    /// 默认高度
    Q_PROPERTY(int implicitHeight READ getImplicitHeight WRITE setImplicitHeight NOTIFY implicitHeightChanged)
    /// 是否接受事件
    Q_PROPERTY(bool acceptEvent READ getAcceptEvent WRITE setAcceptEvent NOTIFY acceptEventChanged)
    /// visible=true时是否可见
    Q_PROPERTY(bool visibility READ getVisibility WRITE setVisibility NOTIFY visibilityChanged)
    /// 当前是否可见，如有些按钮hover时才显示
    Q_PROPERTY(bool visible READ getVisible NOTIFY visibleChanged)
public:
    explicit ARAbstractLayer(QObject *parent = nullptr);

    QRect getRect() const;
    void setRect(const QRect &rect);

    ARSpace::Position getPosition() const;
    void setPosition(ARSpace::Position position);

    int getZ() const;
    void setZ(int z);

    int getImplicitWidth() const;
    void setImplicitWidth(int width);

    int getImplicitHeight() const;
    void setImplicitHeight(int height);

    bool getAcceptEvent() const;
    void setAcceptEvent(bool accept);

    bool getVisibility() const;
    void setVisibility(bool enable);

    bool getVisible() const;
    void setVisible(bool enable);

    /// 操作
    virtual bool mousePress(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseMove(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseRelease(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseDoubleClick(const QPoint &pos, Qt::KeyboardModifiers keymod);
    //virtual bool mouseWheelUp(const QPoint &pos, Qt::KeyboardModifiers keymod);
    //virtual bool mouseWheelDown(const QPoint &pos, Qt::KeyboardModifiers keymod);
    //virtual void mouseEnter(const QPoint &pos);
    //virtual void mouseLeave();

    /// 判断坐标在区域内
    virtual bool posInArea(const QPoint &pos) const;
    /// 三次绘制，以更好的支持多层叠加时的穿插绘制
    virtual void backDraw(QPainter *painter);
    virtual void draw(QPainter *painter);
    virtual void faceDraw(QPainter *painter);

protected:
    /// 位置/大小变化
    virtual void geometryChanged(const QRect &newRect);

signals:
    void layerChanged();
    void rectChanged();
    void positionChanged();
    void zChanged();
    void implicitWidthChanged();
    void implicitHeightChanged();
    void acceptEventChanged();
    void visibilityChanged();
    void visibleChanged();

protected:
    /// 组件在view位置与大小
    QRect rect;
    /// 组件在view停靠位置
    ARSpace::Position position{ ARSpace::PosNone };
    /// 组件堆叠顺序，大的浮在最上层（即小的先绘制，大的后绘制可以遮盖）
    int z{ 0 };
    /// 组件布局径向顺序，大的在外层（position为center的始终在中心区域）
    int r{ 0 };
    /// 布局参考尺寸
    QSize implicitSize{ 0, 0 };
    /// 布局边距，两个相邻的取最大者
    QMargins implicitMargins{ 0, 0, 0, 0 };
    /// 是否接受事件
    bool acceptEvent{ true };
    /// visible=true时是否可见
    bool visibility{ true };
    /// 当前是否可见，如有些按钮hover时才显示
    bool visible{ true };
};

