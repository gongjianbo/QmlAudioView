#include "AVDataBuffer.h"

AVCallback::~AVCallback()
{

}

qint64 AVCallback::readData(char *data, qint64 maxSize)
{
    return -1;
}

qint64 AVCallback::writeData(const char *data, qint64 maxSize)
{
    return -1;
}

AVDataBuffer::AVDataBuffer(AVCallback *callback, QObject *parent)
    : QIODevice(parent), callbackPtr(callback)
{
    Q_ASSERT(callbackPtr != nullptr);
}

qint64 AVDataBuffer::readData(char *data, qint64 maxSize)
{
    return callbackPtr->readData(data, maxSize);
}

qint64 AVDataBuffer::writeData(const char *data, qint64 maxSize)
{
    return callbackPtr->writeData(data, maxSize);
}
