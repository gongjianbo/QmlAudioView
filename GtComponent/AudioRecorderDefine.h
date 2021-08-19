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
struct AudioRecorderWavHead
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

    //生成wav(pcm)文件头信息
    //[in] sampleRate: 采样频率Hz
    //[in] sampleSize: 采样大小bit
    //[in] channelCount: 通道数
    //[in] dataSize: pcm数据字节长度
    //[return] EasyWavHead: wav头
    static AudioRecorderWavHead createWavHead(int sampleRate,
                                              int sampleSize,
                                              int channelCount,
                                              unsigned int dataSize);

    //检测该头数据是否有效
    //主要用在读取并解析使用该头格式写的文件
    static bool isValidWavHead(const AudioRecorderWavHead &head);
};
#pragma pack(pop)

/**
 * @brief 放一些枚举定义，全局变量等
 */
class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    //状态
    //是否需要增加中间态，即ui设置了，但是线程操作状态还没返回
    enum RecordState : int
    {
        Stopped       //默认停止状态
        ,Playing      //播放中
        ,PlayPaused   //播放暂停
        ,Recording    //录制中
        ,RecordPaused //录制暂停
    };
    Q_ENUMS(RecordState)
    //显示模式
    enum DisplayMode : int
    {
        FullRange  //绘制全部数据
        ,Tracking  //跟踪最新数据
    };
    Q_ENUMS(DisplayMode)
};
