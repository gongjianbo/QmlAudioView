#include "AVValueAxis.h"
#include <QDebug>

AVValueAxis::AVValueAxis(QObject *parent)
    : AVAbstractAxis(parent)
{

}

AVValueAxis::CalcMode AVValueAxis::getCalcMode() const
{
    return calcMode;
}

void AVValueAxis::setCalcMode(AVValueAxis::CalcMode mode)
{
    if (calcMode != mode) {
        calcMode = mode;
        emit calcModeChanged();
        calcAxis();
    }
}

int AVValueAxis::getDecimalPrecision() const
{
    return decimalPrecision;
}

void AVValueAxis::setDecimalPrecision(int precison)
{
    if (decimalPrecision != precison) {
        decimalPrecision = precison;
        emit decimalPrecisionChanged();
        emit layerChanged();
    }
}

double AVValueAxis::getFixedValueSpace() const
{
    return fixedValueSpace;
}

void AVValueAxis::setFixedValueSpace(double value)
{
    fixedValueSpace = value;
    emit fixedValueSpaceChanged();
    if (getCalcMode() == AVValueAxis::FixedValue) {
        calcAxis();
    }
}

int AVValueAxis::getRefPixelSpace() const
{
    return refPixelSpace;
}

void AVValueAxis::setRefPixelSpace(int pixel)
{
    refPixelSpace = pixel;
    emit refPixelSpaceChanged();
    if (getCalcMode() == AVValueAxis::RefPixel) {
        calcAxis();
    }
}

double AVValueAxis::getMinLimit() const
{
    return minLimit;
}

void AVValueAxis::setMinLimit(double limit)
{
    changeMinLimit(limit);
    emit layerChanged();
}

void AVValueAxis::changeMinLimit(double limit)
{
    minLimit = limit;
    emit minLimitChanged();
}

double AVValueAxis::getMaxLimit() const
{
    return maxLimit;
}

void AVValueAxis::setMaxLimit(double limit)
{
    changeMaxLimit(limit);
    emit layerChanged();
}

void AVValueAxis::changeMaxLimit(double limit)
{
    maxLimit = limit;
    emit maxLimitChanged();
}

double AVValueAxis::getMinRange() const
{
    return minRange;
}

void AVValueAxis::setMinRange(double limit)
{
    changeMinRange(limit);
    emit layerChanged();
}

void AVValueAxis::changeMinRange(double limit)
{
    minRange = limit;
    emit minRangeChanged();
}

double AVValueAxis::getMinValue() const
{
    return minValue;
}

void AVValueAxis::setMinValue(double value)
{
    changeMinValue(value);
    calcAxis();
}

void AVValueAxis::changeMinValue(double value)
{
    minValue = value;
    emit minValueChanged();
}

double AVValueAxis::getMaxValue() const
{
    return maxValue;
}

void AVValueAxis::setMaxValue(double value)
{
    changeMaxValue(value);
    calcAxis();
}

void AVValueAxis::changeMaxValue(double value)
{
    maxValue = value;
    emit maxValueChanged();
}

double AVValueAxis::getUnit1PxToValue() const
{
    return unit1PxToValue;
}

double AVValueAxis::getUnit1ValueToPx() const
{
    return unit1ValueToPx;
}

double AVValueAxis::pxToValue(double px) const
{
    return px * unit1PxToValue + minValue;
}

double AVValueAxis::valueToPx(double value) const
{
    return (value - minValue) * unit1ValueToPx;
}

void AVValueAxis::draw(QPainter *painter)
{
    painter->setPen(Qt::white);
    painter->fillRect(rect, QColor(0, 255, 0, 100));
    switch (this->getPosition())
    {
    case AVGlobal::PosLeft:
        drawLeft(painter);
        break;
    case AVGlobal::PosRight:
        drawRight(painter);
        break;
    case AVGlobal::PosTop:
        drawTop(painter);
        break;
    case AVGlobal::PosBottom:
        drawBottom(painter);
        break;
    default:
        break;
    }
}

void AVValueAxis::geometryChanged(const QRect &newRect)
{
    AVAbstractAxis::geometryChanged(newRect);
    calcAxis();
}

