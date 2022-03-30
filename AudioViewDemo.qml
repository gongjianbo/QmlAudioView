import QtQuick 2.12
import QtQuick.Controls 2.12
import AudioView 1.0

//音频组件的演示，施工中
//龚建波 2022-01-03
Rectangle {
    id: control

    //【】可视部件全部放到view中进行绘制
    AVXYView {
        id: xy_view
        anchors{
            fill: parent
            margins: 20
            bottomMargin: 70
        }
        layout{
            leftPadding: 10
            bottomPadding: 10
            topPadding: 10
            rightPadding: 10
        }
        font{
            family: "SimSun"
            pixelSize: 12
        }

        AVWrapper {
            position: AVGlobal.PosLeft
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVValueAxis {
                id: v1
                implicitWidth: 50
                position: AVGlobal.PosLeft
                refPixelSpace: 40
                bgColor: "#333333"
                minLimit: -32768
                maxLimit: 32767
                minValue: -32768
                maxValue: 32767
            }
            AVValueAxis {
                id: y_axis
                implicitWidth: 60
                position: AVGlobal.PosLeft
                refPixelSpace: 80
                bgColor: "#222222"
                minLimit: -32768
                maxLimit: 32767
                minValue: -32768
                maxValue: 32767
            }
        }

        AVWrapper {
            position: AVGlobal.PosRight
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVValueAxis {
                implicitWidth: 20
                position: AVGlobal.PosRight
                bgColor: "#222222"
            }
            AVValueAxis {
                id: v2
                implicitWidth: 30
                position: AVGlobal.PosRight
                bgColor: "#333333"
            }
        }

        AVWrapper {
            position: AVGlobal.PosTop
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVValueAxis {
                id: v3
                implicitHeight: 50
                position: AVGlobal.PosTop
                bgColor: "#333333"
            }
            AVValueAxis {
                implicitHeight: 30
                position: AVGlobal.PosTop
                bgColor: "#222222"
            }
        }

        AVWrapper {
            position: AVGlobal.PosBottom
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVValueAxis {
                id: x_axis
                implicitHeight: 30
                position: AVGlobal.PosBottom
                refPixelSpace: 100
                bgColor: "#222222"
            }
            AVValueAxis {
                id: v4
                implicitHeight: 30
                position: AVGlobal.PosBottom
                bgColor: "#333333"
            }
        }

        AVXYGrid {
            bgColor: "#112233"
            xAxis: x_axis
            yAxis: y_axis
        }

        AVAudioSeries {
            lineColor: "#00FF11"
            xAxis: x_axis
            yAxis: y_axis
            audioSource: audio_context.audioSource
        }
    }

    //【】非可视部件也可以放到外面
    AVAudioContext {
        id: audio_context
    }

    //【】底部按钮
    Row {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        spacing: 12

        MyButton {
            text: "Axis显示隐藏"
            onClicked: {
                v1.visibility = !v1.visibility;
                v2.visibility = !v2.visibility;
                v3.visibility = !v3.visibility;
                v4.visibility = !v4.visibility;
            }
        }

        MyButton {
            enabled: audio_context.state !== AVGlobal.Recording
            text: "录制"
            onClicked: {
                audio_context.record()
            }
        }

        MyButton {
            enabled: audio_context.audioSource.duration > 0 &&
                     (audio_context.state === AVGlobal.Stopped ||
                      audio_context.state === AVGlobal.PlayPaused)
            text: "播放"
            onClicked: {
                audio_context.play()
            }
        }

        MyButton {
            enabled: audio_context.state === AVGlobal.Playing ||
                     audio_context.state === AVGlobal.Recording
            text: "暂停"
            onClicked: {
                audio_context.suspend()
            }
        }

        MyButton {
            enabled: audio_context.state === AVGlobal.PlayPaused ||
                     audio_context.state === AVGlobal.RecordPaused
            text: "继续"
            onClicked: {
                audio_context.resume()
            }
        }

        MyButton {
            enabled: audio_context.state !== AVGlobal.Stopped
            text: "停止"
            onClicked: {
                audio_context.stop()
            }
        }

        MyButton {
            text: "保存文件"
            onClicked: {
                audio_context.saveFile("record.wav")
            }
        }

        MyButton {
            text: "读取文件"
            onClicked: {
                audio_context.loadFile("record.wav")
            }
        }
    }
}
