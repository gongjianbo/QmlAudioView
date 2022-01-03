#pragma once
#include <QObject>
#include <QQmlParserStatus>
#include <QQmlListProperty>
#include <QPainter>
#include <QPainterPath>
#include "Common/AVGlobal.h"

/**
 * @brief View中的可视部件基类
 * @author 龚建波
 * @date 2022-01-02
 * @details Axis\Series等继承于layer
 * classBegin() 从根节点开始，componentComplete()从最底层节点开始
 */
class AVAbstractLayer : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    /// 默认属性，子节点
    Q_PROPERTY(QQmlListProperty<QObject> children READ getChildren)
    /// 组件像素位置和大小
    Q_PROPERTY(QRect rect READ getRect WRITE setRect NOTIFY rectChanged)
    /// 组件停靠位置，围绕Position::PosCenter进行布局计算
    Q_PROPERTY(AVGlobal::Position position READ getPosition WRITE setPosition NOTIFY positionChanged)
    /// 图层堆叠顺序，值大的在表层，值小的在底层被上层遮盖
    Q_PROPERTY(int z READ getZ WRITE setZ NOTIFY zChanged)
    /// 参考宽度
    Q_PROPERTY(int implicitWidth READ getImplicitWidth WRITE setImplicitWidth NOTIFY implicitWidthChanged)
    /// 参考高度
    Q_PROPERTY(int implicitHeight READ getImplicitHeight WRITE setImplicitHeight NOTIFY implicitHeightChanged)
    /// 是否接受事件，acceptEvent=true则会根据Z顺序判断执行
    Q_PROPERTY(bool acceptEvent READ getAcceptEvent WRITE setAcceptEvent NOTIFY acceptEventChanged)
    /// 显示策略，visibility=true&&visible=true时才进行绘制
    Q_PROPERTY(bool visibility READ getVisibility WRITE setVisibility NOTIFY visibilityChanged)
    /// 当前是否可见，根据图表中的操作改变
    Q_PROPERTY(bool visible READ getVisible NOTIFY visibleChanged)
    /// 默认属性，子节点
    Q_CLASSINFO("DefaultProperty", "children")
public:
    explicit AVAbstractLayer(QObject *parent = nullptr);

    // 子节点
    QQmlListProperty<QObject> getChildren();

    QRect getRect() const;
    void setRect(const QRect &newRect);

    AVGlobal::Position getPosition() const;
    void setPosition(AVGlobal::Position pos);

    int getZ() const;
    void setZ(int zOrder);

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

    // 操作，return=true表示事件被接收，不再继续传递
    virtual bool mousePress(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseMove(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseRelease(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseDoubleClick(const QPoint &pos, Qt::KeyboardModifiers keymod);

    virtual bool mouseWheelUp(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual bool mouseWheelDown(const QPoint &pos, Qt::KeyboardModifiers keymod);
    virtual void mouseEnter(const QPoint &pos);
    virtual void mouseLeave();

    // 宽高
    virtual int contentHeight() const;
    virtual int contentWidth() const;
    // 判断坐标在区域内
    // 默认visible=false也要判断区域，如有些按钮hover时才显示
    virtual bool posInArea(const QPoint &pos, bool onlyVisible = false) const;
    // 绘制
    virtual void draw(QPainter *painter) = 0;

protected:
    // 初始化操作
    void classBegin() override;
    void componentComplete() override;
    // 对应QQuickItem::geometryChanged
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
    // 子节点
    QList<QObject*> children;
    QList<AVAbstractLayer*> layers;
    // 组件大小，根据width/height由布局计算后得到，用于绘制和交互判定
    QRect rect;
    // 组件位置
    AVGlobal::Position position{AVGlobal::PosNone};
    // z轴堆叠顺序，大的在上层
    int z{0};
    // 参考宽度
    int implicitWidth{0};
    // 参考高度
    int implicitHeight{0};
    // 默认不接受事件
    bool acceptEvent{false};
    // 默认可见
    bool visibility{true};
    bool visible{true};
};
