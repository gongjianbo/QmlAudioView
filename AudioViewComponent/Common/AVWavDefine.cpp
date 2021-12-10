#include "AVWavDefine.h"
#include <memory>

const char *RIFF_FLAG = "RIFF";
const char *WAVE_FLAG = "WAVE";
const char *FMT_FLAG = "fmt ";
const char *DATA_FLAG = "data";

AVWavHead::AVWavHead(const QByteArray &audioData)
{
    if(audioData.size() < sizeof(AVWavHead))
        return;
    memcpy(this, audioData.constData(), sizeof(AVWavHead));
}

AVWavHead::AVWavHead(int sampleRate, int sampleSize,
                     int channelCount, unsigned int dataSize)
{
    //如果长度不能被采样位宽整除，就去掉x个字节的数据
    //if(dataSize%sampleByte!=0){
    //    dataSize-=xByte;
    //}

    //先清零再赋值
    memset(this, 0, sizeof(AVWavHead));

    memcpy(riff.chunkId, RIFF_FLAG, 4);
    memcpy(riff.format, WAVE_FLAG, 4);
    memcpy(fmt.chunkId, FMT_FLAG, 4);
    memcpy(data.chunkId, DATA_FLAG, 4);

    //除去头部前8个字节的长度，用的44字节的定长格式头，所以+44-8=36
    riff.chunkSize = dataSize + 36;
    //fmt块大小
    fmt.chunkSize = 16;
    //1为pcm
    fmt.audioFormat = 0x01;
    fmt.numChannels = channelCount;
    fmt.sampleRate = sampleRate;
    fmt.byteRate = (sampleSize / 8) * channelCount * sampleRate;
    fmt.blockAlign = (sampleSize / 8) * channelCount;
    fmt.bitsPerSample = sampleSize;
    //除去头的数据长度
    data.chunkSize = dataSize;
}

bool AVWavHead::isValid() const
{
    //简单的比较，主要用在未使用解析器时解析wav头
    if (memcmp(riff.chunkId, RIFF_FLAG, 4) != 0 ||
            memcmp(riff.format, WAVE_FLAG, 4) != 0 ||
            memcmp(fmt.chunkId, FMT_FLAG, 4) != 0 ||
            memcmp(data.chunkId, DATA_FLAG, 4) != 0 ||
            riff.chunkSize != data.chunkSize + 36 ||
            fmt.audioFormat != 0x01)
        return false;
    return true;
}
