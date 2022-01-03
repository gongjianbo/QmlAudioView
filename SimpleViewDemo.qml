import QtQuick 2.12
import QtQuick.Controls 2.12
import AudioView 1.0

//音频组件的测试
//龚建波 2021-12-28
Rectangle {
    id: control

    AVSimpleView {
        id: simple_view
        anchors.fill: parent
        anchors.margins: 20
        anchors.bottomMargin: 30+40
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        spacing: 12

        MyButton {
            text: "录制"
            onClicked: {
                simple_view.record()
            }
        }

        MyButton {
            text: "播放"
            onClicked: {
                simple_view.play()
            }
        }

        MyButton {
            text: "暂停播放"
            onClicked: {
                simple_view.suspendPlay()
            }
        }

        MyButton {
            text: "继续播放"
            onClicked: {
                simple_view.resumePlay()
            }
        }

        MyButton {
            text: "结束"
            onClicked: {
                simple_view.stop()
            }
        }

        //音频时间
        Text {
            text: "duration:" + simple_view.durationString
        }

        //位置
        Text {
            text: "position:" + simple_view.positionString
        }
    }
}
