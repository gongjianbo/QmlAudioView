#include "AudioRecorderView.h"

#include <cmath>

#include <QCoreApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QDebug>

AudioRecorderView::AudioRecorderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    init();
    //默认的缓存目录
    setCacheDir(qApp->applicationDirPath()+"/AppData/Default/AudioRecorder");

    //不可见后stop
    connect(this,&AudioRecorderView::visibleChanged,this,[this]{
        if(!isVisible()){
            stop(); //如果是loader切换不需要这个，也可以写在ui里
        }
    });
}

AudioRecorderView::~AudioRecorderView()
{
    free();
}

void AudioRecorderView::setRecordState(AudioRecorder::RecordState state)
{
    if(recordState!=state)
    {
        const AudioRecorder::RecordState old_state=getRecordState();
        recordState=state;
        //录制完成后展示全谱，这里不用单独判断录制暂停
        if(old_state==AudioRecorder::Recording){
            //暂停恢复录制也会设置定时器状态，但是没冲突
            recordTimer.stop();
            updateDataSample();
        }else if(state==AudioRecorder::Stopped){
            //结束后更新状态并绘制
            updateDataSample();
        }
        emit recordStateChanged();
        refresh();
    }
}

void AudioRecorderView::setDisplayMode(AudioRecorder::DisplayMode mode)
{
    if(displayMode!=mode){
        displayMode=mode;
        emit displayModeChanged();
        refresh();
    }
}

void AudioRecorderView::setCacheDir(const QString &dir)
{
    if(cacheDir!=dir){
        cacheDir=dir;
        emit cacheDirChanged(dir);
    }
}

void AudioRecorderView::setDuration(qint64 duration)
{
    if(audioDuration!=duration){
        audioDuration=duration;
        emit durationChanged();
    }
}

QString AudioRecorderView::getDurationString() const
{
    return QTime(0,0).addMSecs(audioDuration).toString("hh:mm:ss");
}

void AudioRecorderView::setPosition(qint64 position)
{
    if(audioPostion!=position){
        audioPostion=position;
        emit positionChanged();
    }
}

QString AudioRecorderView::getPositionString() const
{
    return QTime(0,0).addMSecs(audioPostion).toString("hh:mm:ss");
}

void AudioRecorderView::setAudioCursor(qint64 cursor)
{
    if(audioCursor!=cursor){
        audioCursor=cursor;
        if(audioCursor<0)
            audioCursor=0;
        refresh();
    }
}

bool AudioRecorderView::getHasData() const
{
    return hasData;
}

void AudioRecorderView::setHasData(bool has)
{
    if(hasData!=has){
        hasData=has;
        emit hasDataChanged();
    }
}

void AudioRecorderView::stop()
{
    //预置状态，待operate更新后再同步
    setRecordState(AudioRecorder::Stopped);
    emit requestStop();
}

void AudioRecorderView::play(const QString &deviceName)
{
    //当前暂停则恢复播放
    if(getRecordState()==AudioRecorder::PlayPaused){
        resumePlay();
        return;
    }
    //先判断暂停继续，非暂停再stop后播放，无数据则stop
    setRecordState(AudioRecorder::Playing);
    const QAudioDeviceInfo device_info=deviceInfo.getOutputInfo(deviceName);
    emit requestPlay(getAudioCursor()>0?getAudioCursor():0,device_info);
}

void AudioRecorderView::suspendPlay()
{
    setRecordState(AudioRecorder::PlayPaused);
    emit requestSuspendPlay();
}

void AudioRecorderView::resumePlay()
{
    setRecordState(AudioRecorder::Playing);
    emit requestResumePlay();
}

void AudioRecorderView::record(int sampleRate, int sampleSize, int channelCount,
                               const QString &deviceName)
{
    //当前暂停则恢复录制
    if(getRecordState()==AudioRecorder::RecordPaused){
        resumeRecord();
        return;
    }
    //先stop当前操作、清空数据，再开始录制操作
    clearData();
    QAudioFormat format=audioFormat;
    format.setSampleRate(sampleRate);
    format.setSampleSize(sampleSize);
    format.setChannelCount(channelCount);
    setAudioFormat(format);

    //预置状态，待operate更新后再同步
    setRecordState(AudioRecorder::Recording);

    const QAudioDeviceInfo device_info=deviceInfo.getInputInfo(deviceName);
    emit requestRecord(device_info,audioFormat);
}

