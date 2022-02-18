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
    visible: parent.parent.index==98
    
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
                on: AthenaKernel.isAthena
            }
            PanelToggle {
                id: bootAthena_s
                width: header.width
                icon: Icons.icon_checkmark
                header: qsTr("Boot into Athena on restart")
                on: AthenaKernel.bootAthena
                onPressed: AthenaKernel.bootAthena = !AthenaKernel.bootAthena;
            }
            PanelToggle {
                id: rootfsEncryption_s
                width: header.width
                icon: Icons.icon_padlock
                header: qsTr("Use dm-crypt for overlayfs")
                on: AthenaSettings.encryptedRoot==true
                onPressed: AthenaSettings.encryptedRoot = !AthenaSettings.encryptedRoot
                visible: false
            }
            A.PanelDelimiter {
                visible: AthenaKernel.isAthena
            }
            A.PanelDropdown {
                id: defaultUSB_s
                width: header.width
                visible: AthenaKernel.isAthena
                header: qsTr("Active USB gadget")
                modelList: AthenaSettings.usbModes
                activeIndex: AthenaSettings.usbModes.indexOf(AthenaSettings.usbMode)
                onActiveIndexChanged: AthenaSettings.usbMode = AthenaSettings.usbModes[activeIndex]
                z: 9
            }
            A.PanelDelimiter {
                visible: AthenaKernel.isAthena
            }
            A.InfoText {
                id: screenVoltage_s
                width: header.width
                visible: AthenaKernel.isAthena
                title: qsTr("EPD voltage")
                suffix: qsTr("V")
                
                value: Math.floor(AthenaKernel.epdCurrentVoltage/1000)
            }
            A.InfoText {
                id: currentVoltage_s
                width: header.width
                visible: AthenaKernel.isAthena
                title: qsTr("CPU voltage")
                suffix: qsTr("mV")
                
                value: Math.floor(AthenaKernel.cpuCurrentVoltage/1000)
            }
            A.InfoText {
                id: cpu0Frequency_s
                width: header.width
                visible: AthenaKernel.isAthena
                title: qsTr("CPU frequency")
                suffix: qsTr("MHz")
                
                value: Math.floor(AthenaKernel.cpu0Frequency/1000) + ", " + Math.floor(AthenaKernel.cpu1Frequency/1000)
            }
            A.PanelDropdown {
                id: governorCPU_s
                width: header.width
                visible: AthenaKernel.isAthena
                header: qsTr("Active CPU governor")
                modelList: AthenaSettings.cpuGovernors
                activeIndex: AthenaSettings.cpuGovernors.indexOf(AthenaSettings.cpuGovernor)
                onActiveIndexChanged: AthenaSettings.cpuGovernor = AthenaSettings.cpuGovernors[activeIndex]
                z: 8
            }
            A.PanelSlider {
                id: undervoltEPD_s
                width: header.width
                visible: AthenaKernel.isAthena
                title: qsTr("Adjust EPD voltage by (has overvoltage protection):")
                suffix: qsTr("mV")
                from: -1000
                stepSize: 10
                to: 1000
                greyOut: 0
                alwaysShowSign: true
                
                value: AthenaKernel.epdAdjustmentVoltage
                onDefaultSignal: AthenaKernel.epdAdjustmentVoltage = value;
            }
            A.PanelSlider {
                id: undervoltCPU_s
                width: header.width
                visible: AthenaKernel.isAthena
                title: qsTr("Adjust CPU voltage by:")
                suffix: qsTr("mV")
                from: -100
                stepSize: 25
                to: 75
                greyOut: 0
                alwaysShowSign: true
                
                value: AthenaKernel.cpuUndervolt
                onDefaultSignal: AthenaKernel.cpuUndervolt = value;
            }
            A.PanelSlider {
                id: zRAM_s
                width: header.width
                visible: AthenaKernel.isAthena
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
                width: header.width
                visible: AthenaKernel.isAthena
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
                width: header.width
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
                width: header.width
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
        }
    }
}
