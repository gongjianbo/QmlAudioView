#include "AudioRecorderView.h"

#include <cmath>

#include <QCoreApplication>
#include <QPainter>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QDebug>

AudioRecorderView::AudioRecorderView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    init();

    //采样精度和声道数暂时默认16\1
    audioFormat.setSampleRate(16000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    //抽样点绘制
    sampleData.reserve(10000); //预置元素内存

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
        //录制完成后展示全谱
        if(old_state==AudioRecorder::Record){
            recordTimer.stop();
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

void AudioRecorderView::record(int sampleRate, const QString &deviceName)
{
    //先stop当前操作、清空数据，再开始录制操作
    audioData.clear();
    sampleData.clear();
    audioCursor=0;
    setDuration(0);
    setPosition(0);
    setHasData(false);
    //预置状态，待operate更新后再同步
    setRecordState(AudioRecorder::Record);

    audioFormat.setSampleRate(sampleRate);
    const QAudioDeviceInfo device_info=deviceInfo.getInputInfo(deviceName);
    emit requestRecord(device_info,audioFormat);
}

void AudioRecorderView::stop()
{
    //预置状态，待operate更新后再同步
    setRecordState(AudioRecorder::Stop);
    emit requestStop();
}

void AudioRecorderView::play(const QString &deviceName)
{
    //先判断暂停继续，非暂停再stop后播放，无数据则stop
    setRecordState(AudioRecorder::Playing);
    const QAudioDeviceInfo device_info=deviceInfo.getOutputInfo(deviceName);
    emit requestPlay(device_info);
}

void AudioRecorderView::suspendPlay()
{
    setRecordState(AudioRecorder::PlayPause);
    emit requestSuspendPlay();
}

void AudioRecorderView::resumePlay()
{
    setRecordState(AudioRecorder::Playing);
    emit requestResumePlay();
}

void AudioRecorderView::loadFromFile(const QString &filepath)
{
    audioData.clear();
    sampleData.clear();
    audioCursor=0;
    setDuration(0);
    setPosition(0);
    setHasData(false);
    setRecordState(AudioRecorder::Stop);
    emit requestLoadFile(filepath);
}

void AudioRecorderView::saveToFile(const QString &filepath)
{
    setRecordState(AudioRecorder::Stop);
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
        const int play_pos=double(audioCursor)/audioData.count()*view_width+leftPadding;
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
    connect(this,&AudioRecorderView::requestRecord,ioOperate,&AudioRecorderOperate::doRecord);
    connect(this,&AudioRecorderView::requestStop,ioOperate,&AudioRecorderOperate::doStop);
    connect(this,&AudioRecorderView::requestPlay,ioOperate,&AudioRecorderOperate::doPlay);
    connect(this,&AudioRecorderView::requestSuspendPlay,ioOperate,&AudioRecorderOperate::doSuspendPlay);
    connect(this,&AudioRecorderView::requestResumePlay,ioOperate,&AudioRecorderOperate::doResumePlay);
    connect(ioOperate,&AudioRecorderOperate::recordStateChanged,this,&AudioRecorderView::setRecordState);
    connect(ioOperate,&AudioRecorderOperate::dataChanged,this,&AudioRecorderView::recvData);
    connect(ioOperate,&AudioRecorderOperate::durationChanged,this,&AudioRecorderView::setDuration);
    connect(ioOperate,&AudioRecorderOperate::positionChanged,this,&AudioRecorderView::setPosition);
    connect(ioOperate,&AudioRecorderOperate::cursorChanged,this,[this](qint64 cursor){
        if(audioCursor<cursor||cursor==0)
            audioCursor=cursor;
        refresh();
    });
    connect(this,&AudioRecorderView::requestLoadFile,ioOperate,&AudioRecorderOperate::doLoadFile);
    connect(this,&AudioRecorderView::requestSaveFile,ioOperate,&AudioRecorderOperate::doSaveFile);
    connect(ioOperate,&AudioRecorderOperate::loadFileFinished,this,&AudioRecorderView::loadFileFinished);
    connect(ioOperate,&AudioRecorderOperate::saveFileFinished,this,&AudioRecorderView::saveFileFinished);

    ioThread->start();

    //record时定时刷新
    connect(&recordTimer,&QTimer::timeout,this,&AudioRecorderView::recordUpdate);
    //刷新间隔
    recordTimer.setInterval(30);
}

void AudioRecorderView::free()
{
    ioThread->quit();
    ioThread->wait();
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
    //const point_size=(audioFormat.sampleSize()/8); //目前固定2字节
    //int point_count=data_count/2;
    //if(getDisplayMode()==AudioRecorder::Tracking&&
    //        getRecordState()==AudioRecorder::Record){
    //    //单通道时Ns的范围滚动
    //    //最大显示采样点数计算值=[N sample]*[1 channel]*[sampleRate]
    //    //const int point_max=6*1*audioFormat.sampleRate();
    //    //if(point_count>point_max)
    //    //    point_count=point_max;
    //
    //    //2021-4-18
    //}
    //if(data_count<point_count*2)
    //    return;
    //const int sample_count=point_count;
    //+offset是为了只对point_count部分的数据绘制

    //2021-4-18
    int point_offset=0; //取数据的偏移
    double scale_count=0; //x轴内可以绘制的点数个数
    if(getDisplayMode()==AudioRecorder::Tracking&&
            getRecordState()==AudioRecorder::Record){
        //单通道时Ns的范围滚动
        //最大显示采样点数计算值=[N sample]*[1 channel]*[sampleRate]
        const int point_max=6*1*audioFormat.sampleRate();
        if(recordPoints>point_max)
            point_offset=recordPoints-point_max;
        scale_count=data_count/2-point_offset;
    }else{
        scale_count=data_count/2;
    }
    const int sample_count=data_count/2-point_offset;
    if(data_count<point_offset*2||sample_count<1)
        return;
    //+offset是为了只对point_count部分的数据绘制
    const short *data_ptr=(const short*)audioData.constData()+point_offset;
    //每一段多少采样点
    //除以2是因为太稀疏了，和audition看起来不一样
    int x_step=std::ceil(sample_count/(double)width())/2;
    if(x_step<1)
        x_step=1;
    else if(x_step>sample_count)
        x_step=sample_count;
    //坐标轴轴适应
    const double x_scale=(width()-leftPadding-rightPadding)/(double)scale_count;
    //这里y轴因子已取反，绘制时无需取反
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
    update();
}

void AudioRecorderView::recvData(const QByteArray &data)
{
    audioData.append(data);
    if(!getHasData()){
        setHasData(!data.isEmpty());
        //如果之前没数据，表示这是第一帧数据
        if(getDisplayMode()==AudioRecorder::Tracking&&
                getRecordState()==AudioRecorder::Record){
            recordTimer.start();
            recordPoints=audioData.count()/2; //目前固定2字节
            recordOffset=audioData.count()/2;
            recordElapse.restart();
        }
    }

    //之前根据recv来刷新，现在录制时定时器刷新
    //updateDataSample();
    //refresh();

    //2021-4-18
    if(getDisplayMode()==AudioRecorder::Tracking&&
            getRecordState()==AudioRecorder::Record){
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
    //采样率16k 8k可以整除
    //qDebug()<<"update"<<QTime::currentTime();
    recordPoints=recordOffset+1*audioFormat.sampleRate()*recordElapse.elapsed()/1000;
    if(qAbs(recordPoints-audioData.count()/2)>audioFormat.sampleRate()){
        recordOffset=audioData.count()/2;
        recordElapse.restart();
    }
    //qDebug()<<recordPoints<<audioData.count()/2<<QTime::currentTime();
    updateDataSample();
    refresh();
}
