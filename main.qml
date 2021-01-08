import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

//测试录音组件的使用
//先录制再播放或者保存，保存后下次可以加载
Window {
    id: root
    visible: true
    width: 960
    height: 600
    title: qsTr("QML录音 (by: 龚建波 1992)")

    RecorderPage{
        anchors.fill: parent
        anchors.margins: 12
    }
}
