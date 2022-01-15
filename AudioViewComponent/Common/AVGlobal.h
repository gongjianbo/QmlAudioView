/****************************************************************
 **     《匆匆》--朱自清
 **
 **     燕子去了，有再来的时候；杨柳枯了，有再青的时候；桃花谢了，有再开的时候。
 ** 但是，聪明的，你告诉我，我们的日子为什么一去不复返呢？
 ** --是有人偷了他们罢：那是谁？又藏在何处呢？是他们自己逃走了罢：现在又到了哪里呢？
 **
 **     我不知道他们给了我多少日子；但我的手确乎是渐渐空虚了。在默默里算着，
 ** 八千多日子已经从我手中溜去；像针尖上一滴水滴在大海里，我的日子滴在时间的流里，
 ** 没有声音，也没有影子。我不禁头涔涔而泪潸潸了。
 **
 **     去的尽管去了，来的尽管来着；去来的中间，又怎样地匆匆呢？早上我起来的时候，
 ** 小屋里射进两三方斜斜的太阳。太阳他有脚啊，轻轻悄悄地挪移了；我也茫茫然跟着旋转。
 ** 于是--洗手的时候，日子从水盆里过去；吃饭的时候，日子从饭碗里过去；默默时，
 ** 便从凝然的双眼前过去。我觉察他去的匆匆了，伸出手遮挽时，他又从遮挽着的手边过去，
 ** 天黑时，我躺在床上，他便伶伶俐俐地从我身上跨过，从我脚边飞去了。等我睁开眼和太阳再见，
 ** 这算又溜走了一日。我掩着面叹息。但是新来的日子的影儿又开始在叹息里闪过了。
 **
 **     在逃去如飞的日子里，在千门万户的世界里的我能做些什么呢？
 ** 只有徘徊罢了，只有匆匆罢了；在八千多日的匆匆里，除徘徊外，又剩些什么呢？
 ** 过去的日子如轻烟，被微风吹散了，如薄雾，被初阳蒸融了；我留着些什么痕迹呢？
 ** 我何曾留着像游丝样的痕迹呢？我赤裸裸来到这世界，转眼间也将赤裸裸的回去罢？
 ** 但不能平的，为什么偏要白白走这一遭啊？
 **
 **     你聪明的，告诉我，我们的日子为什么一去不复返呢？
 ** （原载１９２２年４月１１日《时事新报·文学旬刊》第３４期）
 *******************************************************************/
#pragma once
#include <cmath>
#include <vector>
#include <QObject>
#include "AVWavDefine.h"

/// 此宏用来定义操作一致的属性getset
#define PROPERTY_GETSET(type, propertyName, defaultValue, getFunc, setFunc, notifySignal) \
    private: \
    type propertyName{ defaultValue }; \
    public: \
    type getFunc() const { \
        return this->propertyName; \
    } \
    void setFunc(const type &value) { \
        if(value != this->propertyName){ \
            this->propertyName = value; \
            Q_EMIT notifySignal(value); \
        } \
    } \
    Q_SIGNALS: \
    void notifySignal(const type &value);
/// 可读写属性
#define READWRITE_PROPERTY(type, propertyName, defaultValue, getFunc, setFunc, notifySignal) \
    private: Q_PROPERTY(type propertyName READ getFunc WRITE setFunc NOTIFY notifySignal) \
    PROPERTY_GETSET(type, propertyName, defaultValue, getFunc, setFunc, notifySignal)
/// 只读属性
#define READONLY_PROPERTY(type, propertyName, defaultValue, getFunc, setFunc, notifySignal) \
    private: Q_PROPERTY(type propertyName READ getFunc NOTIFY notifySignal) \
    PROPERTY_GETSET(type, propertyName, defaultValue, getFunc, setFunc, notifySignal)

/// delete删除后置位nullptr
#define FREE_OBJECT(obj) if(obj){ delete obj; obj = nullptr; }
#define FREE_ARRAY(arr) if(arr){ delete [] arr; arr = nullptr; }

/// 参数加引号变成字符串
#define OBJECT_NAME(obj) #obj

/// 流输出变量名和变量值，int i = 0; qDebug()<<OS_OBJECT(i);
#define OS_OBJECT(obj) OBJECT_NAME(obj)##":"<<obj

/// 放一些枚举定义，全局变量等
class AVGlobal : public QObject
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

    /// 九宫格位置
    /// view中的组件布局根据Position来确定位置，如series在PosCenter正中
    /// z值大的绘制在上层
    enum Position : int
    {
        PosNone = 0x00          //无效状态
        , PosLeft = 0x01        //横向左侧
        , PosHCenter = 0x02     //横向中间
        , PosRight = 0x04       //横向右侧
        , PosTop = 0x10         //竖向顶部
        , PosVCenter = 0x20     //竖向中间
        , PosBottom = 0x40      //竖向底部
        , PosTopLeft = 0x11     //左上角
        , PosTopRight = 0x14    //右上角
        , PosBottomLeft = 0x41  //左下角
        , PosBottomRight = 0x44 //右下角
        , PosCenter = 0x22      //正中间
    };
    Q_ENUM(Position)

    /// 方向，对应坐标系xy轴
    enum Direction : int
    {
        LeftToRight   //左到右
        , TopToBottom  //上到下
    };
    Q_ENUM(Direction)

    /// 错误信息
    enum ErrorType : int
    {
        NoError //无错误
        , InputFormatError  //录制参数错误
        , InputDeviceError  //录制设备错误
        , InputStartError   //录制启动失败
        , OutputFormatError //播放格式错误
        , OutputDeviceError //播放设备错误
        , OutputStartError  //播放启动失败
        , OutputEmptyError  //无音频数据供播放
    };
    Q_ENUM(ErrorType)

    /// 参照Qt的浮点数比较函数
    static inline bool fuzzyIsEqual(double p1, double p2)
    {
        const double t = std::abs(p1 - p2);
        if (t == 0) {
            return true;
        }
        return ((t / std::max(std::abs(p1), std::abs(p2))) <= 0.000000000001);
    }

    /// 参照Qt的浮点数比较函数
    static inline bool fuzzyIsEqual(float p1, float p2)
    {
        const float t = std::abs(p1 - p2);
        if (t == 0) {
            return true;
        }
        return ((t / std::max(std::abs(p1), std::abs(p2))) <= 0.00001f);
    }

    /// 参照Qt的浮点数判0函数
    static inline bool fuzzyIsZero(double d)
    {
        return (std::abs(d) <= 0.000000000001);
    }

    /// 参照Qt的浮点数判0函数
    static inline bool fuzzyIsZero(float f)
    {
        return (std::abs(f) <= 0.00001f);
    }
};

