import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQml.Models 2.11
import com.remarkable 1.0
import common 1.0
import device.ui.text 1.0
import device.ui.controls 1.0
import device.view.setup 1.0

Item {
    id: root
    anchors.fill: parent
    default property alias content: croot.children

    ScrollOverlay {
        view: froot
        color: Values.colorWhite
    }
    Flickable {
        id: froot
        anchors.fill: parent

        clip: true
        contentWidth: croot.width
        contentHeight: croot.height
        interactive: false
        boundsBehavior: Flickable.StopAtBounds
        
        Column {
            id: croot
        }
    }
}
