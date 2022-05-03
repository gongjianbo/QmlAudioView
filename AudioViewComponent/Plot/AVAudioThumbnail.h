#pragma once
#include "AVAudioSeries.h"

/**
 * @brief 音频波形缩略图
 * @author 龚建波
 * @date 2022-05-03
 */
class AVAudioThumbnail : public AVAudioSeries
{
    Q_OBJECT
    /// 关联series X轴，取范围
    Q_PROPERTY(AVValueAxis* displayXAxis READ getDisplayXAxis WRITE setDisplayXAxis NOTIFY displayXAxisChanged)
public:
    explicit AVAudioThumbnail(QObject *parent = nullptr);

    // 关联series X轴
    AVValueAxis *getDisplayXAxis();
    void setDisplayXAxis(AVValueAxis *axis);

    // 鼠标滚轮事件处理
    bool wheelEvent(QWheelEvent *event) override;
    // 绘制
    void draw(QPainter *painter) override;

protected:
    // 区域变化
    void geometryChanged(const QRect &newRect) override;

private:
    // 绘制边框
    void drawBorder(QPainter *painter);

signals:
    void displayXAxisChanged();

private:
    // series的x轴
    AVValueAxis *displayXAxis{nullptr};
};