void AVValueAxis::drawLeft(QPainter *painter)
{
    painter->save();
    const int right_pos = getRect().right();
    for (int i = 0; i < tickPos.count(); i++)
    {
        const int y_pos = tickPos.at(i);
        painter->drawLine(QPoint(right_pos, y_pos),
                          QPoint(right_pos - 5, y_pos));
        painter->drawText(right_pos - 5 - painter->fontMetrics().horizontalAdvance(tickLabel.at(i)),
                          y_pos + painter->fontMetrics().ascent() / 2,
                          tickLabel.at(i));
    }

    painter->restore();
}

void AVValueAxis::drawRight(QPainter *painter)
{

}

void AVValueAxis::drawTop(QPainter *painter)
{

}

void AVValueAxis::drawBottom(QPainter *painter)
{
    painter->save();
    const int top_pos = getRect().top();
    for (int i = 0; i < tickPos.count(); i++)
    {
        const int x_pos = tickPos.at(i);
        painter->drawLine(QPoint(x_pos, top_pos),
                          QPoint(x_pos, top_pos + 5));
        painter->drawText(x_pos - painter->fontMetrics().horizontalAdvance(tickLabel.at(i)) / 2,
                          top_pos + 5 + painter->fontMetrics().height(),
                          tickLabel.at(i));
    }
    painter->restore();
}

void AVValueAxis::calcAxis()
{
    if (minLimit >= maxLimit || getRect().isNull())
        return;
    if (minValue > maxValue) {
        std::swap(minValue, maxValue);
    }
    if (minLimit > minValue) {
        changeMinValue(minLimit);
    }
    if (maxLimit < maxValue) {
        changeMaxValue(maxLimit);
    }
    switch (this->getPosition())
    {
    case AVGlobal::PosLeft:
    case AVGlobal::PosRight:
    {
        //竖向y轴
        calcSpace(getRect().height() - 1);
        //计算刻度线
        const double top_pos = getRect().top();
        tickPos.clear();
        tickLabel.clear();
        const int precision = getTickPrecision();
        //i是用刻度px算坐标位置；j是用刻度px算i对应的value
        //条件i>pos-N是为了显示最大值那个刻度
        for (double i = getRect().bottom() - pxStart, j = pxStart; i > top_pos - 2; i -= pxSpace, j += pxSpace)
        {
            tickPos.push_back(std::round(i));
            const double label_value = (minValue + (j)*unit1PxToValue);
            QString label_text = QString::number(label_value, 'f', precision);
            if (label_text == "-0")
            { //会有-0
                label_text = "0";
            }
            tickLabel.push_back(label_text);
        }
    }
        break;
    case AVGlobal::PosTop:
    case AVGlobal::PosBottom:
    {
        //横向x轴
        calcSpace(getRect().width() - 1);
        //计算刻度线
        const double right_pos = getRect().right();
        tickPos.clear();
        tickLabel.clear();
        const int precision = getTickPrecision();
        //i是用刻度px算坐标位置；j是用刻度px算i对应的value
        //条件i>pos-N是为了显示最大值那个刻度
        for (double i = getRect().left() + pxStart, j = pxStart; i < right_pos + 2; i += pxSpace, j += pxSpace)
        {
            tickPos.push_back(std::round(i));
            const double label_value = (minValue + (j)*unit1PxToValue);
            QString label_text = QString::number(label_value, 'f', precision);
            if (label_text == "-0")
            { //会有-0
                label_text = "0";
            }
            tickLabel.push_back(label_text);
        }
    }
        break;
    default:
        break;
    }
    emit layerChanged();
}

void AVValueAxis::calcSpace(double axisLength)
{
    //计算每单位值
    //为什么算了两个互为倒数的数呢？因为浮点数精度问题
    unit1PxToValue = (maxValue - minValue) / (axisLength);
    unit1ValueToPx = (axisLength) / (maxValue - minValue);
    //计算间隔和起点
    //计算刻度间隔及刻度起点
    switch (getCalcMode())
    {
    case FixedValue:
        //该模式ValueSpace固定不变;
        valueSpace = fixedValueSpace;
        pxSpace = calcPxSpace(unit1PxToValue, valueSpace);
        pxStart = calcPxStart(unit1PxToValue, valueSpace, minValue, maxValue);
        break;
    case RefPixel:
        valueSpace = calcValueSpace(unit1PxToValue, refPixelSpace);
        pxSpace = calcPxSpace(unit1PxToValue, valueSpace);
        pxStart = calcPxStart(unit1PxToValue, valueSpace, minValue, maxValue);
        break;
    default:
        break;
    }
}