void AudioRecorderView::suspendRecord()
{
    setRecordState(AudioRecorder::RecordPaused);
    emit requestSuspendRecord();
}

void AudioRecorderView::resumeRecord()
{
    setRecordState(AudioRecorder::Recording);
    emit requestResumeRecord();
}

void AudioRecorderView::loadFromFile(const QString &filepath)
{
    clearData();
    setRecordState(AudioRecorder::Stopped);
    emit requestLoadFile(filepath);
}

void AudioRecorderView::saveToFile(const QString &filepath)
{
    setRecordState(AudioRecorder::Stopped);
    emit requestSaveFile(filepath);
}

QString AudioRecorderView::saveToCache(const QString &uuid)
{
    const QString file_path=QString("%1/%2.wav").arg(getCacheDir()).arg(uuid);
    saveToFile(file_path);
    return file_path;
}

void AudioRecorderView::paint(QPainter *painter)
{
    painter->setCompositionMode(QPainter::CompositionMode_Source);
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

    //网格-横线
    painter->setPen(cursorColor);
    painter->translate(wave_x,wave_y);
    painter->drawLine(0,0,view_width,0);
    int y_px=0;
    painter->setPen(gridColor);
    qint64 y_range=audioFormat.sampleSize()==16?0x10000:0x100;
    for(int i=yValueSpace;i<y_range/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        painter->drawLine(0,y_px,view_width,y_px);
        painter->drawLine(0,-y_px,view_width,-y_px);
    }
    painter->translate(-wave_x,-wave_y);
    //网格-横线
    painter->translate(leftPadding,topPadding);
    int x_px=0;
    for(qint64 i=-xTimeBegin%xValueSpace;i<=xTimeEnd;i+=xValueSpace)
    {
        x_px=i*x1ValueToPx;
        if(x_px<0) continue;
        painter->drawLine(x_px,0,x_px,view_height);
    }
    painter->translate(-leftPadding,-topPadding);

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
        const int play_pos=double(getAudioCursor())/audioData.count()*view_width+leftPadding;
        painter->drawLine(play_pos,topPadding,
                          play_pos,height()-bottomPadding);
    }

    //纵轴幅度
    painter->translate(wave_x,wave_y);
    QString y_text;
    painter->setPen(textColor);
    painter->drawText(-5-painter->fontMetrics().width("0"),painter->fontMetrics().height()/2,"0");
    for(int i=yValueSpace;i<y_range/2;i+=yValueSpace)
    {
        y_px=i*y1ValueToPx;
        y_text=QString::number(i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          -y_px+painter->fontMetrics().height()/2,
                          y_text);
        y_text=QString::number(-i);
        painter->drawText(-5-painter->fontMetrics().width(y_text),
                          y_px+painter->fontMetrics().height()/2,
                          y_text);
    }
    painter->translate(-wave_x,-wave_y);
    painter->setPen(axisColor);
    painter->drawLine(leftPadding,topPadding,leftPadding,topPadding+view_height);

    //横轴时间
    painter->drawLine(leftPadding,topPadding+view_height,leftPadding+view_width,topPadding+view_height);
    const QString time_end=QTime(0,0,0).addMSecs(xTimeEnd).toString("hh:mm:ss.zzz");
    const int text_y=topPadding+view_height+painter->fontMetrics().ascent()+5;
    const int end_x=view_width+leftPadding-painter->fontMetrics().width(time_end);
    painter->drawText(end_x,text_y,time_end);
    for(qint64 i=-xTimeBegin%xValueSpace;i<=xTimeEnd;i+=xValueSpace)
    {
        x_px=i*x1ValueToPx;
        if(x_px<0){
            continue;
        }else if(x_px>1&&(xTimeBegin+i+1.3*xValueSpace)>xTimeEnd){
            //>1是避免0点也不绘制
            break;
        }
        QString time_i=QTime(0,0,0).addMSecs(xTimeBegin+i).toString("hh:mm:ss.zzz");
        painter->drawText(x_px+leftPadding,text_y,time_i);
    }
}

void AudioRecorderView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry,oldGeometry);
    qint64 y_range=audioFormat.sampleSize()==16?0x10000:0x100;
    calculateYSpace(plotAreaHeight(),-y_range/2,y_range/2);
    calculateXSpace(plotAreaWidth(),xTimeBegin,xTimeEnd);
    updateDataSample();
    refresh();
}

