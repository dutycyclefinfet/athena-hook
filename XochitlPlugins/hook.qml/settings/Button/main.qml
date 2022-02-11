import QtQuick 2.6
import com.remarkable 1.0
import common 1.0
import device.view.navigator 1.0

SidebarItem {
    title: "Athena"
    icon: Icons.icon_warning_circle
    anchors {
        left: parent.left
        right: parent.right
    }
    inverted: parent.parent.parent.selectedIndex == 10
    active: true
    visible: true
    onClicked: parent.parent.parent.sideBarItemClicked(10)
    onActiveChanged: requestDelayedAntialiasing()
    delayedAntialiasing: true
}