double AVValueAxis::calcPxSpace(double unitP2V, double valueSpace) const
{
    //这里与真0.0比较
    if (unitP2V <= 0.0) {
        qWarning() << __FUNCTION__ << "unitP2V is too min" << unitP2V;
        return 30.0;
    }
    return valueSpace / unitP2V;
}

double AVValueAxis::calcPxStart(double unitP2V, double valueSpace, double valueMin, double valueMax) const
{
    Q_UNUSED(valueMax)
    if (unitP2V <= 0.0 || valueSpace <= 0.0) {
        qWarning() << __FUNCTION__ << "unitP2V or valueSpace is too min" << unitP2V << valueSpace;
        return 0.0;
    }
    //min有正负，而unit和space只有正
    //如果最小值为正数or零
    //从最小值往上找第一个能被value_space整除的数
    //如果最小值为负数
    //从0往下找最后一个能被value_space整除的数
    //（如果min绝对值小于value_space则起点为0）
    //即起点值应该是value_space的整倍数
    const double begin_precision = std::pow(10, decimalPrecision);
    const double begin_cut = (decimalPrecision <= 0)
            ? 0
            : qRound(std::abs(valueMin) * begin_precision) % qRound(valueSpace * begin_precision) / begin_precision;
    //因为cut是value_space模出来的，且该分支min和value_space都为正，
    //所以起始值(value_space-cut)不会为负。
    //起点px就为起始值*单位值表示的像素；或者为起始值/单位像素表示的值
    //(注意：起始值是距离起点的间隔值)
    const double begin_val = qFuzzyIsNull(begin_cut) ? 0.0 : (valueMin >= 0.0) ? (valueSpace - begin_cut)
                                                                               : begin_cut;
    return begin_val / unitP2V;

    //之前以左上角为起始计算的逻辑，会导致左下角xy的零点相交误差大，现在改为左下角开始算
    //if(getAxisPosition()==AtTop||getAxisPosition()==AtBottom){
    //    //横向刻度值的是从左至右，和坐标x值增长方向一样
    //    return begin_val/unitP2V;
    //}else if(getAxisPosition()==AtLeft||getAxisPosition()==AtRight){
    //    //竖向如果从上往下开始计算，则刻度值和坐标y值增长方向相反
    //    const double end_val=(valueMax-valueMin-begin_val)-valueSpace*(int)((valueMax-valueMin-begin_val)/valueSpace);
    //    return end_val/unitP2V;
    //}
    //return 0;
}

double AVValueAxis::calcValueSpace(double unitP2V, int pxRefSpace) const
{
    //尽量为整除
    const double space_ref = unitP2V * pxRefSpace;
    double space_temp = space_ref;
    if (space_ref > 1)
        space_temp = calcValueSpaceHelper(space_ref, 1);
    else
        space_temp = calcValueSpaceHelper(space_ref * std::pow(10, decimalPrecision), 1) * std::pow(10, -decimalPrecision);
    //避免过大过小
    /*if(space_temp<=std::pow(10,-_decimalPrecision)){
        return std::pow(10,-_decimalPrecision);
    }else if(space_temp<space_ref*0.7){
        return space_temp*2;
    }else if(space_temp>space_ref*1.8){
        return space_temp/2;
    }*/
    return space_temp;
}

