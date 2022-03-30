#include "AVDataInput.h"

#include <QFile>
#include <QDebug>

AVDataInput::AVDataInput(QObject *parent)
    : QObject(parent)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new AVDataBuffer(this, this);
    audioBuffer->open(QIODevice::WriteOnly);
}

AVDataInput::~AVDataInput()
{
    freeRecord();
}

qint64 AVDataInput::writeData(const char *data, qint64 maxSize)
{
    //此处不判断Source，在start时判断
    //双声道时数据为一左一右连续
    //QByteArray new_data = QByteArray(data, maxSize);
    if (audioSource && maxSize > 0 && audioSource->maxSize() > maxSize) {
        audioSource->appendData(data, maxSize);
    }
    else {
        qDebug() << __FUNCTION__ << "data size out of range";
    }
    return maxSize;
}

AVDataSource *AVDataInput::getAudioSource()
{
    return audioSource;
}

void AVDataInput::setAudioSource(AVDataSource *source)
{
    if (audioSource == source) {
        return;
    }
    if (audioSource) {
        audioSource->disconnect(this);
    }
    audioSource = source;
    if (audioSource) {
        connect(audioSource, &AVDataSource::durationChanged, this, &AVDataInput::setDuration);
    }
    emit audioSourceChanged();
}

qint64 AVDataInput::getDuration() const
{
    return inputDuration;
}

void AVDataInput::setDuration(qint64 duration)
{
    if (inputDuration != duration) {
        inputDuration = duration;
        emit durationChanged(duration);
    }
}

QAudio::State AVDataInput::getState() const
{
    if (audioInput) {
        return audioInput->state();
    }
    return QAudio::StoppedState;
}

bool AVDataInput::startRecord(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << __FUNCTION__ << device.deviceName() << format;
    stopRecord();

    inputDevice = device;
    inputFormat = format;
    //清空已有的数据
    if (audioSource) {
        audioSource->clear();
        audioSource->setFormat(format);
    }
    //无效的参数
    if (!audioSource || !inputFormat.isValid() || inputDevice.isNull()) {
        qDebug() << __FUNCTION__ << "failed, sample rate:" << inputFormat.sampleRate()
                 << "device null:" << inputDevice.isNull() << inputDevice.supportedSampleRates();
        if (!audioSource) {
            emit errorChanged(AVGlobal::InputSourceError);
        }
        else if (!inputFormat.isValid()) {
            emit errorChanged(AVGlobal::InputFormatError);
        }
        else if (inputDevice.isNull()) {
            emit errorChanged(AVGlobal::InputDeviceError);
        }
        return false;
    }
    //参数不相等才重新new
    if (audioInput && (currentDevice != inputDevice || currentFormat != inputFormat)) {
        freeRecord();
    }
    if (!audioInput) {
        //保存当前deviceinfo，下次对比是否相同
        currentDevice = inputDevice;
        currentFormat = inputFormat;
        audioInput = new QAudioInput(currentDevice, currentFormat, this);
        connect(audioInput, &QAudioInput::stateChanged, this, &AVDataInput::stateChanged);
        connect(audioInput, &QAudioInput::notify, this, &AVDataInput::onNotify);
        audioInput->setBufferSize(inputFormat.sampleRate() * inputFormat.channelCount());
    }
    audioBuffer->reset();
    //pull mode: m_audioInput->start(m_audioInfo.data());
    //push mode: auto io = m_audioInput->start();
    audioInput->start(audioBuffer);
    if (audioInput->error() != QAudio::NoError) {
        emit errorChanged(AVGlobal::InputStartError);
        return false;
    }
    return true;
}

void AVDataInput::stopRecord()
{
    if (audioInput) {
        audioInput->stop();
    }
}

void AVDataInput::suspendRecord()
{
    if (audioInput) {
        audioInput->suspend();
    }
}

void AVDataInput::resumeRecord()
{
    if (audioInput) {
        audioInput->resume();
    }
}

void AVDataInput::freeRecord()
{
    if (audioInput) {
        audioInput->stop();
        audioBuffer->close();

        audioInput->deleteLater();
        audioInput = nullptr;
    }
}

bool AVDataInput::loadFile(const QString &filepath)
{
    stopRecord();
    if (!audioSource) {
        return false;
    }

    QFile file(filepath);
    if (file.exists() && file.size() > 44 && file.open(QIODevice::ReadOnly))
    {
        AVWavHead head;
        file.read((char*)&head, 44);
        qint64 data_size = file.size() - 44;
        std::vector<char> read_data;
        read_data.reserve(data_size);
        if (head.isValid() &&  data_size > 0) {
            qint64 buf_size = 1024 * 1024 * 10;
            std::vector<char> read_buf;
            read_buf.resize(buf_size);
            qint64 once_size = 0;
            //读取
            //pcm_data = file.readAll();
            while (!file.atEnd()) {
                once_size = file.read(read_buf.data(), buf_size);
                if (once_size > 0 && once_size <= buf_size) {
                    read_data.insert(read_data.end(), read_buf.cbegin(), read_buf.cbegin() + once_size);
                }
            }
        }
        file.close();
        //采样率等置为相同参数
        if (head.isValid() && data_size > 0 &&
                data_size == (qint64)read_data.size() &&
                data_size == (qint64)head.data.chunkSize &&
                (data_size % head.fmt.numChannels == 0)) {
            QAudioFormat format = audioSource->getFormat();
            format.setSampleRate(head.fmt.sampleRate);
            format.setChannelCount(head.fmt.numChannels);
            format.setSampleSize(head.fmt.bitsPerSample);
            inputFormat = format;
            audioSource->setFormat(format);

            //有效的信息，更新当前波形数据
            audioSource->setData(read_data);
            return true;
        }
    }
    return false;
}

void AVDataInput::onNotify()
{

}
