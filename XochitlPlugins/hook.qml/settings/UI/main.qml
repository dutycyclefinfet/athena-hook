import QtQuick 2.6
import QtQuick.Controls 2.4
import com.remarkable 1.0
import common 1.0
import device.ui.controls 1.0
import device.ui.text 1.0
import device.view.dialogs 1.0
import "." as A

Item {
    id: root
    
    anchors.fill: parent
    visible: parent.parent.index==10
    
    Panel {
        id: header
        width: parent.width
        height: Values.navigatorSettingsHeaderHeight

        TextHeaderLarge {
            text: qsTr("Athena settings")
            anchors.centerIn: parent
            height: parent.height
            color: Values.colorWhite
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        anchors {
            top: header.visible ? header.bottom : header.top
            topMargin: header.visible ? 0 : 90
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        
        A.ScrollableArea {
            PanelToggle {
                id: isAthena_s
                width: header.width
                icon: Icons.icon_checkmark
                header: qsTr("Running athena now")
                on: AthenaSettings.kernelIsAthena
            }
            PanelToggle {
                id: bootAthena_s
                width: header.width
                icon: Icons.icon_checkmark
                header: qsTr("Boot into Athena on restart")
                on: AthenaSettings.kernelBootAthena
                onPressed: AthenaSettings.kernelBootAthena = !AthenaSettings.kernelBootAthena;
            }
            PanelToggle {
                id: rootfsEncryption_s
                width: parent.width
                icon: Icons.icon_padlock
                header: qsTr("Use dm-crypt for overlayfs")
                on: AthenaSettings.encryptedRoot==true
                onPressed: AthenaSettings.encryptedRoot = !AthenaSettings.encryptedRoot
                visible: false
            }
            PanelToggle {
                id: rootfsClear_s
                width: parent.width
                icon: Icons.icon_trashcan
                header: qsTr("Wipe overlayfs on next boot")
                on: AthenaSettings.kernelOverlayWipe
                onPressed: AthenaSettings.kernelOverlayWipe = !AthenaSettings.kernelOverlayWipe
            }
            A.PanelDelimiter {
                visible: AthenaSettings.kernelIsAthena
            }
            A.PanelDropdown {
                id: defaultUSB_s
                visible: AthenaSettings.kernelIsAthena
                header: qsTr("Active USB gadget")
                modelList: AthenaSettings.usbModes
                activeIndex: AthenaSettings.usbModes.indexOf(AthenaSettings.usbMode)
                onActiveIndexChanged: AthenaSettings.usbMode = AthenaSettings.usbModes[activeIndex]
                z: 9
            }
            A.PanelDelimiter {
                visible: AthenaSettings.kernelIsAthena
            }
            A.PanelDropdown {
                id: governorCPU_s
                visible: AthenaSettings.kernelIsAthena
                header: qsTr("Active CPU governor")
                modelList: AthenaSettings.cpuGovernors
                activeIndex: AthenaSettings.cpuGovernors.indexOf(AthenaSettings.cpuGovernor)
                onActiveIndexChanged: AthenaSettings.cpuGovernor = AthenaSettings.cpuGovernors[activeIndex]
                z: 8
            }
            A.PanelSlider {
                id: undervoltCPU_s
                visible: AthenaSettings.kernelIsAthena
                title: qsTr("Adjust CPU voltage by:")
                suffix: qsTr("mV")
                from: -100
                stepSize: 25
                to: 100
                greyOut: 0
                alwaysShowSign: true
                
                value: AthenaSettings.cpuUndervolt
                onDefaultSignal: AthenaSettings.cpuUndervolt = value;
            }
            A.PanelSlider {
                id: zRAM_s
                visible: AthenaSettings.kernelIsAthena
                title: qsTr("Use zRAM:")
                suffix: qsTr(" MB")
                from: 0
                stepSize: 64
                to: 512
                greyOut: 0
                
                value: AthenaSettings.zRAM
                onDefaultSignal: AthenaSettings.zRAM = value;
            }
            A.PanelDelimiter {}
            A.PanelSlider {
                id: batteryLimit_s
                visible: AthenaSettings.kernelIsAthena
                title: qsTr("Limit maximum battery charge to:")
                suffix: qsTr("%")
                from: 50
                stepSize: 5
                to: 100
                greyOut: 1
                
                value: AthenaSettings.batteryLimit
                onDefaultSignal: AthenaSettings.batteryLimit = value;
            }
            A.PanelSlider {
                id: suspend_s
                title: qsTr("Suspend your reMarkable after:")
                suffix: qsTr(" minutes")
                from: 0
                stepSize: 1
                to: 60
                greyOut: 0
                
                value: Math.floor(Settings.idleSuspendDelay/60000)
                onDefaultSignal: Settings.idleSuspendDelay = value*60000;
            }
            A.PanelSlider {
                id: poweroff_s
                title: qsTr("Power off your reMarkable after:")
                suffix: qsTr(" hours")
                from: 0
                stepSize: 1
                to: 24
                greyOut: 0
                enabled: (Settings.idleSuspendDelay == 0)
                
                value: Math.floor(Settings.powerOffDelay/3600000)
                onDefaultSignal: Settings.powerOffDelay = value*3600000;
            }
            A.PanelDelimiter {}
            PanelToggle {
                id: loadXochitlPlugins_s
                width: parent.width
                icon: Icons.icon_remarkable_device
                header: qsTr("Enable Xochitl plugins")
                on: AthenaSettings.enableXochitlPlugins
                onPressed: AthenaSettings.enableXochitlPlugins = !AthenaSettings.enableXochitlPlugins
            }
        }
    }
}
