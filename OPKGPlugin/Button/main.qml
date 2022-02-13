import QtQuick 2.6
import com.remarkable 1.0
import common 1.0
import device.view.navigator 1.0

SidebarItem {
    title: "Package manager"
    icon: Icons.icon_storage
    anchors {
        left: parent.left
        right: parent.right
    }
    inverted: parent.parent.parent.selectedIndex == 99
    active: true
    visible: true
    onClicked: parent.parent.parent.sideBarItemClicked(99)
    onActiveChanged: requestDelayedAntialiasing()
    delayedAntialiasing: true
}