void AudioRecorderView::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    QRect plot_rect(leftPadding,rightPadding,plotAreaWidth(),plotAreaHeight());
    if(!getHasData()||!plot_rect.contains(event->pos()))
        return;
    if(event->button()==Qt::LeftButton){
        qint64 cursor=(double)(event->x()-leftPadding)/plotAreaWidth()*audioData.size();
        cursor-=(cursor%(audioFormat.sampleSize()/8));
        //非录制状态下点击移动游标
        if(getRecordState()==AudioRecorder::Stopped||
        getRecordState()==AudioRecorder::Playing||
                getRecordState()==AudioRecorder::PlayPaused)
        {
           emit requestUpdateCursorOffset(cursor);
        }

    }else if(event->button()==Qt::RightButton){

    }
}

void AudioRecorderView::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if(!getHasData())
        return;
}

void AudioRecorderView::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    if(!getHasData())
        return;
    if(event->button()==Qt::LeftButton){

    }else if(event->button()==Qt::RightButton){

    }
}

void AudioRecorderView::init()
{
    qRegisterMetaType<QAudioDeviceInfo>("QAudioDeviceInfo");
    qRegisterMetaType<QList<QAudioDeviceInfo>>("QList<QAudioDeviceInfo>");
    qRegisterMetaType<AudioRecorder::RecordState>("AudioRecorder::RecordState");

    ioThread=new QThread(this);
    ioOperate=new AudioRecorderOperate;
    ioOperate->moveToThread(ioThread);

    connect(ioThread,&QThread::started,ioOperate,&AudioRecorderOperate::init);
    connect(ioThread,&QThread::finished,ioOperate,&QObject::deleteLater);
    //通过信号槽来调用
    connect(this,&AudioRecorderView::requestStop,ioOperate,&AudioRecorderOperate::doStop);
    connect(this,&AudioRecorderView::requestPlay,ioOperate,&AudioRecorderOperate::doPlay);
    connect(this,&AudioRecorderView::requestSuspendPlay,ioOperate,&AudioRecorderOperate::doSuspendPlay);
    connect(this,&AudioRecorderView::requestResumePlay,ioOperate,&AudioRecorderOperate::doResumePlay);
    connect(this,&AudioRecorderView::requestRecord,ioOperate,&AudioRecorderOperate::doRecord);
    connect(this,&AudioRecorderView::requestSuspendRecord,ioOperate,&AudioRecorderOperate::doSuspendRecord);
    connect(this,&AudioRecorderView::requestResumeRecord,ioOperate,&AudioRecorderOperate::doResumeRecord);
    connect(this,&AudioRecorderView::requestUpdateCursorOffset,ioOperate,&AudioRecorderOperate::doUpdateCursorOffset);

    connect(ioOperate,&AudioRecorderOperate::recordStateChanged,this,&AudioRecorderView::setRecordState);
    connect(ioOperate,&AudioRecorderOperate::dataChanged,this,&AudioRecorderView::recvData);
    connect(ioOperate,&AudioRecorderOperate::durationChanged,this,&AudioRecorderView::setDuration);
    connect(ioOperate,&AudioRecorderOperate::positionChanged,this,&AudioRecorderView::setPosition);
    connect(ioOperate,&AudioRecorderOperate::cursorChanged,this,&AudioRecorderView::setAudioCursor);

    connect(this,&AudioRecorderView::requestLoadFile,ioOperate,&AudioRecorderOperate::doLoadFile);
    connect(this,&AudioRecorderView::requestSaveFile,ioOperate,&AudioRecorderOperate::doSaveFile);
    connect(ioOperate,&AudioRecorderOperate::loadFileFinished,this,&AudioRecorderView::loadFileFinished);
    connect(ioOperate,&AudioRecorderOperate::saveFileFinished,this,&AudioRecorderView::saveFileFinished);
    connect(ioOperate,&AudioRecorderOperate::loadFileFinished,
            this,[this](const QString &filepath,const QAudioFormat &format,bool result){
        Q_UNUSED(filepath)
        Q_UNUSED(result)
        setAudioFormat(format);
    });

    ioThread->start();

    //record时定时刷新
    connect(&recordTimer,&QTimer::timeout,this,&AudioRecorderView::recordUpdate);
    //刷新间隔
    recordTimer.setInterval(30);

    //采样精度和声道数暂时默认16\1
    //默认参数可以放到全局配置
    audioFormat.setSampleRate(16000);
    audioFormat.setSampleSize(16);
    audioFormat.setChannelCount(1);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    //抽样点绘制
    sampleData.reserve(10000); //预置元素内存
}

