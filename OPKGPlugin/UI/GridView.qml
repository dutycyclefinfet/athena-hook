import QtQuick 2.6
import QtQuick.Controls 2.4

import Athena 9.9

Rectangle {
    id: root
    color: "#FFFFFF"
    anchors.fill: parent
    clip: true
    property var appDatabase: {}
    property var installedDatabase: []
    property string defIcon: Icons.empty.application
    
    signal dragDown()
    signal dragUp()
    signal dragLeft()
    signal dragRight()
    signal tap(string name)
    
    GridView {
        id: root_grid
        anchors.fill: parent
        
        property int text_margin: 5
        property int text_size: 24
        property int icon_size: 128
        property int icon_margin: 16
        property alias appDatabase: root.appDatabase
        property int lastDirScroll: -1

        onAppDatabaseChanged: {
            root_delay.running = true;
        }

        model: ListModel{}
        snapMode: GridView.SnapOneRow
        focus: false
        interactive: false
        highlightMoveDuration: 0
        leftMargin: (width-Math.floor(width/cellWidth)*cellWidth)/2
        rightMargin: leftMargin

        signal dragDown()
        signal dragUp()
        signal dragLeft()
        signal dragRight()
        signal tap(string name)

        onDragUp: {
            if (root_grid.lastDirScroll == 1) {
                root_grid.lastDirScroll = -1
                root_grid.currentIndex = Math.max(root_grid.currentIndex-Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight)*2+1, 0)
            } else {
                root_grid.currentIndex = Math.max(root_grid.currentIndex-Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight), 0)
            }
        }
        onDragDown: {
            if (root_grid.lastDirScroll == -1) {
                root_grid.lastDirScroll = 1
                root_grid.currentIndex = Math.min(root_grid.currentIndex+Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight)*2-1, root_grid.model.count-1)
            } else {
                root_grid.currentIndex = Math.min(root_grid.currentIndex+Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight), root_grid.model.count-1)
            }
        }
        
        Component {
            id: iconDelegate
            Item {
                width: root_grid.cellWidth
                height: root_grid.cellHeight
                Rectangle {
                    id: icon_rect
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: root_grid.icon_margin
                    anchors.rightMargin: root_grid.icon_margin
                    anchors.topMargin: root_grid.icon_margin
                    width: root_grid.cellWidth-root_grid.icon_margin*2
                    height: root_grid.cellWidth-root_grid.icon_margin*2
                    clip: true
                    Item {
                        anchors.fill: parent
                        Image {
                            id: icon_image
                            source: icon
                            onStatusChanged: if(status==Image.Error||status==Image.Null) source=root.defIcon
                            anchors.fill: parent
                            mipmap: true
                            asynchronous: true
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            anchors.topMargin: 5
                            anchors.bottomMargin: 5
                        }
                        Image {
                            id: icon_background
                            source: Icons.border.rounded
                            anchors.fill: parent
                            sourceSize.width: width
                            sourceSize.height: height
                        }
                        Rectangle {
                            id: icon_border
                            anchors.fill: parent
                            border.width: 1
                            border.color: "black"
                            radius: 35
                            color: "transparent"
                        }
                        Rectangle {
                            border.width: 1
                            border.color: "black"
                            color: "#FFFFFF"
                            width: parent.width/3
                            height: width
                            radius: width
                            anchors.top: parent.top
                            anchors.right: parent.right
                            visible: (root.installedDatabase.indexOf(name) != -1)
                            clip: true
                            Image {
                                id: icon_installed
                                source: Icons.modifier.installed
                                mipmap: true
                                asynchronous: true
                                anchors.centerIn: parent
                                width: parent.width*0.66
                                height: parent.height*0.66
                                sourceSize.width: width
                                sourceSize.height: height
                            }
                        }
                    }
                }
                Text {
                    id: icon_text
                    anchors.top: icon_rect.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: root_grid.text_margin
                    anchors.rightMargin: root_grid.text_margin
                    anchors.bottomMargin: root_grid.text_margin
                    text: name
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    elide: Text.ElideMiddle
                    font.pixelSize: root_grid.text_size
                    color: "black"
                }
                GestureArea {
                    onTap: { root_grid.tap(name); root.tap(name) }
                    onDragDown: { root_grid.dragUp(); root.dragUp() }
                    onDragUp: { root_grid.dragDown(); root.dragDown() }
                    onDragLeft: { root_grid.dragLeft(); root.dragLeft() }
                    onDragRight: { root_grid.dragRight(); root.dragRight() }
                }
            }
        }
        
        cellWidth: root_grid.icon_size + root_grid.icon_margin*2
        cellHeight: root_grid.icon_size + root_grid.icon_margin*2 + root_grid.text_margin + root_grid.text_size*2
        delegate: iconDelegate
    }
    signal refresh()
    onRefresh: visibleChanged()
    
    Timer {
        id: root_delay
        interval: 5
        running: false
        onTriggered: {
            root_grid.model.clear();
            if (root.appDatabase.length != undefined) {
                for (var ix in root_grid.appDatabase) {
                    root_grid.model.append({"icon": "", "name": root_grid.appDatabase[ix]});
                }
            } else {
                for (var name in root_grid.appDatabase) {
                    var db = root_grid.appDatabase[name];
                    db["name"] = name;
                    root_grid.model.append(db);
                }
            }
        }
    }
    
    Rectangle {
        id: pageNum
        
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 32
        width: height*2
        
        Text {
            anchors.centerIn: parent
            font.pixelSize: parent.height/2
            property int countPerPage: Math.floor(root_grid.height/root_grid.cellHeight)*Math.floor(root_grid.width/root_grid.cellWidth)
            text: Math.floor(root_grid.currentIndex/countPerPage+1) + "/" + Math.ceil(root_grid.model.count/countPerPage)
        }
    }
    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
        visible: !root.appDatabase || (Array.isArray(root.appDatabase) && !root.appDatabase.length) || !Object.keys(root.appDatabase).length
        Text {
            anchors.centerIn: parent
            text: "No apps/themes here yet!"
            font.pixelSize: 32
            color: "#353535"
        }
        GestureArea {
            onDragLeft: { root.dragLeft() }
            onDragRight: { root.dragRight() }
        }
    }
}
