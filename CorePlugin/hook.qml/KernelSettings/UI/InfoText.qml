import QtQuick 2.6
import QtQuick.Controls 2.4

Item {
    id: root
    property string suffix: ""
    property string title: ""
    property int value: 0
    property bool enabled: true
    property bool alwaysShowSign: false
    
    height: o_title.height + o_title.anchors.topMargin + 46
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
        id: o_title
        anchors {
            left: parent.left
            leftMargin: 145
            top: parent.top
        }
        text: root.title
        font.bold: true
        font.pixelSize: 28
        color: enabled ? "#FFFFFF" : "#808080"
    }
    Text {
        id: value
        anchors {
            left: o_title.left
            leftMargin: 10
            top: o_title.top
            right: parent.right
            rightMargin: 145
        }
        text: ((root.alwaysShowSign && root.value > 0) ? "+" : "") + root.value + root.suffix
        font.bold: true
        font.pixelSize: 28
        horizontalAlignment: Text.AlignRight
        color: enabled ? "#FFFFFF" : "#808080"
    }
}
