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

        AVWrapper {
            position: AVGlobal.PosLeft
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVWrapper {
                implicitWidth: 30
            }
            AVWrapper {
                id: v1
                implicitWidth: 50
            }
        }

        AVWrapper {
            position: AVGlobal.PosRight
            direction: AVGlobal.LeftToRight
            spacing: 10
            AVWrapper {
                implicitWidth: 20
            }
            AVWrapper {
                id: v2
                implicitWidth: 30
            }
        }

        AVWrapper {
            position: AVGlobal.PosTop
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVWrapper {
                implicitHeight: 30
            }
            AVWrapper {
                implicitHeight: 50
            }
        }

        AVWrapper {
            position: AVGlobal.PosBottom
            direction: AVGlobal.TopToBottom
            spacing: 10
            AVWrapper {
                id: v3
                implicitHeight: 20
            }
            AVWrapper {
                id: v4
                implicitHeight: 30
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
