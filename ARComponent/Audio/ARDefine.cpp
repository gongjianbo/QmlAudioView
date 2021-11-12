#include "ARDefine.h"

const char *RIFF_FLAG = "RIFF";
const char *WAVE_FLAG = "WAVE";
const char *FMT_FLAG = "fmt ";
const char *DATA_FLAG = "data";

ARWavHead ARWavHead::createWavHead(int sampleRate, int sampleSize,
                                   int channelCount, unsigned int dataSize)
{
    //如果长度不能被采样位宽整除，就去掉x个字节的数据
    //if(dataSize%sampleByte!=0){
    //    dataSize-=xByte;
    //}

    //部分参数暂时用的固定值
    const int bits = sampleSize;
    const int channels = channelCount;
    const int head_size = sizeof(ARWavHead);
    //先清零再赋值
    ARWavHead wav_head;
    memset(&wav_head, 0, head_size);

    memcpy(wav_head.riffFlag, RIFF_FLAG, 4);
    memcpy(wav_head.waveFlag, WAVE_FLAG, 4);
    memcpy(wav_head.fmtFlag, FMT_FLAG, 4);
    memcpy(wav_head.dataFlag, DATA_FLAG, 4);

    //除去头部前8个字节的长度，用的44字节的定长格式头，所以+44-8=36
    wav_head.riffSize = dataSize + 36;
    //fmt块大小
    wav_head.fmtSize = 16;
    //1为pcm
    wav_head.compressionCode = 0x01;
    wav_head.numChannels = channels;
    wav_head.sampleRate = sampleRate;
    wav_head.bytesPerSecond = (bits / 8) * channels * sampleRate;
    wav_head.blockAlign = (bits / 8) * channels;
    wav_head.bitsPerSample = bits;
    //除去头的数据长度
    wav_head.dataSize = dataSize;

    return wav_head;
}

bool ARWavHead::isValidWavHead(const ARWavHead &head)
{
    //简单的比较，主要用在未使用解析器时解析wav头
    if (memcmp(head.riffFlag, RIFF_FLAG, 4) != 0 ||
            memcmp(head.waveFlag, WAVE_FLAG, 4) != 0 ||
            memcmp(head.fmtFlag, FMT_FLAG, 4) != 0 ||
            memcmp(head.dataFlag, DATA_FLAG, 4) != 0 ||
            head.riffSize != head.dataSize + 36 ||
            head.compressionCode != 0x01)
        return false;
    return true;
}
