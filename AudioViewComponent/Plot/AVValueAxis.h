#pragma once
#include "Common/AVGlobal.h"
#include "AVAbstractAxis.h"

/**
 * @brief 坐标轴-通用型
 * @author 龚建波
 * @date 2022-01-13
 * @note
 * 1.limit和value有set和change两种修改值的接口，其中change接口只触发changed信号，不触发计算
 *   但是set接口会触发layerChanged、calcAxis等接口调用
 */
class AVValueAxis : public AVAbstractAxis
{
    Q_OBJECT
    // 刻度计算模式
    Q_PROPERTY(AVValueAxis::CalcMode calcMode READ getCalcMode WRITE setCalcMode NOTIFY calcModeChanged)
    /// 小数显示精度位数,3则range显示3位小数
    Q_PROPERTY(int decimalPrecision READ getDecimalPrecision WRITE setDecimalPrecision NOTIFY decimalPrecisionChanged)
    /// 固定值间隔
    Q_PROPERTY(double fixedValueSpace READ getFixedValueSpace WRITE setFixedValueSpace NOTIFY fixedValueSpaceChanged)
    /// 像素参考间隔
    Q_PROPERTY(int refPixelSpace READ getRefPixelSpace WRITE setRefPixelSpace NOTIFY refPixelSpaceChanged)
    /// 最小值限制，配合value范围动态调整设置
    Q_PROPERTY(double minLimit READ getMinLimit WRITE setMinLimit NOTIFY minLimitChanged)
    /// 最大值限制，配合value范围动态调整设置
    Q_PROPERTY(double maxLimit READ getMaxLimit WRITE setMaxLimit NOTIFY maxLimitChanged)
    /// 最小范围限制
    Q_PROPERTY(double minRange READ getMinRange WRITE setMinRange NOTIFY minRangeChanged)
    /// 当前显示最小值
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
    /// 当前显示最大值
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)
public:
    // 刻度线的间隔计算方式
    enum CalcMode {
        /// 固定值间隔
        FixedValue,
        /// 根据参考像素间隔
        RefPixel
    };
    Q_ENUM(CalcMode)
public:
    explicit AVValueAxis(QObject *parent = nullptr);

    // 刻度计算模式
    AVValueAxis::CalcMode getCalcMode() const;
    void setCalcMode(AVValueAxis::CalcMode mode);

    // 小数精度
    int getDecimalPrecision() const;
    void setDecimalPrecision(int precison);

    // 固定值的间隔
    double getFixedValueSpace() const;
    void setFixedValueSpace(double value);

    // 参考像素范围的间隔
    int getRefPixelSpace() const;
    void setRefPixelSpace(int pixel);

    // 最小值限制
    double getMinLimit() const;
    void setMinLimit(double limit);
    // change接口只触发changed信号，不触发计算
    void changeMinLimit(double limit);

    // 最大值限制
    double getMaxLimit() const;
    void setMaxLimit(double limit);
    void changeMaxLimit(double limit);

    // 最小范围限制
    double getMinRange() const;
    void setMinRange(double limit);
    void changeMinRange(double limit);

    // 当前显示的最小刻度
    double getMinValue() const;
    void setMinValue(double value);
    void changeMinValue(double value);

    // 当前显示的最大刻度
    double getMaxValue() const;
    void setMaxValue(double value);
    void changeMaxValue(double value);

    // 像素与值的换算
    double getUnit1PxToValue() const;
    double getUnit1ValueToPx() const;

    /**
    * @brief 坐标轴像素值转数值
    * @details 暂时只有2方向,
    * Qt绘制起点为左上角，往右下角取正.
    * @param px 鼠标pos
    * 该函数只负责计算对应的刻度数值，横向时可能参数要减去left,
    * 竖向时可能参数先被bottom+1减一下
    * @return 对应的刻度数值
    */
    double pxToValue(double px) const;

    /**
    * @brief 数值转坐标轴像素值
    * @details 暂时只有2方向,
    * Qt绘制起点为左上角，往右下角取正.
    * @param value 对应的刻度数值
    * @return 鼠标所在point对应的px长度，
    * 该函数只负责计算距离，横向时可能要拿得到的px加上left,
    * 竖向时可能需要拿bottom+1来减去得到的px.
    */
    double valueToPx(double value) const;

    // 绘制
    void draw(QPainter *painter) override;

protected:
    // 对应QQuickItem::geometryChanged
    void geometryChanged(const QRect &newRect) override;

private:
    // 坐标轴在上下左右不同位置时，绘制不同的效果
    void drawLeft(QPainter *painter);
    void drawRight(QPainter *painter);
    void drawTop(QPainter *painter);
    void drawBottom(QPainter *painter);
    // 大小or范围等变动后重新计算刻度信息
    void calcAxis();
    // 计算间隔和起点
    void calcSpace(double axisLength);
    // 计算刻度像素间隔
    double calcPxSpace(double unitP2V, double valueSpace) const;
    // 计算刻度像素起点
    double calcPxStart(double unitP2V, double valueSpace, double valueMin, double valueMax) const;
    // 计算值间隔
    double calcValueSpace(double unitP2V, int pxRefSpace) const;
    double calcValueSpaceHelper(double valueRefRange, int dividend) const;
    // 刻度值的小数位数
    int getTickPrecision() const;
    int getTickPrecisionHelper(double valueSpace, double compare, int precision) const;
    // 步进
    double valueCalcStep() const;
    double valueZoomInStep() const;
    double valueZoomOutStep() const;
    // 根据pos计算zoom的左右/上下百分比
    double calcZoomProportionWithPos(const QPoint &pos) const;

signals:
    void calcModeChanged();
    void decimalPrecisionChanged();
    void fixedValueSpaceChanged();
    void refPixelSpaceChanged();
    void minLimitChanged();
    void maxLimitChanged();
    void minRangeChanged();
    void minValueChanged();
    void maxValueChanged();

public slots:
    // 移动
    void addMinValue();
    void subMinValue();
    void addMaxValue();
    void subMaxValue();
    bool moveValueWidthPx(int px);
    // 放大缩小
    void zoomValueIn();
    void zoomValueOut();
    void zoomValueInPos(const QPoint &pos);
    void zoomValueOutPos(const QPoint &pos);
    // 全览，value设置为limit
    void overallView();
    // 设置刻度limit范围
    void setLimitRange(double min, double max, double range);
    // 设置刻度当前value显示范围
    void setValueRange(double min, double max);

private:
    // 刻度计算方式，默认根据参考px计算
    AVValueAxis::CalcMode calcMode{AVValueAxis::RefPixel};
    //显示的小数位数
    int decimalPrecision{3};
    //刻度根据固定值间隔时的参考
    double fixedValueSpace{100.0};
    //刻度根据像素间隔的参考
    //通过参考像素间隔计算得到值间隔，再取整后转换为像素间隔
    double refPixelSpace{35.0};

    //刻度值限定范围
    double minLimit{0.0};
    double maxLimit{1000.0};
    //最小缩放范围
    double minRange{10.0};
    //当前显示范围
    double minValue{0.0};
    double maxValue{1000.0};

    //1像素表示的值
    double unit1PxToValue{1.0};
    //1单位值表示的像素
    double unit1ValueToPx{1.0};
    //刻度绘制像素起点
    //横向以左侧开始，竖向以底部开始
    double pxStart{0.0};
    //刻度像素间隔
    double pxSpace{30.0};
    //刻度值间隔
    double valueSpace{1.0};
    //刻度像素位置
    QVector<int> tickPos;
    //刻度值文本
    QVector<QString> tickLabel;
};
