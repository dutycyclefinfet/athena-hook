import QtQuick 2.6
import QtQuick.Controls 2.4

import Athena 9.9

Rectangle {
    id: root
    color: "#FFFFFF"
    property int margin: 10
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.topMargin: margin
    anchors.leftMargin: margin
    anchors.rightMargin: margin
    clip: true
    property var images: []
    property string defIcon: Icons.empty.image
    
    signal dragDown()
    signal dragUp()
    signal dragLeft()
    signal dragRight()
    signal tap(int ix)

    onImagesChanged: {
        if (!images.length) {
            images = [root.defIcon];
        }
        root_delay.running = true;
    }
    
    GridView {
        id: root_grid
        anchors.fill: parent
        
        property int text_margin: 5
        property int text_size: 24
        property int icon_size: 128
        property int icon_margin: 16
        property int lastDirScroll: -1

        model: ListModel{}
        snapMode: GridView.SnapOneRow
        focus: false
        interactive: false
        highlightMoveDuration: 0

        signal dragDown()
        signal dragUp()
        signal dragLeft()
        signal dragRight()
        signal tap(int ix)

        onDragRight: {
            if (root_grid.lastDirScroll == 1) {
                root_grid.lastDirScroll = -1
                root_grid.currentIndex = Math.max(root_grid.currentIndex-Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight)*2+1, 0)
            } else {
                root_grid.currentIndex = Math.max(root_grid.currentIndex-Math.floor(root_grid.width/root_grid.cellWidth)*Math.floor(root_grid.height/root_grid.cellHeight), 0)
            }
        }
        onDragLeft: {
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
                Image {
                    width: root_grid.cellWidth
                    height: root_grid.cellHeight
                    source: img
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
                    GestureArea {
                        onTap: { root_grid.tap(ix); root.tap(ix) }
                        onDragLeft: { root_grid.dragLeft(); root.dragLeft() }
                        onDragRight: { root_grid.dragRight(); root.dragRight() }
                    }
                }
            }
        }
        
        cellWidth: root.width/3
        cellHeight: root.height
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
            for (var ix in root.images) {
                root_grid.model.append({"img": root.images[ix], "ix": ix});
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
}
