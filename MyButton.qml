import QtQuick 2.12
import QtQuick.Controls 2.12

//自定义按钮
//龚建波 2021-12-28
AbstractButton {
    id: control

    hoverEnabled: true
    checkable: false
    implicitWidth: implicitContentWidth+leftPadding+rightPadding
    implicitHeight: 30 //implicitContentHeight+topPadding+bottomPadding
    horizontalPadding: 12
    verticalPadding: 4

    //圆角
    property int radius: 3
    //文本颜色
    property color textColor: "#FFFFFF"
    //默认背景色
    property color normalBgColor: "#555555"
    //鼠标悬停
    property color hoverBgColor: "#00B0FF"
    //焦点
    property color focusBgColor: normalBgColor
    //选中
    property color checkedColor: pressedBgColor
    //点击
    property color pressedBgColor: "#2090FF"
    //不可选
    property color disabledBgColor: "#888888"
    property color bgColor: (!enabled)
                            ? disabledBgColor
                            : pressed
                              ? pressedBgColor
                              : hovered
                                ? hoverBgColor
                                : checked
                                  ? checkedColor
                                  : focus
                                    ? focusBgColor
                                    : normalBgColor

    Keys.onPressed: {
        event.accepted=(event.key===Qt.Key_Space);
    }
    Keys.onReleased: {
        event.accepted=(event.key===Qt.Key_Space);
    }

    font.pixelSize: 14
    //font.family: "Microsoft YaHei"

    //文本
    contentItem: Text {
        color: control.textColor
        text: control.text
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        opacity: enabled ? 1 : 0.44
        bottomPadding: 1
    }
    //背景
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.lighter(control.bgColor) }
            GradientStop { position: 0.5; color: control.bgColor }
            GradientStop { position: 1.0; color: Qt.lighter(control.bgColor) }
        }
        radius: control.radius
    }
}
