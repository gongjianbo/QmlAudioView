#include "AVDataOutput.h"

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

AVDataOutput::AVDataOutput(QObject *parent)
    : QObject(parent)
{
    //作为QAudioInput/Output的start启动参数，处理时回调read/write接口
    audioBuffer = new AVDataBuffer(this, this);
    audioBuffer->open(QIODevice::ReadOnly);
}

AVDataOutput::~AVDataOutput()
{
    freePlay();
}

qint64 AVDataOutput::readData(char *data, qint64 maxSize)
{
    //此处不判断Source，在start时判断
    if (!data || maxSize < 1 || !audioSource)
        return 0;
    const std::vector<char> &audio_data = audioSource->getData();
    //如果是选区播放，可以将截至位置减去播放位置
    const qint64 data_size = audioSource->size() - outputCount;
    if (data_size <= 0) {
        //stateChanged没有触发停止，但return 0 会触发 IdleState 状态
        //现在改为通过IdleState状态判断结束
        //QTimer::singleShot(1, [this] { playFinished(); });
        return 0;
    }

    const qint64 read_size = (data_size >= maxSize) ? maxSize : data_size;
    memcpy(data, audio_data.data() + outputCount, read_size);
    outputCount += read_size;
    //refresh(); 这个间隔回调太大了，不适合用来刷新播放进度
    return read_size;
}

AVDataSource *AVDataOutput::getAudioSource()
{
    return audioSource;
}

void AVDataOutput::setAudioSource(AVDataSource *source)
{
    if (audioSource == source) {
        return;
    }
    audioSource = source;
    emit audioSourceChanged();
}

qint64 AVDataOutput::getPosition() const
{
    return playPosition;
}

void AVDataOutput::setPosition(qint64 position)
{
    if (playPosition != position) {
        playPosition = position;
        emit positionChanged(position);
    }
}

qint64 AVDataOutput::getCurrentIndex() const
{
    return playCurrentIndex;
}

void AVDataOutput::setCurrentIndex(qint64 index)
{
    if (!audioSource) {
        return;
    }
    const QAudioFormat format = audioSource->getFormat();
    const qint64 sample_byte = format.sampleSize() / 8;
    //data size为0时index=负数，下一步会重置为index=0
    if (index >= audioSource->size() - sample_byte) {
        index = audioSource->size() - sample_byte;
    }
    if (index < 0) {
        index = 0;
    }
    if (playCurrentIndex != index) {
        playCurrentIndex = index;
        emit currentIndexChanged(index);

        //const QAudioFormat format = audioSource->getFormat();
        const int sample_rate = format.sampleRate();
        //const int sample_byte = format.sampleSize() / 8;
        const int channel_count = format.channelCount();
        //在播放的时候，数组下标比播放位置小一个采样，此处补齐
        index += sample_byte;
        const qint64 position = ((index / sample_byte) / (1.0 * channel_count * sample_rate) * 1000);
        setPosition(position);
    }
}

void AVDataOutput::zeroCurrentIndex()
{
    playCurrentIndex = 0;
    emit currentIndexChanged(0);
    setPosition(0);
}

qint64 AVDataOutput::getStartIndex() const
{
    return playStartIndex;
}

void AVDataOutput::setStartIndex(qint64 index)
{
    if (playStartIndex != index) {
        playStartIndex = index;
        emit startIndexChanged(index);
    }
}

/*void AVDataOutput::setCurrentOffset(qint64 offset)
{
   audioSource 先判断再使用
   const QAudioFormat format = audioSource->getFormat();
    if (offset > 0 && format.sampleSize() > 0) {
        audioBuffer->reset();
        const qint64 sample_byte = format.sampleSize() / 8;
        offset -= (offset % sample_byte);
        //data size为0时index=-负数，下一步会重置为index=0
        if (offset >= audioSource->size() - sample_byte) {
            offset = audioSource->size() - sample_byte;
        }
        if (offset < 0) {
            offset = 0;
        }
        outputCount = offset;
        if (!audioOutput || audioOutput->state() == QAudio::StoppedState) {
            outputOffset = offset;
        }
        else {
            outputOffset = outputOffset + offset - playCurrentIndex;
        }
        setCurrentIndex(offset);
    }
}*/

QAudio::State AVDataOutput::getState() const
{
    if (audioOutput) {
        return audioOutput->state();
    }
    return QAudio::StoppedState;
}

