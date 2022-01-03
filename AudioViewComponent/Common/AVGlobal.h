#pragma once
/// 此文件放置一些全局定义
/// 龚建波 2021-12-10
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

