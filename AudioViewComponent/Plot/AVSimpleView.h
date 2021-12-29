#pragma once
#include <QQuickPaintedItem>
#include "Common/AVGlobal.h"
#include "Common/AVWavDefine.h"
#include "Audio/AVDataSource.h"
#include "Audio/AVDataInput.h"
#include "Audio/AVDataOutput.h"
#include "Audio/AVDataBuffer.h"

//测试音频输入输出
//目前还没有写UI的逻辑，先用一个简单波形绘制测试逻辑
class AVSimpleView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString durationString READ getDurationString NOTIFY durationChanged)
    Q_PROPERTY(QString positionString READ getPositionString NOTIFY positionChanged)
public:
    explicit AVSimpleView(QQuickItem *parent = nullptr);
    ~AVSimpleView();

    QString getDurationString() const;
    QString getPositionString() const;

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void durationChanged();
    void positionChanged();

public slots:
    //录音
    void record();
    //播放
    void play();
    //暂停
    void suspendPlay();
    //恢复
    void resumePlay();
    //停止
    void stop();

private:
    //存放数据
    AVDataSource *dataSource{nullptr};
    //输入
    AVDataInput *dataInput{nullptr};
    //输出
    AVDataOutput *dataOutput{nullptr};
};