bool AVDataOutput::startPlay(const QAudioDeviceInfo &device, const QAudioFormat &format)
{
    qDebug() << __FUNCTION__ << device.deviceName() << format;
    endPlay();

    outputDevice = device;
    outputFormat = format;
    //无效的参数
    if (!audioSource || !outputFormat.isValid() || outputDevice.isNull() || audioSource->isEmpty()) {
        qDebug() << __FUNCTION__ << "failed, sample rate:" << outputFormat.sampleRate()
                 << "device null:" << outputDevice.isNull() << outputDevice.supportedSampleRates();
        if (!audioSource) {
            emit errorChanged(AVGlobal::OutputSourceError);
        }
        else if (!outputFormat.isValid()) {
            emit errorChanged(AVGlobal::OutputFormatError);
        }
        else if (outputDevice.isNull()) {
            emit errorChanged(AVGlobal::OutputDeviceError);
        }
        else if (audioSource->isEmpty()) {
            emit errorChanged(AVGlobal::OutputEmptyError);
        }
        return false;
    }
    //参数不相等才重新new
    if (audioOutput && (currentDevice != outputDevice || currentFormat != outputFormat)) {
        freePlay();
    }
    if (!audioOutput) {
        //保存当前deviceinfo，下次对比是否相同
        currentDevice = outputDevice;
        currentFormat = outputFormat;
        //QAudioFormat n_format=outputDevice.nearestFormat(outputFormat);
        audioOutput = new QAudioOutput(outputDevice, outputFormat, this);
        connect(audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State state)
        {
            emit stateChanged(state);
            //没有音频数据可供处理时触发IdleState状态
            if (state == QAudio::IdleState) {
                emit playFinished();
            }
        });
        connect(audioOutput, &QAudioOutput::notify, this, &AVDataOutput::onNotify);
        //目前用notify来控制进度刷新
        audioOutput->setNotifyInterval(30);
        //缓冲区
        //audioOutput->setBufferSize(12800);
    }
    audioBuffer->reset();
    //pull mode: m_audioOutput->start(m_generator.data());
    //push mode: auto io = m_audioOutput->start();
    audioOutput->start(audioBuffer);
    //qDebug()<<audioOutput->bufferSize()<<"buffer";
    if (audioOutput->error() != QAudio::NoError) {
        emit errorChanged(AVGlobal::OutputStartError);
        return false;
    }
    return true;
}

void AVDataOutput::stopPlay()
{
    if (audioOutput) {
        audioOutput->stop();
    }
    outputCount = 0;
    outputOffset = 0;
    setStartIndex(0);
    zeroCurrentIndex();
}

void AVDataOutput::endPlay()
{
    if (audioOutput) {
        audioOutput->stop();
    }
    //如果当前位置已在结尾，则回到播放起始位置重新播放
    //todo判断选区
    if (audioSource && outputCount >= audioSource->size()) {
        outputCount = 0;
        //outputOffset = 0;
        setStartIndex(0);
        zeroCurrentIndex();
    }
    outputOffset = outputCount;
}

void AVDataOutput::suspendPlay()
{
    if (audioOutput) {
        audioOutput->suspend();
    }
}

void AVDataOutput::resumePlay()
{
    if (audioOutput) {
        audioOutput->resume();
    }
}

void AVDataOutput::freePlay()
{
    if (audioOutput) {
        audioOutput->stop();
        audioBuffer->close();

        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}

bool AVDataOutput::saveFile(const QString &filepath)
{
    stopPlay();
    if (!audioSource) {
        return false;
    }
    //QFile不能生成目录
    QFileInfo info(filepath);
    if (!info.dir().exists()) {
        info.dir().mkpath(info.absolutePath());
    }

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QAudioFormat format = audioSource->getFormat();
    const std::vector<char> &data = audioSource->getData();
    if (data.size() == 0) {
        return false;
    }

    //文件头
    AVWavHead head(format.sampleRate(), format.sampleSize(),
                   format.channelCount(), (unsigned int)data.size());
    //写文件
    file.write((const char*)(&head), sizeof(AVWavHead));
    qint64 data_size = (qint64)data.size();
    while (data_size > 1024 * 1024 * 10) {
        file.write(data.data(), 1024 * 1024 * 10);
        data_size -= 1024 * 1024 * 10;
    }
    //尾巴那点数据
    file.write(data.data(), data_size);
    file.close();
    return true;
}

void AVDataOutput::onNotify()
{
    if (!audioOutput || !audioSource)
        return;
    if (audioSource->isEmpty()) {
        zeroCurrentIndex();
        return;
    }
    //用processedUSecs获取start到当前的us数，但是start后有点延迟
    //进度=已放时间和总时间之比*总字节数，注意时间单位
    qint64 cur_pos = outputOffset + (audioOutput->processedUSecs() / 1000.0) /
            audioSource->getDuration() * audioSource->size();
    if (cur_pos > outputCount) {
        cur_pos = outputCount;
    }
    //减temp_offset是为了补偿缓冲区还未播放的时差，音画同步
    int temp_offset = (audioOutput->bufferSize() - audioOutput->bytesFree());
    if (temp_offset < 0) {
        temp_offset = 0;
    }
    //减一个采样做下标
    cur_pos = cur_pos - temp_offset - audioSource->getSampleSize() / 8;
    if (cur_pos < 0) {
        cur_pos = 0;
    }
    setCurrentIndex(cur_pos);
}
