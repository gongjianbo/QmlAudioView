#include "AudioRecorderBuffer.h"

AudioRecorderCallback::~AudioRecorderCallback()
{

}

qint64 AudioRecorderCallback::readData(char *, qint64)
{
    return -1;
}

qint64 AudioRecorderCallback::writeData(const char *, qint64)
{
    return -1;
}

AudioRecorderBuffer::AudioRecorderBuffer(AudioRecorderCallback *callback, QObject *parent)
    : QIODevice(parent),callbackPtr(callback)
{
    Q_ASSERT(callbackPtr!=nullptr);
}

qint64 AudioRecorderBuffer::readData(char *data, qint64 maxSize)
{
    return callbackPtr->readData(data,maxSize);
}

qint64 AudioRecorderBuffer::writeData(const char *data, qint64 maxSize)
{
    return callbackPtr->writeData(data,maxSize);
}
