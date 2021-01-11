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
        backgroundColor: "#0E306A"
        viewColor: "#13275D"
        seriesColor: "#1F8FFF"
        gridColor: "#083B95"
        cursorColor: "#FF385E"

        //output.cacheDir:
        onInputDeviceChanged: {
            //正在使用的输入设备变更
            let is_record=(oldState===AudioRecorderView.Record);
            //如果为录制状态则弹出save对话框
            if(!is_record)
                return;
            //根据对话框操作进行后续
            //save_dialog.open();
            console.log("open save dialog ... ...");
        }
    }

    Column{
        id: btn_area
        spacing: 10
        Row{
            spacing: 10

            //太挫了，插了耳机电脑就默认只有耳机的输入输出，没法测试多个io
            ComboBox{
                id: input_comb
                width: 250
                model: recorder.input.filterInputDevicesName
                popup.onAboutToShow: {
                    recorder.input.updateInputDevices();
                }
            }
            ComboBox{
                id: output_comb
                width: 250
                model: recorder.output.filterOutputDevicesName
                popup.onAboutToShow: {
                    recorder.output.updateOutputDevices();
                }
            }
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[duration]:"+recorder.durationString
            }
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[position]:"+recorder.positionString
            }
            Label{
                anchors.verticalCenter: parent.verticalCenter
                text: "[state]:"+recorder.recordState
            }
        }

        Row{
            spacing: 10
            Button{
                text: "录制"
                onClicked: {
                    if(recorder.input.isAvailableDevice(input_comb.currentText)){
                        recorder.record(16000,input_comb.currentText);
                    }else{
                        console.log("音频输入设备不可用，请重新选择");
                    }
                }
            }
            Button{
                text: "停止"
                onClicked: recorder.stop()
            }
            Button{
                text: "播放"
                onClicked: {
                    recorder.output.resetToDefaultDevice();
                    recorder.play(output_comb.currentText);
                }
            }
            Button{
                text: "暂停"
                onClicked: recorder.suspendPlay()
            }
            Button{
                text: "恢复"
                onClicked: recorder.resumePlay()
            }
            Button{
                text: "保存文件"
                onClicked: recorder.saveToFile("./save.wav")
            }
            Button{
                text: "加载文件"
                onClicked: recorder.loadFromFile("./save.wav")
            }
        }
    }
}
