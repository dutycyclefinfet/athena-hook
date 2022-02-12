import QtQuick 2.6
import QtQuick.Controls 2.4

Item {
    id: root
    property string suffix: ""
    property string title: ""
    property int from: 0
    property int stepSize: 0
    property int to: 0
    property alias value: slider.value
    property int greyOut: 0
    property bool enabled: true
    property bool alwaysShowSign: false
    
    signal defaultSignal()
    height: o_title.height + o_title.anchors.topMargin + slider.height + slider.anchors.topMargin + 46
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
        text: ((root.alwaysShowSign && slider.value > 0) ? "+" : "") + slider.value + root.suffix
        font.bold: true
        font.pixelSize: 28
        horizontalAlignment: Text.AlignRight
        color: enabled ? "#FFFFFF" : "#808080"
    }
    Slider {
        id: slider
        from: root.from
        stepSize: root.stepSize
        to: root.to
        anchors.left: o_title.left
        anchors.right: value.right
        anchors.top: o_title.bottom
        anchors.topMargin: 10
        height: 60
        property int oldValue: 0
        enabled: root.enabled
        handle: Rectangle {
            x: parent.visualPosition * (parent.width - width)
            width: parent.height
            height: parent.height
            color: enabled ? "#909090" : "#000000"
            border.color: enabled ? "#000000" : "#909090"
            border.width: 2
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "|"
                font.bold: true
                font.pixelSize: parent.height/2
                color: enabled ? "#000000" : "#909090"
            }
        }
        background: Rectangle {
            height: parent.height
            color: enabled ? (greyOut&2 ? "#505050" : "#FFFFFF") : "#808080"

            Rectangle {
                x: parent.parent.visualPosition * parent.width
                width: (1-parent.parent.visualPosition) * (parent.width)
                height: parent.height
                color: enabled ? (greyOut&1 ? "#505050" : "#FFFFFF") : "#808080"
            }
        }
        onValueChanged: function() {
            if (this.value != this.oldValue) {
                this.oldValue = this.value
                signal_delay.running = true;
            }
        }
        Timer {
            id: signal_delay
            interval: 2000
            onTriggered: {
                root.defaultSignal();
            }
        }
    }
}
