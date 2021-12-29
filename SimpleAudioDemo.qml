import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SimpleDemo 1.0

//简易录音组件的演示
//龚建波 2021-12-11
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
        MyButton {
            id: btn_record
            horizontalPadding: 24
            text: "录制"
            onClicked: {
                recorder.record()
            }
        }
        MyButton {
            horizontalPadding: 24
            text: "播放"
            onClicked: {
                recorder.play()
            }
        }
        MyButton {
            horizontalPadding: 24
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
        MyButton {
            horizontalPadding: 24
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
        MyButton {
            horizontalPadding: 24
            text: "读文件"
            enabled: recorder.workState === SimpleAudioRecorder.Stop
            onClicked: {
                recorder.loadFile("simple.wav")
            }
        }
        MyButton {
            horizontalPadding: 24
            text: "写文件"
            enabled: recorder.workState === SimpleAudioRecorder.Stop
            onClicked: {
                recorder.saveFile("simple.wav")
            }
        }
    }
}
