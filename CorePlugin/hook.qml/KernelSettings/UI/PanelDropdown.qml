import QtQuick 2.6
import com.remarkable 1.0
import common 1.0
import device.ui.controls 1.0
import device.ui.text 1.0
import device.view.dialogs 1.0

PanelAction {
    id: root
    width: parent.width
    header: qsTr("")
    property var modelList: []
    property int activeIndex: 0
    actionText: modelList[activeIndex]
    actionIcon: subroot.visible ? Icons.icon_angle_up : Icons.icon_angle_down
    showFrame: true

    Column {
        id: subroot
        anchors {
            top: root.buttonBox.bottom
            left: root.left
            right: root.right
            leftMargin: Values.navigatorSettingsItemMargin
            rightMargin: Values.navigatorSettingsItemMargin
        }
        visible: false

        Repeater {
            id: subroot_repeater
            model: root.modelList
            delegate: ButtonBox {
                readonly property var squishID: root.modelList.indexOf(modelData)
                inverted: !(root.activeIndex==squishID)
                height: 90
                anchors.right: parent.right
                anchors.left: parent.left
                iconVisible: false
                leftBorderVisible: true
                rightBorderVisible: true
                bottomBorderVisible: true
                backgroundVisible: true

                TextParagraph {
                    anchors.left: parent.left
                    anchors.leftMargin: Values.designGridMargin * 2
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    color: root.activeIndex==squishID ? Values.colorBlack : Values.colorWhite
                    text: modelData
                }

                onClicked: {
                    subroot.visible = false;
                    root.activeIndex = squishID;
                }
            }
        }
    }
    onClicked: subroot.visible = !subroot.visible
}
