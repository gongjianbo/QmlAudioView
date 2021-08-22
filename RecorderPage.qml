import QtQuick 2.12
import QtQuick.Controls 2.12
import Gt.Component 2.0

//测试录音组件
Item {
    id: control

    AudioRecorderView {
        id: recorder
        anchors.fill: parent
        anchors.topMargin: btn_area.height+10
        radius: 4
        onLoadFileFinished: {
            console.log("load file finished",result);
        }
        onSaveFileFinished: {
            console.log("save file finished",result);
        }
        onRequestSelectTempSlice: {
            select_menu.x=pos.x;
            select_menu.y=pos.y;
            select_menu.open();
        }
        onRequestUnselectSlice: {
            unselect_menu.x=pos.x;
            unselect_menu.y=pos.y;
            unselect_menu.idx=sliceIndex;
            unselect_menu.open();
        }

        Menu {
            id: select_menu
            MenuItem {
                text: "选中"
                onTriggered: {
                    recorder.selectTempSlice();
                }
            }
        }
        Menu {
            id: unselect_menu
            property int idx: 0
            MenuItem {
                text: "取消选中"
                onTriggered: {
                    recorder.unselectSlice(unselect_menu.idx);
                }
            }
        }
    }

    Column {
        id: btn_area
        spacing: 10
        Row {
            spacing: 10

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: " 输入设备:"
            }
            //测试时插了耳机笔记本就默认只有耳机的输入输出，没法测试多个io
            ComboBox {
                id: input_comb
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                width: 250
                model: recorder.deviceInfo.inputDeviceNames
                //可以保留点弹框刷新设备信息
                popup.onAboutToShow: {
                    recorder.deviceInfo.updateDeviceInfos();
                }
                onModelChanged: {
                    console.log("input device name list changed")
                    let index=find(recorder.deviceInfo.currentInputName);
                    if(index>-1){
                        //如果在播放或录制则终止，并弹框提示
                        currentIndex=index;
                    }else{
                        //正在使用的输入设备变更
                        let is_record=(recorder.recordState===AudioRecorder.Recording);
                        //如果为录制状态则弹出save对话框
                        if(!is_record)
                            return;
                        recorder.stop();
                        //根据对话框操作进行后续
                        //save_dialog.open();
                    }
                }
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: " 输出设备:"
            }
            ComboBox {
                id: output_comb
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                width: 250
                model: recorder.deviceInfo.outputDeviceNames
                //popup.onAboutToShow: {
                //    recorder.deviceInfo.updateDeviceInfos();
                //}
                onModelChanged: {
                    console.log("output device name list changed")
                    let index=find(recorder.deviceInfo.currentOutputName);
                    if(index>-1){
                        //如果在播放或录制则终止，并弹框提示
                        currentIndex=index;
                    }
                }
            }
        }

        Row {
            spacing: 10

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: " 录制采样率Hz:"
            }
            ComboBox {
                id: samplerate_comb
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                width: 110
                model: [8000,16000] //要支持更高的采样率待更新绘制逻辑
                currentIndex: 1
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: " 采样精度bit:"
            }
            ComboBox {
                id: samplesize_comb
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                width: 80
                model: [8,16]
                currentIndex: 1
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: " 通道数:"
            }
            ComboBox {
                id: channelcount_comb
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                width: 80
                model: [1,2]
                currentIndex: 0
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "[duration]:"+recorder.durationString
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "[position]:"+recorder.positionString
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "[state]:"+recorder.recordState
            }
        }

        Row {
            spacing: 10
            MyButton {
                enabled: (recorder.recordState!==AudioRecorder.Playing)&&
                         (recorder.recordState!==AudioRecorder.PlayPaused)
                property bool onRecording: (recorder.recordState===AudioRecorder.Recording)
                text: {
                    switch(recorder.recordState)
                    {
                    case AudioRecorder.Recording: return "暂停";
                    case AudioRecorder.RecordPaused: return "录制";
                    }
                    return "录制";
                }

                onClicked: {
                    if(onRecording){
                        recorder.suspendRecord();
                    }else{
                        //如果没有选择框，用默认的就reset deviceinfo
                        //如果未选择正确的输入设备则弹框
                        if(input_comb.currentIndex<0)
                            return;
                        recorder.deviceInfo.setCurrentInputIndex(input_comb.currentIndex);
                        //设置了deviceInfo没必要填record的device参数
                        //recorder.record(16000,input_comb.currentText);
                        let sample_rate=Number.parseInt(samplerate_comb.currentText);
                        let sample_size=Number.parseInt(samplesize_comb.currentText);
                        let channel_count=Number.parseInt(channelcount_comb.currentText);
                        recorder.record(sample_rate,sample_size,channel_count);
                    }
                }
            }
            MyButton {
                enabled: (recorder.recordState!==AudioRecorder.Stopped)
                text: "停止"
                onClicked: {
                    let is_record=(recorder.recordState===AudioRecorder.Recording);
                    recorder.stop();
                    //如果为录制状态则弹出save对话框
                    if(!is_record)
                        return;
                    //根据对话框操作进行后续
                    //save_dialog.open();
                }
            }
            MyButton {
                id: btn_play
                enabled: (recorder.recordState!==AudioRecorder.Recording)&&
                         (recorder.recordState!==AudioRecorder.RecordPaused)&&
                         recorder.hasData
                property bool onPlaying: (recorder.recordState===AudioRecorder.Playing)
                text: {
                    switch(recorder.recordState)
                    {
                    case AudioRecorder.Playing: return "暂停";
                    case AudioRecorder.PlayPaused: return "播放";
                    }
                    return "播放";
                }
                onClicked: {
                    if(onPlaying){
                        recorder.suspendPlay();
                    }else{
                        //如果没有选择框，用默认的就reset deviceinfo
                        //如果未选择正确的输出设备则弹框
                        if(output_comb.currentIndex<0)
                            return;
                        recorder.deviceInfo.setCurrentOutputIndex(output_comb.currentIndex);
                        //设置了deviceInfo没必要填play的device参数
                        //recorder.play(output_comb.currentText);
                        recorder.play();
                    }
                }

                //把按钮空格响应去掉后可以关联快捷键
                //Shortcut{
                //    autoRepeat: false
                //    enabled: visible
                //    sequence: "space"
                //    onActivated: {
                //        //空格键播放
                //        if((recorder.recordState!==AudioRecorder.Record))
                //            btn_play.clicked();
                //    }
                //}
            }
            Item {
                width: 20
                height: 20
            }
            MyButton {
                enabled: (recorder.recordState===AudioRecorder.Stopped)&&recorder.hasData
                text: "保存文件"
                onClicked: recorder.saveToFile("./save.wav")
            }
            MyButton {
                enabled: (recorder.recordState===AudioRecorder.Stopped)
                text: "加载文件"
                onClicked: recorder.loadFromFile("./save.wav")
            }
            Item {
                width: 20
                height: 20
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: "[选区个数]:"+recorder.selectCount
            }
        }
    }
}
