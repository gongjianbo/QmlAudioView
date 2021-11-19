#pragma once
#include <QQuickPaintedItem>
#include <QEvent>

#include "ARLayout.h"
#include "ARAbstractLayer.h"
#include "ARAbstractAxis.h"

/**
 * @brief 带坐标轴的绘制区域
 * @author 龚建波
 * @date 2020-04-16
 */
class ARBaseView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    //已有属性 fillColor 填充颜色
    /// 布局
    Q_PROPERTY(ARLayout *layout READ getLayout CONSTANT)
    /// 点击事件开关
    Q_PROPERTY(bool acceptClick READ getAcceptClick WRITE setAcceptClick NOTIFY acceptClickChanged)
    /// 悬停事件开关
    Q_PROPERTY(bool acceptHover READ getAcceptHover WRITE setAcceptHover NOTIFY acceptHoverChanged)
public:
    explicit ARBaseView(QQuickItem *parent = nullptr);

    ARLayout *getLayout();

    bool getAcceptClick() const;
    void setAcceptClick(bool accept);

    bool getAcceptHover() const;
    void setAcceptHover(bool accept);

    /// 绘制
    void paint(QPainter *painter) override;

protected:
    void classBegin() override;
    void componentComplete() override;
    //void childEvent(QChildEvent *event) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    //void hoverEnterEvent(QHoverEvent *event) override;
    //void hoverLeaveEvent(QHoverEvent *event) override;
    //void hoverMoveEvent(QHoverEvent *event) override;
    //void mouseOnMove(const QPoint &pos);
    //void wheelEvent(QWheelEvent *event) override;

signals:
    void acceptClickChanged();
    void acceptHoverChanged();

public slots:
    /// 刷新
    void refresh();

protected:
    /// 布局
    ARLayout *layout{ nullptr };
    /// 通过z值来排序，z值大的最后绘制
    QList<ARAbstractLayer*> layerList;
    /// 点击事件
    bool acceptClick{ true };
    /// 悬停事件
    bool acceptHover{ true };
};


