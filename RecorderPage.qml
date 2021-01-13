import QtQuick 2.12
import QtQuick.Controls 2.12

import GongJianBo 1.0

//测试录音组件
Item {
    id: control

    AudioRecorderView{
        id: recorder
        anchors.fill: parent
        anchors.topMargin: btn_area.height+10
        radius: 4
    }

    Column{
        id: btn_area
        spacing: 10
        Row{
            spacing: 10

            //测试时插了耳机笔记本就默认只有耳机的输入输出，没法测试多个io
            ComboBox{
                id: input_comb
                width: 250
                model: recorder.deviceInfo.inputDeviceNames
                //可以保留点弹框刷新设备信息
                //popup.onAboutToShow: {
                //    recorder.deviceInfo.updateDeviceInfos();
                //}
                onModelChanged: {
                    console.log("input device name list changed")
                    let index=find(recorder.deviceInfo.currentInputName);
                    if(index>-1){
                        //如果在播放或录制则终止，并弹框提示
                        currentIndex=index;
                    }
                }
            }
            ComboBox{
                id: output_comb
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
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[duration]:"//+recorder.durationString
            }
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[position]:"//+recorder.positionString
            }
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[state]:"//+recorder.recordState
            }
        }

        Row{
            spacing: 10
            MyButton{
                text: "录制"
                onClicked: {
                    //如果没有选择框，用默认的就reset deviceinfo
                    //如果未选择正确的输入设备则弹框
                    if(input_comb.currentIndex<0)
                        return;
                    recorder.deviceInfo.setCurrentInputIndex(input_comb.currentIndex);

                    recorder.record(16000,input_comb.currentText);
                }
            }
            MyButton{
                text: "停止"
                onClicked: recorder.stop()
            }
            MyButton{
                text: "播放"
                onClicked: {
                    //如果没有选择框，用默认的就reset deviceinfo
                    //如果未选择正确的输出设备则弹框
                    if(output_comb.currentIndex<0)
                        return;
                    recorder.deviceInfo.setCurrentOutputIndex(output_comb.currentIndex);

                    //recorder.output.resetToDefaultDevice();
                    //recorder.play(output_comb.currentText);
                }
            }
            MyButton{
                text: "暂停"
                //onClicked: recorder.suspendPlay()
            }
            MyButton{
                text: "恢复"
                //onClicked: recorder.resumePlay()
            }
            MyButton{
                text: "保存文件"
                //onClicked: recorder.saveToFile("./save.wav")
            }
            MyButton{
                text: "加载文件"
                //onClicked: recorder.loadFromFile("./save.wav")
            }
        }
    }
}
