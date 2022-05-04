#pragma once
#include <QObject>
#include "AVAbstractSeries.h"
#include "AVValueAxis.h"
#include "Audio/AVDataSource.h"

/**
 * @brief 音频波形图绘制
 * @author 龚建波
 * @date 2022-01-15
 * @note
 * 目前xy axis坐标轴只和valueaxis类型适配
 * pinter播放位置指示器暂时放series，绘制多个series时就需要单独的指示器了
 */
class AVAudioSeries : public AVAbstractSeries
{
    Q_OBJECT
    /// 数据源
    Q_PROPERTY(AVDataSource* audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged)
    /// 播放指示器（红色竖线）值
    Q_PROPERTY(qint64 pointerFrame READ getPointerFrame WRITE setPointerFrame NOTIFY pointerFrameChanged)
    /// 播放指示器（红色竖线）颜色
    Q_PROPERTY(QColor pointerColor READ getPointerColor WRITE setPointerColor NOTIFY pointerColorChanged)
public:
    explicit AVAudioSeries(QObject *parent = nullptr);

    // 数据存放-使输入/输出/图表可共享数据
    AVDataSource *getAudioSource();
    void setAudioSource(AVDataSource *source);

    qint64 getPointerFrame() const;
    void setPointerFrame(qint64 frame);

    QColor getPointerColor() const;
    void setPointerColor(const QColor &color);

    // 鼠标滚轮事件处理
    bool wheelEvent(QWheelEvent *event) override;
    // 绘制
    void draw(QPainter *painter) override;

protected:
    // 设置axis
    void onSetXAxis(AVAbstractAxis *axis) override;
    void onSetYAxis(AVAbstractAxis *axis) override;
    // 区域变化
    void geometryChanged(const QRect &newRect) override;

signals:
    void audioSourceChanged();
    void pointerFrameChanged();
    void pointerColorChanged();

private slots:
    // 数据变更后坐标轴设置范围，重新抽样数据点并绘制
    void onDataChange();
    // 重新抽样数据点并绘制
    void calcSeries();
    // 数据抽样
    void calcSample();
    // 路径组装
    void calcPath();

private:
    // 坐标
    AVValueAxis *xValue{nullptr};
    AVValueAxis *yValue{nullptr};
    // 数据
    AVDataSource *audioSource{nullptr};
    // 存放抽样点的下标
    QList<int> sampleIndexs;
    // 抽样后的连线
    QPainterPath samplePath;

    // 指示器位置
    qint64 pointerFrame{0};
    // 指示器颜色
    QColor pointerColor{QColor(255, 0, 0)};
};
