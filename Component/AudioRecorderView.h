#ifndef AUDIORECORDERVIEW_H
#define AUDIORECORDERVIEW_H

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>

#include "AudioRecorderDevice.h"
#include "AudioRecorderOperate.h"

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
 * 3.操作时，先预置本地状态再通知线程执行对应操作
 *   线程返回的数据先判断当前状态是否符合，
 *   直接用锁还是用信号槽？先用信号槽完成功能
 */
class AudioRecorderView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(AudioRecorder::RecordState recordState READ getRecordState NOTIFY recordStateChanged)
    Q_PROPERTY(AudioRecorder::DisplayMode displayMode READ getDisplayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(AudioRecorderDevice *deviceInfo READ getDeviceInfo CONSTANT)
    Q_PROPERTY(QString cacheDir READ getCacheDir WRITE setCacheDir NOTIFY cacheDirChanged)
    Q_PROPERTY(qint64 duration READ getDuration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ getDurationString NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ getPosition NOTIFY positionChanged)
    Q_PROPERTY(QString positionString READ getPositionString NOTIFY positionChanged)
    Q_PROPERTY(bool hasData READ getHasData NOTIFY hasDataChanged)
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
    explicit AudioRecorderView(QQuickItem *parent = nullptr);
    ~AudioRecorderView();

    //录制状态
    AudioRecorder::RecordState getRecordState() const { return recordState; }
    void setRecordState(AudioRecorder::RecordState state);

    //绘制模式，目前仅对录制有效，播放时展示全谱
    AudioRecorder::DisplayMode getDisplayMode() const { return displayMode; }
    void setDisplayMode(AudioRecorder::DisplayMode mode);

    //设备信息
    AudioRecorderDevice *getDeviceInfo() { return &deviceInfo; }

    //缓存目录，配合不带路径的save函数
    QString getCacheDir() const { return cacheDir; }
    void setCacheDir(const QString &dir);

    //当前数据的总时长ms
    qint64 getDuration() const { return audioDuration; }
    void setDuration(qint64 duration);
    //将duration毫秒数转为时分秒格式
    QString getDurationString() const;

    //当前播放或者录制的时间ms
    qint64 getPosition() const { return audioPostion; }
    void setPosition(qint64 position);
    QString getPositionString() const;

    //当前是否有数据
    bool getHasData() const;
    void setHasData(bool has);

    //录制
    //sampleRate:输入采样率
    //device:输入设备名称，建议在deviceInfo设置，这里置空
    Q_INVOKABLE void record(int sampleRate,const QString &deviceName=QString());
    //停止录制/播放
    Q_INVOKABLE void stop();
    //播放
    //device:输入设备名称，建议在deviceInfo设置，这里置空
    Q_INVOKABLE void play(const QString &deviceName=QString());
    //暂停播放
    Q_INVOKABLE void suspendPlay();
    //暂停恢复
    Q_INVOKABLE void resumePlay();

    //从文件读取
    //目前不带解析器，只能解析44字节定长wav-pcm格式头
    //（与本组件生成的wav文件格式一致）
    Q_INVOKABLE void loadFromFile(const QString &filepath);

    //保存到文件
    Q_INVOKABLE void saveToFile(const QString &filepath);
    //保存到cache路径
    //（因为导入到音频库是以uuid为文件名，所以传入的文件名为uuid）
    //return 完整路径
    Q_INVOKABLE QString saveToCache(const QString &uuid);

protected:
    void paint(QPainter *painter) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    //初始化输入输出等
    void init();
    void free();
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
    void cacheDirChanged(const QString &dir);
    void durationChanged();
    void positionChanged();
    void hasDataChanged();
    //录制
    void requestRecord(const QAudioDeviceInfo &device, const QAudioFormat &format);
    //停止录制/播放
    void requestStop();
    //播放
    void requestPlay(const QAudioDeviceInfo &device);
    //暂停播放
    void requestSuspendPlay();
    //暂停恢复
    void requestResumePlay();
    //读写文件
    void requestLoadFile(const QString &filepath);
    void requestSaveFile(const QString &filepath);
    void loadFileFinished(bool result);
    void saveFileFinished(bool result);

public slots:
    //刷新，调用update
    void refresh();
    //添加数据
    void recvData(const QByteArray &data);

private:
    //当前状态
    AudioRecorder::RecordState recordState=AudioRecorder::Stop;
    //绘制模式
    AudioRecorder::DisplayMode displayMode=AudioRecorder::Tracking;
    //刷新定时器
    QTimer updateTimer;
    //刷新间隔
    QElapsedTimer updateElapse;

    //设备信息
    AudioRecorderDevice deviceInfo;
    //格式参数
    QAudioFormat audioFormat;
    //输入输出放到线程处理
    QThread *ioThread=nullptr;
    //输入输出操作管理，置于线程中
    AudioRecorderOperate *ioOperate=nullptr;

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
    //是否有数据
    bool hasData=false;
    //输出数据计数，对应read/write接口
    //qint64 outputCount=0;
    //播放数据计数，对应ui游标/audioOutput->processedUSecs()
    qint64 audioCursor=0;
    //数据时长ms
    qint64 audioDuration=0;
    //播放或者录制时长ms
    qint64 audioPostion=0;
    //临时数据缓存目录
    QString cacheDir;

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
    QColor seriesColor=QColor("#1F8FFF");
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
};

#endif // AUDIORECORDERVIEW_H