double AVValueAxis::calcValueSpaceHelper(double valueRefRange, int dividend) const
{
    //分段找合适的间隔，分割倍数dividend每次递归乘以10
    //考虑到当前应用场景，没有处理太大or太小的数
    //其实这个递归也不是很好，如果数值较大比较费时间，但是统计数值位数也需要去递归
    if (valueRefRange > 8 * dividend) {
        //if(dividend>10000*100)return dividend;
        return calcValueSpaceHelper(valueRefRange, dividend * 10);
    } else if (valueRefRange > 4.5 * dividend) {
        return 5 * dividend;
    } else if (valueRefRange > 3 * dividend) {
        return 4 * dividend;
    } else if (valueRefRange > 1.5 * dividend) {
        return 2 * dividend;
    } else {
        return dividend;
    }

    //递归思路
    /*if(temp_value>8*x){//x=1,>8--loop
        if(temp_value>8*x(10)){ //x=10,>80--loop
        }if(temp_value>4*x(10)){ //x=10,50
        }else if(temp_value>1.5*x(10)){ //x=10,20
        }else{ //x=10,10
        }
    }else if(temp_value>4*x){ //x=1,5
    }else if(temp_value>1.5*x){ //x=1,2
    }else{ //x=1,1
        //...
    }*/
}

int AVValueAxis::getTickPrecision() const
{
    //刻度的小数位数
    return getTickPrecisionHelper(valueSpace, 1, 0);
}

int AVValueAxis::getTickPrecisionHelper(double valueSpace, double compare, int precision) const
{
    //第二个参数为小数参照，每次递归除以10再和传入的参数一间隔值比较
    //如果valueSpace大于compare，那么小数精度就是当前precision
    if (valueSpace >= compare) {
        return precision;
    }
    return getTickPrecisionHelper(valueSpace, compare / 10, precision + 1);
}

double AVValueAxis::valueCalcStep() const
{
    // add sub的步进，根据需求自定义
    switch (getCalcMode())
    {
    case RefPixel:
        return valueSpace;
        break;
    case FixedValue:
        return (maxValue - minValue) / 5;
        break;
    default:
        break;
    }
    return valueSpace;
}

double AVValueAxis::valueZoomInStep() const
{
    //zoomin 步进，根据需求自定义
    return (maxValue - minValue) / 4;
}

double AVValueAxis::valueZoomOutStep() const
{
    //zoomout 步进，根据需求自动逸
    return (maxValue - minValue) / 2;
}

double AVValueAxis::calcZoomProportionWithPos(const QPoint &pos) const
{
    //根据点在rect的位置计算百分比，通过百分比来计算左右缩放的值
    double zoom_proportion = 0.5;
    switch (this->getPosition())
    {
    case AVGlobal::PosTop:
    case AVGlobal::PosBottom:
    {
        const int pos_x = pos.x();
        const int rect_left = getRect().left();
        const int rect_right = getRect().right();
        zoom_proportion = (pos_x - rect_left) / (double)(rect_right - rect_left);
    }
        break;
    case AVGlobal::PosLeft:
    case AVGlobal::PosRight:
    {
        const int pos_y = pos.y();
        const int rect_top = getRect().top();
        const int rect_bottom = getRect().bottom();
        zoom_proportion = (rect_bottom - pos_y) / (double)(rect_bottom - rect_top);
    }
        break;
    default:
        break;
    }
    if (zoom_proportion <= 0.0)
        return 0.0;
    if (zoom_proportion >= 1.0)
        return 1.0;
    return zoom_proportion;
}

void AVValueAxis::addMinValue()
{
    //不能小于最小范围
    if (maxValue - minValue <= minRange)
        return;
    minValue += valueCalcStep();
    if (maxValue - minValue < minRange) {
        minValue = maxValue - minRange;
    }
    setMinValue(minValue);
}

void AVValueAxis::subMinValue()
{
    //不能小于最小值的limit
    if (minValue <= minLimit)
        return;
    minValue -= valueCalcStep();
    if (minValue < minLimit) {
        minValue = minLimit;
    }
    setMinValue(minValue);
}

void AVValueAxis::addMaxValue()
{
    //不能大于最大值的limit
    if (maxValue > maxLimit)
        return;
    maxValue += valueCalcStep();
    if (maxValue > maxLimit) {
        maxValue = maxLimit;
    }
    setMaxValue(maxValue);
}

void AVValueAxis::subMaxValue()
{
    //不能小于最小范围
    if (maxValue - minValue <= minRange)
        return;
    maxValue -= valueCalcStep();
    if (maxValue - minValue < minRange) {
        maxValue = minValue + minRange;
    }
    setMaxValue(maxValue);
}

