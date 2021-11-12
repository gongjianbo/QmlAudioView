#include "ARDataBuffer.h"

ARCallback::~ARCallback()
{

}

qint64 ARCallback::readData(char *data, qint64 maxSize)
{
    return -1;
}

qint64 ARCallback::writeData(const char *data, qint64 maxSize)
{
    return -1;
}

ARDataBuffer::ARDataBuffer(ARCallback *callback, QObject *parent)
    : QIODevice(parent), callbackPtr(callback)
{
    Q_ASSERT(callbackPtr != nullptr);
}

qint64 ARDataBuffer::readData(char *data, qint64 maxSize)
{
    return callbackPtr->readData(data, maxSize);
}

qint64 ARDataBuffer::writeData(const char *data, qint64 maxSize)
{
    return callbackPtr->writeData(data, maxSize);
}
