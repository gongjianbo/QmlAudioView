#ifndef AUDIORECORDERVIEW_H
#define AUDIORECORDERVIEW_H

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "AudioRecorderDevice.h"

/**
 * @brief 录音可视化组件
 * @author 龚建波
 * @date 2021-1-12
 * @version v2.0
 * @details
 * 1.录制可参照Qt示例：audio和audioinput
 * 2.在操作录制、播放时会先调用stop，重置状态
 *
 * @note
 * 1.在初版逻辑的基础上引入线程进程来录制和播放
 * 2.没有border属性，可以在外层嵌套一层rect
 */
class AudioRecorderView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(AudioRecorderView::RecordState recordState READ getRecordState NOTIFY recordStateChanged)
    Q_PROPERTY(AudioRecorderView::DisplayMode displayMode READ getDisplayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(AudioRecorderDevice *deviceInfo READ getDeviceInfo CONSTANT)
    //目前这些属性不会触发交互，暂时用member，在初始化时设置
    Q_PROPERTY(int leftPadding MEMBER leftPadding)
    Q_PROPERTY(int rightPadding MEMBER rightPadding)
    Q_PROPERTY(int topPadding MEMBER topPadding)
    Q_PROPERTY(int bottomPadding MEMBER bottomPadding)
    Q_PROPERTY(int radius MEMBER radius)
    Q_PROPERTY(QColor backgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor viewColor MEMBER viewColor)
    Q_PROPERTY(QColor gridColor MEMBER gridColor)
    Q_PROPERTY(QColor seriesColor MEMBER seriesColor)
    Q_PROPERTY(QColor cursorColor MEMBER cursorColor)
    Q_PROPERTY(QColor axisColor MEMBER axisColor)
    Q_PROPERTY(QColor textColor MEMBER textColor)
public:
    //状态
    enum RecordState
    {
        Stop       //默认停止状态
        ,Playing   //播放
        ,PlayPause //播放暂停
        ,Record    //录制
    };
    Q_ENUMS(RecordState)
    //显示模式
    enum DisplayMode
    {
        FullRange  //绘制全部数据
        ,Tracking  //跟踪最新数据
    };
    Q_ENUMS(DisplayMode)
public:
    explicit AudioRecorderView(QQuickItem *parent = nullptr);
    ~AudioRecorderView();

    //录制状态
    AudioRecorderView::RecordState getRecordState() const { return recordState; }
    void setRecordState(AudioRecorderView::RecordState state);

    //绘制模式，目前仅对录制有效，播放时展示全谱
    AudioRecorderView::DisplayMode getDisplayMode() const { return displayMode; }
    void setDisplayMode(AudioRecorderView::DisplayMode mode);

    //设备信息
    AudioRecorderDevice *getDeviceInfo() { return &deviceInfo; }

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    //去掉padding的宽高
    int plotAreaWidth() const;
    int plotAreaHeight() const;
    //更新数据点抽样，绘制时根据抽样绘制
    void updateDataSample();
    //计算y周像素间隔
    void calculateSpace(double yAxisLen);
    double calculateSpaceHelper(double valueRefRange, int dividend) const;

signals:
    void recordStateChanged();
    void displayModeChanged();

public slots:
    //刷新，调用update
    void refresh();

private:
    //当前状态
    RecordState recordState=Stop;
    //绘制模式
    DisplayMode displayMode=Tracking;
    //刷新定时器
    QTimer updateTimer;
    //刷新间隔
    QElapsedTimer updateElapse;

    //设备信息
    AudioRecorderDevice deviceInfo;

    //四个边距
    //该版本刻度是一体的，所以刻度的宽高也算在padding里
    int leftPadding=60;
    int rightPadding=5;
    int topPadding=5;
    int bottomPadding=5;
    //圆角
    int radius=0;

    //颜色
    //背景色
    QColor backgroundColor=QColor("#0E306A");
    //图区域颜色
    QColor viewColor=QColor("#13275D");
    //网格颜色
    QColor gridColor=QColor(120,120,120);
    //曲线颜色
    QColor seriesColor=QColor("#083B95");
    //游标颜色
    QColor cursorColor=QColor("#FF385E");
    //刻度轴颜色
    QColor axisColor=QColor(200,200,200);
    //文本颜色
    QColor textColor=QColor(200,200,200);

    //计算刻度间隔
    double y1PxToValue=1;
    double y1ValueToPx=1;
    double yRefPxSpace=40;
    int yValueSpace=1000;

    //表示一个绘制用的抽样点信息
    struct SamplePoint
    {
        //目前是没有滚轮缩放的
        //暂时抽样时就把像素位置算好了
        int x; //像素坐标，相对于横轴0点
        int y; //像素坐标，相对于横轴0点
    };
    //绘制的抽样数据
    QList<SamplePoint> sampleData;
    //完整的音频数据
    QByteArray audioData;
};

#endif // AUDIORECORDERVIEW_H
