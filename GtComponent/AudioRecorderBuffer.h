#pragma once
#include <QIODevice>

/**
 * @brief 用于IODevice/Buffer回调
 * @author 龚建波
 * @date 2021-1-12
 * @details
 * 实际存储数据的类继承该类，在IODevice/Buffer类回调时调用虚函数
 */
class AudioRecorderCallback
{
public:
    AudioRecorderCallback() = default;
    virtual ~AudioRecorderCallback() = 0;

    //虚函数接口用于数据读写回调
    virtual qint64 readData(char* data, qint64 maxSize);
    virtual qint64 writeData(const char* data, qint64 maxSize);
};

/**
 * @brief QAudioInput/Output等处理数据时的辅助类
 * @author 龚建波
 * @date 2021-1-12
 * @details
 * 读写时回调Callback的读写接口
 */
class AudioRecorderBuffer : public QIODevice
{
    Q_OBJECT
public:
    explicit AudioRecorderBuffer(AudioRecorderCallback* callback, QObject* parent = nullptr);

    //虚函数接口用于数据读写回调
    qint64 readData(char* data, qint64 maxSize) override;
    qint64 writeData(const char* data, qint64 maxSize) override;

private:
    //读写数据时再回调view相关接口进行操作
    AudioRecorderCallback* callbackPtr{ nullptr };
};
