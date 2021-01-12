#include "AudioRecorderView.h"

#include <cmath>

#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QDebug>

AudioRecorderView::AudioRecorderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //刷新延时
    updateTimer.setSingleShot(true);
    connect(&updateTimer,&QTimer::timeout,this,[this]{
        update();
    });

    //不可见后stop
    connect(this,&AudioRecorderView::visibleChanged,this,[this]{
        if(!isVisible()){
            //stop();
        }
    });
}

AudioRecorderView::~AudioRecorderView()
{
    //stop();
}

void AudioRecorderView::setRecordState(AudioRecorderView::RecordState state)
{
    if(recordState!=state)
    {
        const RecordState old_state=getRecordState();
        recordState=state;
        //录制完成后展示全谱
        if(old_state==Record){
            updateDataSample();
        }
        emit recordStateChanged();
        refresh();
    }
}

void AudioRecorderView::setDisplayMode(AudioRecorderView::DisplayMode mode)
{
    if(displayMode!=mode){
        displayMode=mode;
        emit displayModeChanged();
        refresh();
    }
}

void AudioRecorderView::record(int sampleRate, const QString &deviceName)
{
    //先stop当前操作、清空数据，再开始录制操作
    audioData.clear();
    sampleData.clear();
    //预置状态，待operate更新后再同步
    setRecordState(RecordState);
}

void AudioRecorderView::stop()
{

}

void AudioRecorderView::play(const QString &deviceName)
{

}

void AudioRecorderView::suspendPlay()
{

}

void AudioRecorderView::resumePlay()
{

}

void AudioRecorderView::paint(QPainter *painter)
{
    //暂时没有封装步骤
    //series区域的宽高
    const int view_width=plotAreaWidth();
    const int view_height=plotAreaHeight();
    //series零点坐标
    const int wave_x=leftPadding;
    const int wave_y=view_height/2+topPadding;

    //背景色
    painter->setPen(Qt::NoPen);
    //painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setBrush(backgroundColor);
    painter->drawRoundedRect(0,0,width(),height(),radius,radius);
    //painter->setRenderHint(QPainter::Antialiasing,false);
    painter->setBrush(Qt::NoBrush);
    painter->fillRect(leftPadding,topPadding,view_width,view_height,viewColor);

    //网格
    painter->translate(wave_x,wave_y);
    painter->setPen(cursorColor);
    painter->drawLine(0,0,view_width,0);
    int y_px=0;
    painter->setPen(gridColor);
    for(int i=yValueSpace;i<0xFFFF/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        painter->drawLine(0,y_px,view_width,y_px);
        painter->drawLine(0,-y_px,view_width,-y_px);
    }
    painter->translate(-wave_x,-wave_y);

    //有数据时才绘制曲线
    if(!audioData.isEmpty())
    {
        //绘制波形
        painter->setPen(seriesColor);
        painter->translate(wave_x,wave_y);
        for(int i=0;i<sampleData.count()-1;i++)
        {
            const SamplePoint &cur=sampleData.at(i);
            const SamplePoint &next=sampleData.at(i+1);
            painter->drawLine(cur.x,cur.y,next.x,next.y);
        }
        painter->translate(-wave_x,-wave_y);

        //画游标
        painter->setPen(cursorColor);
        const int play_pos=double(0)/audioData.count()*view_width+leftPadding; //playCount=0
        painter->drawLine(play_pos,topPadding,
                          play_pos,height()-bottomPadding);
    }

    //纵轴幅度
    painter->translate(wave_x,wave_y);
    QString y_text;
    painter->setPen(textColor);
    painter->drawText(-5-painter->fontMetrics().width("0"),painter->fontMetrics().height()/2,"0");
    for(int i=yValueSpace;i<0xFFFF/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        y_text=QString::number(i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          y_px+painter->fontMetrics().height()/2,
                          y_text);
        y_text=QString::number(-i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          -y_px+painter->fontMetrics().height()/2,
                          y_text);
    }
    painter->translate(-wave_x,-wave_y);
    painter->setPen(axisColor);
    painter->drawLine(leftPadding,topPadding,leftPadding,topPadding+view_height);

    //横轴时间，略
    painter->drawLine(leftPadding,topPadding+view_height,leftPadding+view_width,topPadding+view_height);
}

