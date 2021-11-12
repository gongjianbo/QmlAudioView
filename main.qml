import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import AR.Component 3.0

Window {
    id: root
    visible: true
    width: 960
    height: 640
    title: qsTr("QML Audio Recorder (by: GongJianBo 1992)")


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "gray"
            radius: 4
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "gray"
            radius: 4
            RecorderView {
                anchors.fill: parent
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "gray"
            radius: 4

            Row {
                anchors.verticalCenter: parent.verticalCenter
                x: 10
                spacing: 20

                Button {
                    text: "record"
                }

                Button {
                    text: "play"
                }

                Button {
                    text: "suspend"
                }

                Button {
                    text: "stop"
                }
            }//end button row
        }
    }//end row layout
}
