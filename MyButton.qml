import QtQuick 2.12
import QtQuick.Controls 2.12

Button {
    id: control

    font{
        pixelSize: 14
        family: "Microsoft YaHei"
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 90
        implicitHeight: 36
        radius: 4
        border.color: Qt.darker(color)
        color: !control.enabled
               ?"#AAAAAA":control.pressed
                 ?"#104E8B":control.hovered
                   ?"#1C86EE":"#1874CD"
    }
}
