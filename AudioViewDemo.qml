import QtQuick 2.12
import QtQuick.Controls 2.12
import AudioView 1.0

//音频组件的演示，施工中
//龚建波 2022-01-03
Rectangle {
    id: control

    AVXYView {
        id: xy_view
        anchors.fill: parent
        anchors.margins: 20
        layout{
            leftPadding: 10
            bottomPadding: 10
            topPadding: 10
            rightPadding: 10
        }

        AVWrapper {
            position: AVGlobal.PosLeft
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVValueAxis {
                implicitWidth: 30
                position: AVGlobal.PosLeft
            }
            AVValueAxis {
                id: v1
                implicitWidth: 50
                position: AVGlobal.PosLeft
            }
        }

        AVWrapper {
            position: AVGlobal.PosRight
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVValueAxis {
                implicitWidth: 20
                position: AVGlobal.PosRight
            }
            AVValueAxis {
                id: v2
                implicitWidth: 30
                position: AVGlobal.PosRight
            }
        }

        AVWrapper {
            position: AVGlobal.PosTop
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVValueAxis {
                implicitHeight: 30
                position: AVGlobal.PosTop
            }
            AVValueAxis {
                implicitHeight: 50
                position: AVGlobal.PosTop
            }
        }

        AVWrapper {
            position: AVGlobal.PosBottom
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVValueAxis {
                id: v3
                implicitHeight: 20
                position: AVGlobal.PosBottom
            }
            AVValueAxis {
                id: v4
                implicitHeight: 30
                position: AVGlobal.PosBottom
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            v1.visibility = !v1.visibility;
            v2.visibility = !v2.visibility;
            v3.visibility = !v3.visibility;
            v4.visibility = !v4.visibility;
        }
    }
}
