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

            Text {
                anchors.verticalCenter: parent.verticalCenter
                x: 10
                text: "duration(ms):" + recorder_view.source.duration
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "gray"
            radius: 4

            RecorderView {
                id: recorder_view
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
                    onClicked: {
                        recorder_view.record()
                    }
                }

                Button {
                    text: "play"
                    onClicked: {
                        recorder_view.play()
                    }
                }

                Button {
                    text: "suspend"
                    onClicked: {
                        if(recorder_view.workState == ARSpace.Recording){
                            recorder_view.suspendRecord()
                        }else if(recorder_view.workState == ARSpace.Playing){
                            recorder_view.suspendPlay()
                        }
                    }
                }

                Button {
                    text: "stop"
                    onClicked: {
                        recorder_view.stop()
                    }
                }

                Button {
                    text: "save"
                    onClicked: {
                        recorder_view.saveToFile("temp.wav")
                    }
                }

                Button {
                    text: "load"
                    onClicked: {
                        recorder_view.loadFromFile("temp.wav")
                    }
                }
            }//end button row
        }
    }//end row layout
}
