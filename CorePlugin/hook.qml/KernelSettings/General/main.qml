import QtQuick 2.6
import QtQuick.Controls 2.4
import com.remarkable 1.0
import common 1.0
import device.ui.controls 1.0
import device.ui.text 1.0
import device.view.dialogs 1.0

import "." as A

Row {
    id: root
    visible: parent.parent.index==0
    
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 400
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: Values.designGridVerticalWidth

    Button {
        id: restartXochitlButton
        text: qsTr("Restart Xochitl")
        iconCode: Icons.icon_restore
        onClicked: AthenaSystem.restartXochitl();
    }

    Button {
        id: bootAthenaButton
        text: qsTr("Reboot to Athena")
        iconCode: Icons.icon_power
        onClicked: {
            AthenaKernel.bootAthena = true;
            UpgradeManager.reboot();
        }
    }
}
