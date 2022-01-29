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
 * 目前xy axis只和valueaxis适配
 */
class AVAudioSeries : public AVAbstractSeries
{
    Q_OBJECT
    Q_PROPERTY(AVDataSource* audioSource READ getAudioSource WRITE setAudioSource NOTIFY audioSourceChanged)
public:
    explicit AVAudioSeries(QObject *parent = nullptr);

    // 数据存放-使输入/输出/图表可共享数据
    AVDataSource *getAudioSource();
    void setAudioSource(AVDataSource *source);

    // 事件处理
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
};
