#pragma once
#include <QIODevice>

/**
 * @brief 用于IODevice/Buffer回调
 * @author 龚建波
 * @date 2021-1-12
 * @details
 * 实际存储数据的类继承该类，在IODevice/Buffer类回调时调用虚函数
 */
class ARCallback
{
public:
    ARCallback() = default;
    virtual ~ARCallback() = 0;

    /// 输出时回调read
    virtual qint64 readData(char *data, qint64 maxSize);
    /// 录入时回调write
    virtual qint64 writeData(const char *data, qint64 maxSize);
};

/**
 * @brief QAudioInput/Output等处理数据时的辅助类
 * @author 龚建波
 * @date 2021-1-12
 * @details
 * 读写时回调Callback的读写接口
 */
class ARDataBuffer : public QIODevice
{
    Q_OBJECT
public:
    explicit ARDataBuffer(ARCallback *callback, QObject *parent = nullptr);

    /// 输出时回调read
    qint64 readData(char *data, qint64 maxSize) override;
    /// 录入时回调write
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    /// 读写数据时再回调相关接口进行操作
    ARCallback *callbackPtr{ nullptr };
};
