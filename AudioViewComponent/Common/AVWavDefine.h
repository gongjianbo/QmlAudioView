#pragma once
#include <QObject>
#include <QAudioFormat>

/**
 * @brief wav文件头结构体
 * @author 龚建波
 * @date 2020-11-12
 * @details
 * wav格式头是不定长的，不过这里用的比较简单的固定格式，便于处理
 * 包含RIFF块、文件格式标志、fmt块、压缩编码时fact块、data块
 * （数值以小端存储，不过pc一般默认小端存储，暂不特殊处理）
 * 参照：https://www.cnblogs.com/ranson7zop/p/7657874.html
 * 参照：https://www.cnblogs.com/Ph-one/p/6839892.html
 * 参照：http://mobile.soomal.com/doc/10100001099.htm
 * @todo
 * 划分为RIFF和RF64，以支持更长的数据内容
 * @note
 * 此处wav/wave指Microsoft Wave音频文件格式，后缀为".wav"
 * wave长度变量为uint32四个字节，只支持到4GB（2^32）
 * （网上有说只支持2G，用工具拼接实测是可以支持到接近4GB的）
 * 对于更长的数据，可以使用Sony Wave64，后缀为".w64"；
 * 或者使用RF64，RF64即RIFF的64bit版本。
 * Adobe Audition软件保存wav时如果数据超过4GB，会使用RF64格式保存
 */
#pragma pack(push,1)
/// wav格式头的RIFF块
/// RIFF即Resource Interchange File Format，中文意思为资源交换文件格式，
/// 它是一种基础的文件格式，最初由Microsoft和IBM在1991年定义，
/// 它约定了资源类型文件的基本结构，Microsoft的Wave和AVI文件都是基于RIFF机构的
struct AVWavRiffChunk
{
    char chunkId[4]; //文档标识，大写"RIFF"
    //从下一个字段首地址开始到文件末尾的总字节数。
    //该字段的数值加 8 为当前文件的实际长度。
    unsigned int chunkSize;
    char format[4]; //文件格式标识，大写"WAVE"
};

/// wav格式头的FMT块
struct AVWavFmtChunk
{
    char chunkId[4]; //格式块标识，小写"fmt "
    //格式块长度，可以是 16、 18 、20、40 等
    //值应该是audioFormat到bitsPerSample的字节大小，目前固定为16
    unsigned int chunkSize; //格式块长度
    unsigned short audioFormat; //编码格式代码，1为pcm
    unsigned short numChannels; //通道个数
    unsigned int sampleRate; //采样频率
    //pcm编码时，该数值为:声道数×采样频率×每样本的数据位数/8。
    //播放软件利用此值可以估计缓冲区的大小。
    unsigned int byteRate; //码率（数据传输速率）
    //采样帧大小。该数值为:声道数×位数/8。
    //播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区。
    unsigned short blockAlign; //数据块对其单位
    //存储每个采样值所用的二进制数位数。常见的位数有16、24、32
    unsigned short bitsPerSample; //采样位数/深度/精度
};

/// wav格式头的DATA块
struct AVWavDataChunk
{
    char chunkId[4]; //表示数据开头，小写"data"
    unsigned int chunkSize; //数据部分的长度
};

/// wave格式头，目前取44字节固定格式
struct AVWavHead
{
    AVWavRiffChunk riff;
    AVWavFmtChunk fmt;
    AVWavDataChunk data;

    /// 默认构造，啥也不干
    AVWavHead(){}

    /**
     * @brief 根据读取的文件数据构造wav格式头
     * 一般用在读文件时，构造后使用isValid()判断有效性
     * @param audioData 音频格式头数据，目前仅支持44字节格式头
     */
    AVWavHead(const QByteArray &audioData);

    /**
     * @brief 根据采样率、精度等参数构造wav格式头
     * 一般在写文件时根据参数调用此构造
     * @param sampleRate 采样率，如16000Hz
     * @param sampleSize 采样精度，如16位
     * @param channelCount 声道数，如1单声道
     * @param dataSize 有效数据字节数
     */
    AVWavHead(int sampleRate, int sampleSize,
              int channelCount, unsigned int dataSize);

    /**
     * @brief 判断该wav头参数是否有效
     * 主要用在读取并解析使用该头格式写的文件
     * @return =true则格式有效
     */
    bool isValid() const;
};
#pragma pack(pop)