void AudioRecorderView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry,oldGeometry);
    updateDataSample();
    calculateSpace(plotAreaHeight());
    refresh();
}

int AudioRecorderView::plotAreaWidth() const
{
    return (width()-leftPadding-rightPadding);
}

int AudioRecorderView::plotAreaHeight() const
{
    return (height()-topPadding-bottomPadding);
}

void AudioRecorderView::updateDataSample()
{
    sampleData.clear();

    const int data_count=audioData.count();
    if(data_count<2&&data_count%2!=0)
        return;

    //根据模式切换显示的数据范围，暂时固定值
    //s*channel*sampleRate
    int data_show=data_count/2;
    if(getDisplayMode()==Tracking&&getRecordState()==Record){
        //单通道时5s的范围滚动
        const int max_show=5*1*16000;//audioInput.inputFormat.sampleRate();
        if(data_show>max_show)
            data_show=max_show;
    }
    if(data_count<data_show*2)
        return;
    const int sample_count=data_show;
    const short *data_ptr=(const short*)audioData.constData()+(data_count/2-data_show);
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step=std::ceil(sample_count/(double)width())/2;
    if(x_step<1)
        x_step=1;
    else if(x_step>sample_count)
        x_step=sample_count;
    //坐标轴轴适应
    const double x_scale=(width()-leftPadding-rightPadding)/(double)sample_count;
    const double y_scale=-(height()-topPadding-bottomPadding)/(double)0x10000;

    short cur_max=0;
    short cur_min=0;
    int index_max=0;
    int index_min=0;
    //分段找最大最小作为该段的抽样点
    for(int i=0;i<sample_count;i+=x_step)
    {
        cur_max=data_ptr[i];
        cur_min=data_ptr[i];
        index_max=i;
        index_min=i;
        for(int j=i;j<i+x_step&&j<sample_count;j++)
        {
            //遍历找这一段的最大最小值
            if(cur_max<data_ptr[j]){
                cur_max=data_ptr[j];
                index_max=j;
            }
            if(cur_min>data_ptr[j]){
                cur_min=data_ptr[j];
                index_min=j;
            }
        }
        SamplePoint pt_min{ int(index_min*x_scale),int(cur_min*y_scale) };
        SamplePoint pt_max{ int(index_max*x_scale),int(cur_max*y_scale) };
        //根据先后顺序存最大最小，相等就存一个
        if(index_max>index_min){
            sampleData<<pt_min<<pt_max;
        }else if(index_max<index_min){
            sampleData<<pt_max<<pt_min;
        }else{
            sampleData<<pt_min;
        }
    }
}

void AudioRecorderView::calculateSpace(double yAxisLen)
{
    //暂时为固定范围
    y1PxToValue=0xFFFF/(yAxisLen);
    y1ValueToPx=(yAxisLen)/0xFFFF;

    //计算间隔
    double space_ref=y1PxToValue*yRefPxSpace;
    if(space_ref<1)
        space_ref=1;
    yValueSpace=std::round(calculateSpaceHelper(space_ref,1));
}

double AudioRecorderView::calculateSpaceHelper(double valueRefRange, int dividend) const
{
    if(valueRefRange>8*dividend){
        //if(dividend>10000*100)return dividend;
        return calculateSpaceHelper(valueRefRange,dividend*10);
    }else if(valueRefRange>4.5*dividend){
        return 5*dividend;
    }else if(valueRefRange>3*dividend){
        return 4*dividend;
    }else if(valueRefRange>1.5*dividend){
        return 2*dividend;
    }else{
        return dividend;
    }
}

void AudioRecorderView::refresh()
{
    //大于xx ms立即刷新，否则定时器刷新
    if(updateElapse.elapsed()>30){
        updateTimer.stop();
        update();
        updateElapse.restart();
    }else{
        //未结束则重新start
        updateTimer.start(30);
    }
}
