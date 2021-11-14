#pragma once
#include <QAudio>
#include <QAudioFormat>

/**
 * @brief wav文件头结构体
 * @author 龚建波
 * @date 2020-11-12
 * @details
 * wav头是不定长格式，不过这里用的比较简单的格式，便于处理
 * 包含RIFF块、文件格式标志、fmt块、压缩编码时fact块、data块
 * （数值以小端存储，不过pc一般默认小端存储，暂不特殊处理）
 * 参照：https://www.cnblogs.com/ranson7zop/p/7657874.html
 * 参照：https://www.cnblogs.com/Ph-one/p/6839892.html
 * @note
 * 最大元素为4字节，所以对齐后本身就是44字节的
 * 加字节对齐处理只是为了提醒
 */
#pragma pack(push,1)
struct ARWavHead
{
    char riffFlag[4]; //文档标识，大写"RIFF"
    //从下一个字段首地址开始到文件末尾的总字节数。
    //该字段的数值加 8 为当前文件的实际长度。
    unsigned int riffSize; //数据长度
    char waveFlag[4]; //文件格式标识，大写"WAVE"
    char fmtFlag[4]; //格式块标识，小写"fmt "
    //格式块长度，可以是 16、 18 、20、40 等
    //值应该是compressionCode到bitsPerSample的大小，这里为16
    unsigned int fmtSize; //格式块长度
    unsigned short compressionCode; //编码格式代码，1为pcm
    unsigned short numChannels; //通道个数
    unsigned int sampleRate; //采样频率
    //pcm编码时，该数值为:声道数×采样频率×每样本的数据位数/8。
    //播放软件利用此值可以估计缓冲区的大小。
    unsigned int bytesPerSecond; //码率（数据传输速率）
    //采样帧大小。该数值为:声道数×位数/8。
    //播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区。
    unsigned short blockAlign; //数据块对其单位
    //存储每个采样值所用的二进制数位数。常见的位数有16、24、32
    unsigned short bitsPerSample; //采样位数/深度/精度
    char dataFlag[4]; //表示数据开头，小写"data"
    unsigned int dataSize; //数据部分的长度

    /**
     * @brief 根据参数生成wav(pcm)文件头信息
     * @param sampleRate 采样率，如16000Hz
     * @param sampleSize 采样精度，如16位
     * @param channelCount 声道数，如1单声道
     * @param dataSize 有效数据字节数
     * @return wav头信息的ARWavHead结构体
     */
    static ARWavHead createWavHead(int sampleRate, int sampleSize,
                                   int channelCount, unsigned int dataSize);

    /**
     * @brief 检测该头数据是否有效
     * 主要用在读取并解析使用该头格式写的文件
     * @param head wav头信息的ARWavHead结构体
     * @return =true则格式有效
     */
    static bool isValidWavHead(const ARWavHead &head);
};
#pragma pack(pop)

/**
 * @brief 放一些枚举定义，全局变量等
 */
class ARSpace : public QObject
{
    Q_OBJECT
public:
    /// 操作状态
    enum WorkState : int
    {
        Stopped        //默认停止状态
        , Playing      //播放中
        , PlayPaused   //播放暂停
        , Recording    //录制中
        , RecordPaused //录制暂停
    };
    Q_ENUM(WorkState)

};