bool AVValueAxis::moveValueWidthPx(int px)
{
    double move_step = qAbs(px) * unit1PxToValue;
    if (move_step <= 0)
        return false;
    // <0 就是往min端移动，>0 就是往max端移动
    if (px < 0) {
        if (minValue <= minLimit)
            return false;
        if (minValue - move_step < minLimit) {
            move_step = minValue - minLimit;
        }
        minValue -= move_step;
        maxValue -= move_step;
    } else {
        if (maxValue > maxLimit)
            return false;
        if (maxValue + move_step > maxLimit) {
            move_step = maxLimit - maxValue;
        }
        minValue += move_step;
        maxValue += move_step;
    }
    emit minValueChanged();
    emit maxValueChanged();
    calcAxis();
    return true;
}

void AVValueAxis::zoomValueIn()
{
    const double val_range = maxValue - minValue;
    if (val_range <= minRange)
        return;
    const double zoom_step = valueZoomInStep();
    if (zoom_step <= 0)
        return;
    if (val_range - zoom_step < minRange) {
        const double zoom_real_step = val_range - minRange;
        minValue += zoom_real_step / 2;
        maxValue = minValue + minRange;
    } else {
        minValue += zoom_step / 2;
        maxValue -= zoom_step / 2;
    }
    emit minValueChanged();
    emit maxValueChanged();
    calcAxis();
}

void AVValueAxis::zoomValueOut()
{
    if (minValue <= minLimit && maxValue >= maxLimit) {
        return;
    }
    const double zoom_half = valueZoomOutStep() / 2;
    const double min_zoom = (minValue - zoom_half < minLimit)
            ? (minValue - minLimit)
            : (zoom_half);
    const double max_zoom = (maxValue + zoom_half > maxLimit)
            ? (maxLimit - maxValue)
            : (zoom_half);
    //先不考虑补上不足的部分
    minValue -= min_zoom;
    maxValue += max_zoom;
    emit minValueChanged();
    emit maxValueChanged();
    calcAxis();
}

void AVValueAxis::zoomValueInPos(const QPoint &pos)
{
    const double val_range = maxValue - minValue;
    if (val_range <= minRange)
        return;
    const double zoom_step = valueZoomInStep();
    if (zoom_step <= 0)
        return;
    const double zoom_proportion = calcZoomProportionWithPos(pos);
    if (val_range - zoom_step < minRange) {
        const double zoom_real_step = val_range - minRange;
        minValue += zoom_real_step / 2;
        maxValue = minValue + minRange;
    } else {
        minValue += zoom_step * zoom_proportion;
        maxValue -= zoom_step * (1 - zoom_proportion);
    }
    emit minValueChanged();
    emit maxValueChanged();
    calcAxis();
}

void AVValueAxis::zoomValueOutPos(const QPoint &pos)
{
    if (minValue <= minLimit && maxValue >= maxLimit) {
        return;
    }
    const double zoom_proportion = calcZoomProportionWithPos(pos);
    const double zoom_step = valueZoomInStep();
    const double min_step = zoom_step * zoom_proportion;
    const double max_step = zoom_step * (1 - zoom_proportion);
    const double min_zoom = (minValue - min_step < minLimit)
            ? (minValue - minLimit)
            : (min_step);
    const double max_zoom = (maxValue + max_step > maxLimit)
            ? (maxLimit - maxValue)
            : (max_step);
    //先不考虑补上不足的部分
    minValue -= min_zoom;
    maxValue += max_zoom;
    emit minValueChanged();
    emit maxValueChanged();
    calcAxis();
}

void AVValueAxis::overallView()
{
    if (minValue <= minLimit && maxValue >= maxLimit) {
        return;
    }
    changeMinValue(minLimit);
    changeMaxValue(maxLimit);
    calcAxis();
}

void AVValueAxis::setLimitRange(double min, double max, double range)
{
    if (min >= max || max - min < range) {
        return;
    }
    changeMinLimit(min);
    changeMaxLimit(max);
    changeMinRange(range);
    emit layerChanged();
}

void AVValueAxis::setValueRange(double min, double max)
{
    if (min >= max || max - min <= minRange) {
        return;
    }
    changeMinValue(min);
    changeMaxValue(max);
    calcAxis();
}
