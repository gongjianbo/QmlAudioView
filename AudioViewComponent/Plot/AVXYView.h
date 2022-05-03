#pragma once
#include <QQuickPaintedItem>
#include <QFont>
#include "Common/AVGlobal.h"
#include "AVAbstractLayer.h"
#include "AVXYLayout.h"

/**
 * @brief 二维图表绘制区域
 * @author 龚建波
 * @date 2022-01-03
 */
class AVXYView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AVXYLayout* layout READ getLayout CONSTANT)
    Q_PROPERTY(QFont font READ getFont WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor NOTIFY bgColorChanged)
public:
    explicit AVXYView(QQuickItem *parent = nullptr);

    // 布局
    AVXYLayout *getLayout();

    // 字体
    QFont getFont() const;
    void setFont(const QFont ft);

    // 背景颜色 background color
    QColor getBgColor() const;
    void setBgColor(const QColor &color);

    // 绘制
    void paint(QPainter *painter) override;

protected:
    // qml组件初始化
    void classBegin() override;
    void componentComplete() override;
    // 窗口尺寸位置变化
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    // 事件处理
    bool event(QEvent *ev) override;

signals:
    void fontChanged();
    void bgColorChanged();

public slots:
    // 刷新
    void refresh();

private:
    // 布局计算
    AVXYLayout *layout;
    // layer节点，根据z值排序表，z值大的后绘制使之在表层
    QList<AVAbstractLayer*> layers;
    // 字体
    QFont font;
    // 背景色
    QColor bgColor{QColor(0, 0, 0)};
    // event.pos()
    QPoint eventPos;
};
