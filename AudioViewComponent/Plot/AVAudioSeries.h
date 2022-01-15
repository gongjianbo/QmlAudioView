#pragma once
#include <QObject>
#include "AVAbstractSeries.h"
#include "Audio/AVDataSource.h"

/**
 * @brief 音频波形图绘制
 * @author 龚建波
 * @date 2022-01-15
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

    // 绘制
    void draw(QPainter *painter) override;

signals:
    void audioSourceChanged();

private slots:
    // 数据变更后重新抽样数据点并绘制
    void onDataChange();

private:
    AVDataSource *audioSource{nullptr};
};
