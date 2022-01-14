import QtQuick 2.12
import QtQml 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

//测试例程
Window {
    id: root
    visible: true
    width: 960
    height: 640
    title: qsTr("QML Audio View (by: GongJianBo 1992)")
    color: "white"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Rectangle {
            id: header
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            z: 2
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#777777" }
                GradientStop { position: 0.50; color: "#666666" }
                GradientStop { position: 0.51; color: "#555555" }
                GradientStop { position: 1.0; color: "#333333" }
            }
            ButtonGroup {
                id: view_btn_group
                buttons: [btn_audio_view, btn_test_view, btn_simple_audio]
            }
            Row {
                x: 10
                spacing: 10
                height: 30

                Row {
                    spacing: 1
                    anchors.verticalCenter: parent.verticalCenter
                    MyButton {
                        id: btn_audio_view
                        checkable: true
                        text: "[Audio View Demo]"
                        normalBgColor: "transparent"
                        radius: 0
                        onClicked: {
                            root_loader.source = "qrc:/AudioViewDemo.qml"
                        }
                    }

                    MyButton {
                        id: btn_test_view
                        checkable: true
                        text: "[Simple View Test]"
                        normalBgColor: "transparent"
                        radius: 0
                        onClicked: {
                            root_loader.source = "qrc:/SimpleViewDemo.qml"
                        }
                    }

                    MyButton {
                        id: btn_simple_audio
                        checkable: true
                        text: "[Simple Audio Demo]"
                        normalBgColor: "transparent"
                        radius: 0
                        onClicked: {
                            root_loader.source = "qrc:/SimpleAudioDemo.qml"
                        }
                    }
                }

                Rectangle {
                    width: 2
                    height: 20
                    color: "#EEEEEE"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            Rectangle {
                width: parent.width
                height: 2
                anchors.bottom: parent.bottom
                color: "#2090FF"
            }
        }
        Rectangle {
            id: content
            Layout.fillHeight: true
            Layout.fillWidth: true
            z: 1
            color: "#EEEEEE"
            Loader {
                id: root_loader
                anchors.fill: parent
                //source: "qrc:/AudioViewDemo.qml"
            }
        }
        Rectangle {
            id: footer
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            z: 2
            color: "#555555"
        }
    }

    Component.onCompleted: {
        view_btn_group.buttons[0].checked = true;
        view_btn_group.checkedButton.clicked();
    }
}
