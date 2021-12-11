import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Window {
    id: root
    visible: true
    width: 960
    height: 640
    title: qsTr("QML Audio View (by: GongJianBo 1992)")
    color: "white"

    SimpleAudioDemo {
        anchors.fill: parent
    }
}
