#pragma once
#include <QQuickPaintedItem>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>

#include "ARDevice.h"
#include "ARDataSource.h"
#include "ARDataInput.h"
#include "ARDataOutput.h"

/**
 * @brief 录音可视化组件
 * @author 龚建波
 * @date 2021-11-12
 * @version v3.0
 * @details
 * 1.在v2.0版本实验了将输入输出放到线程，但绘制还是会卡顿，反而增加了复杂度，故取消线程
 * 主要问题还是在于数据抽样以及刷新绘制上，尽量保持匀速刷新
 * （在Qt源码中本身数据的读写就是在线程中进行的）
 * 2.最开始没有编辑的需求，所以波形图直接一个paintEvent中就抽样并绘制了
 * 但是后来逐渐增加了波形界面编辑的需求，所以需要对波形部分进行重构
 * 3.目前基于Qt5开发，QByteArray的容量受限于int的上限大约2GB
 * 考虑到不会有这么长的录制数据，只能是不自定义数据容器，在一定大小时自动停止录制
 * 4.之前只要是录制指定参数格式（采样率16K-精度16位-单声道的pcm-wav）
 * 后面依旧只支持wav格式，但是对采样率-精度-声道做更多的拓展设计
 */
class ARView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(ARDevice *device READ getDevice CONSTANT)
    Q_PROPERTY(ARDataSource *source READ getSource CONSTANT)
    Q_PROPERTY(ARSpace::WorkState workState READ getWorkState NOTIFY workStateChanged)
public:
    explicit ARView(QQuickItem* parent = nullptr);
    ~ARView();

    /// 输入输出设备管理
    ARDevice *getDevice() { return &device; }
    /// 音频数据管理
    ARDataSource *getSource() { return &source; }
    /// 当前状态，播放-录制-暂停-停止等
    ARSpace::WorkState getWorkState() const { return workState; }
    void setWorkState(ARSpace::WorkState state);

    /**
     * @brief 录制
     * @param sampleRate 采样率Hz
     * @param sampleSize 采样大小bit
     * @param channelCount 通道数
     * @param deviceName 输入设备名称，建议在deviceInfo设置，这里置空
     */
    Q_INVOKABLE void record(int sampleRate = 16000,
                            int sampleSize = 16,
                            int channelCount = 1,
                            const QString &deviceName = QString());
    /// 暂停录制
    Q_INVOKABLE void suspendRecord();
    /// 暂停恢复录制
    Q_INVOKABLE void resumeRecord();
    /**
    * @brief 播放
    * @param deviceName 输入设备名称，建议在deviceInfo设置，这里置空
    */
    Q_INVOKABLE void play(const QString &deviceName = QString());
    /// 暂停播放
    Q_INVOKABLE void suspendPlay();
    /// 暂停恢复播放
    Q_INVOKABLE void resumePlay();
    /// 停止录制/播放
    Q_INVOKABLE void stop();

    /// 保存音频数据到文件
    Q_INVOKABLE bool saveToFile(const QString &filepath);
    /// 从文件读取音频数据
    /// 目前不带解析器，只能解析44字节定长wav-pcm格式头
    /// （与本组件生成的wav文件格式一致）
    Q_INVOKABLE bool loadFromFile(const QString &filepath);

protected:
    void paint(QPainter* painter) override;
    //void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    //void mouseReleaseEvent(QMouseEvent* event) override;
    //void hoverEnterEvent(QHoverEvent* event) override;
    //void hoverLeaveEvent(QHoverEvent* event) override;
    //void hoverMoveEvent(QHoverEvent* event) override;

private:
    /// 初始化
    void init();
    /// 释放
    void free();

signals:
    void workStateChanged(ARSpace::WorkState state);

private:
    /// 输入输出设备参数
    ARDevice device;
    /// 当前操作的音频数据，input/recorder和output/player可访问
    ARDataSource source;

    /// 处理音频录制
    ARDataInput *recorder{ nullptr };
    /// 处理音频播放
    ARDataOutput *player{ nullptr };

    /// 当前工作状态
    ARSpace::WorkState workState{ ARSpace::Stopped };
    /// 绘制模式
    ARSpace::DisplayMode displayMode{ ARSpace::Tracking };
};