void AudioRecorderView::free()
{
    ioThread->quit();
    ioThread->wait();
}

void AudioRecorderView::clearData()
{
    audioData.clear();
    sampleData.clear();
    setAudioCursor(0);
    setDuration(0);
    setPosition(0);
    setHasData(false);
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

    //数据字节数
    const int data_count=audioData.count();
    //采样率hz
    const int sample_rate=audioFormat.sampleRate();
    //采样字节大小，这里没有判断为0否，设置时判断
    const int sample_byte=audioFormat.sampleSize()/8;
    //通道数，这里没有判断为0否，设置时判断
    const int channel_count=audioFormat.channelCount();
    //这里之前的判断误写为了&&
    if(data_count<(sample_byte*channel_count)||
            data_count%(sample_byte*channel_count)!=0||
            (sample_byte!=1&&sample_byte!=2)||
            (channel_count!=1&&channel_count!=2))
        return;

    //1s的采样数，乘上sample_byte为1s的字节数
    const qint64 seconds_points=sample_rate*channel_count;
    //单个通道采样点总数
    //const int channel_points=data_count/sample_byte/channel_count;

    //取可见范围数据的偏移
    int point_offset=0;
    //x轴内可以绘制的点数个数，不分通道数，计算时用步进跳过另一个通道
    double show_points=0;
    //判断是否为录制状态
    const bool on_recording=(getRecordState()==AudioRecorder::Recording||
                             getRecordState()==AudioRecorder::RecordPaused);

    if(getDisplayMode()==AudioRecorder::Tracking&&on_recording){
        //单通道时Ns的范围滚动
        //最大显示采样点数计算值=[N sample]*[1 channel]*[sampleRate]
        const int range_points=6*seconds_points; //6s范围滚动
        if(recordPoints>range_points)
            point_offset=recordPoints-range_points;
        //从完整范围去掉offset部分就是要绘制的点
        show_points=recordPoints-point_offset;
        xTimeBegin=point_offset*1000.0/seconds_points;
        xTimeEnd=recordPoints*1000.0/seconds_points;
    }else{
        show_points=data_count/sample_byte;
        xTimeBegin=point_offset*1000.0/seconds_points;
        xTimeEnd=data_count/sample_byte*1000.0/seconds_points;
    }
    const int sample_count=data_count/sample_byte-point_offset;
    //qDebug()<<xTimeBegin<<xTimeEnd<<recordPoints
    //       <<point_offset<<data_count/2<<sample_count;
    if(data_count<point_offset*sample_byte||sample_count<1)
        return;
    //+offset是为了只对show_points显示部分的数据绘制
    //short对应16位精度，char对应8位精度
    const short *short_ptr=(const short*)audioData.constData()+point_offset;
    const uchar *char_ptr=(const uchar*)audioData.constData()+point_offset;
    //每一段内循环的步进
    const int sec_step=channel_count==2?2:1;
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    //TODO 双声道步进计算可能有问题，但是没有设备验证，笔记本的录音两个声道几乎一样
    int x_step=std::ceil(sample_count/(double)width())/2/sec_step*sec_step;
    if(x_step<sec_step)
        x_step=sec_step;
    else if(x_step>sample_count)
        x_step=sample_count;
    //坐标轴轴适应
    const double x_scale=(width()-leftPadding-rightPadding)/(double)show_points;
    //这里y轴因子已取反，绘制时无需取反
    //0x10000为short幅度范围
    const double y_scale=-(height()-topPadding-bottomPadding)/
            (double)(sample_byte==2?0x10000:0x100);

    int cur_max=0;
    int cur_min=0;
    int index_max=0;
    int index_min=0;
    //分段找最大最小作为该段的抽样点
    //双声道时只取了左声道
    for(int i=0;i<sample_count;i+=x_step)
    {
        //根据采样点字节大小来选择类型转换
        if(sample_byte==2){
            cur_max=short_ptr[i];
            cur_min=short_ptr[i];
            index_max=i;
            index_min=i;
            for(int j=i;j<i+x_step&&j<sample_count;j+=sec_step)
            {
                //遍历找这一段的最大最小值
                if(cur_max<short_ptr[j]){
                    cur_max=short_ptr[j];
                    index_max=j;
                }
                if(cur_min>short_ptr[j]){
                    cur_min=short_ptr[j];
                    index_min=j;
                }
            }
        }else{
            cur_max=char_ptr[i];
            cur_min=char_ptr[i];
            index_max=i;
            index_min=i;
            for(int j=i;j<i+x_step&&j<sample_count;j+=sec_step)
            {
                //遍历找这一段的最大最小值
                if(cur_max<char_ptr[j]){
                    cur_max=char_ptr[j];
                    index_max=j;
                }
                if(cur_min>char_ptr[j]){
                    cur_min=char_ptr[j];
                    index_min=j;
                }
            }
            cur_max-=0x100/2;
            cur_min-=0x100/2;
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

    calculateXSpace(plotAreaWidth(),xTimeBegin,xTimeEnd);
}

void AudioRecorderView::calculateYSpace(double yAxisLen, qint64 yMin, qint64 yMax)
{
    if(yMax-yMin>0){
        y1PxToValue=(yMax-yMin)/(yAxisLen);
        y1ValueToPx=(yAxisLen)/(yMax-yMin);
    }else{
        y1PxToValue=1;
        y1ValueToPx=1;
    }

    //计算间隔
    double space_ref=y1PxToValue*yRefPxSpace;
    if(space_ref<1)
        space_ref=1;
    yValueSpace=std::round(calculateSpaceHelper(space_ref,1));
}

void AudioRecorderView::calculateXSpace(double xAxisLen, qint64 xMin, qint64 xMax)
{
    if(xMax-xMin>0){
        x1PxToValue=(xMax-xMin)/(xAxisLen);
        x1ValueToPx=(xAxisLen)/(xMax-xMin);
    }else{
        x1PxToValue=1;
        x1ValueToPx=1;
    }

    //计算间隔
    double space_ref=x1PxToValue*xRefPxSpace;
    if(space_ref<1)
        space_ref=1;
    xValueSpace=std::round(calculateSpaceHelper(space_ref,1));
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

void AudioRecorderView::setAudioFormat(const QAudioFormat &format)
{
    //qDebug()<<"set format"<<audioFormat<<bool(audioFormat==format);
    if(audioFormat==format)
        return;
    audioFormat=format;
    qint64 y_range=audioFormat.sampleSize()==16?0x10000:0x100;
    calculateYSpace(plotAreaHeight(),-y_range/2,y_range/2);
    updateDataSample();
    refresh();
}

void AudioRecorderView::refresh()
{
    update();
}

void AudioRecorderView::recvData(const QByteArray &data)
{
    audioData.append(data);
    //增加了录制暂停状态，所以改为判断定时器未启动才启动
    if(!recordTimer.isActive()){
        setHasData(!data.isEmpty());
        //如果之前没数据，表示这是第一帧数据
        if(getDisplayMode()==AudioRecorder::Tracking&&
                getRecordState()==AudioRecorder::Recording){
            const int sample_byte=audioFormat.sampleSize()/8;
            recordTimer.start();
            recordPoints=audioData.count()/sample_byte;
            recordOffset=audioData.count()/sample_byte;
            recordElapse.restart();
        }
    }

    //之前根据recv来刷新，现在录制时定时器刷新
    //updateDataSample();
    //refresh();

    //2021-4-18
    if(getDisplayMode()==AudioRecorder::Tracking&&
            getRecordState()==AudioRecorder::Recording){
        //qDebug()<<"recv"<<QTime::currentTime();
    }else{
        updateDataSample();
        refresh();
    }
}

void AudioRecorderView::recordUpdate()
{
    //if(getDisplayMode()==AudioRecorder::Tracking&&
    //        getRecordState()==AudioRecorder::Record){
    //1s的采样数，乘上sample_byte为1s的字节数
    const double seconds_points=audioFormat.channelCount()*audioFormat.sampleRate();
    const int sample_byte=audioFormat.sampleSize()/8;
    //recordPoints为定时器计算的录制采样数，避免数据接收时间不均匀导致滚动不平滑
    //当和实际audioData采样数差值较大则强制刷新下
    recordPoints=recordOffset+seconds_points*recordElapse.elapsed()/1000;
    if(qAbs(recordPoints-audioData.count()/sample_byte)>seconds_points){
        recordOffset=audioData.count()/sample_byte;
        recordElapse.restart();
    }
    updateDataSample();
    refresh();
}
