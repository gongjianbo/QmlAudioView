import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SimpleDemo 1.0

//简易录音组件的演示
Rectangle {
    id: control

    SimpleAudioRecorder {
        id: recorder
        anchors{
            fill: parent
            margins: 20
            topMargin: duration.implicitHeight + 40
            bottomMargin: btn_record.implicitHeight + 40
        }
    }

    //音频时间
    Text {
        id: duration
        anchors{
            left: parent.left
            top: parent.top
            margins: 20
        }
        text: "duration:" + recorder.durationString
    }

    //位置
    Text {
        anchors{
            right: parent.right
            top: parent.top
            margins: 20
        }
        text: "position:" + recorder.positionString
    }

    //操作按钮
    Row {
        anchors{
            left: parent.left
            bottom: parent.bottom
            margins: 20
        }
        spacing: 15
        Button {
            id: btn_record
            text: "录制"
            onClicked: {
                recorder.record()
            }
        }
        Button {
            text: "播放"
            onClicked: {
                recorder.play()
            }
        }
        Button {
            text: "暂停"
            enabled: (recorder.workState === SimpleAudioRecorder.Recording ||
                      recorder.workState === SimpleAudioRecorder.Playing)
            onClicked: {
                if(recorder.workState === SimpleAudioRecorder.Recording){
                    recorder.recordPause()
                }else if(recorder.workState === SimpleAudioRecorder.Playing){
                    recorder.playPause()
                }
            }
        }
        Button {
            text: "停止"
            enabled: recorder.workState !== SimpleAudioRecorder.Stop
            onClicked: {
                recorder.stop()
            }
        }
    }

    Row {
        anchors{
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
        spacing: 15
        Button {
            text: "读文件"
            enabled: recorder.workState === SimpleAudioRecorder.Stop
            onClicked: {
                recorder.loadFile("simple.wav")
            }
        }
        Button {
            text: "写文件"
            enabled: recorder.workState === SimpleAudioRecorder.Stop
            onClicked: {
                recorder.saveFile("simple.wav")
            }
        }
    }
}
