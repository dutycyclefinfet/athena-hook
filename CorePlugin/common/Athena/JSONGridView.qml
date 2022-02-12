import QtQuick 2.6
import QtQuick.Controls 2.4

import "../Common" as Q

GridView {
    id: root
    anchors.fill: parent
    property alias source: jsonLoader.source
    property alias filter: jsonLoader.filter
    
    property int lastDirScroll: -1
    onVisibleChanged: jsonLoader.sourceChanged()
    
    Q.JSONLoader {
        id: jsonLoader
        onDataChanged: {
            model.clear()
            for (var n=0; n<data.length; n++) {
                model.append(data[n]);
            }
        }
    }

    model: ListModel{}
    snapMode: GridView.SnapOneRow
    focus: false
    interactive: false
    highlightMoveDuration: 0
    leftMargin: (width-Math.floor(width/cellWidth)*cellWidth)/2
    rightMargin: leftMargin
    
    signal scrollDown()
    signal scrollUp()
    
    onScrollUp: {
        if (root.lastDirScroll == 1) {
            root.lastDirScroll = -1
            root.currentIndex = Math.max(root.currentIndex-Math.floor(root.width/root.cellWidth)*Math.floor(root.height/root.cellHeight)*2+1, 0)
        } else {
            root.currentIndex = Math.max(root.currentIndex-Math.floor(root.width/root.cellWidth)*Math.floor(root.height/root.cellHeight), 0)
        }
    }
    onScrollDown: {
        if (root.lastDirScroll == -1) {
            root.lastDirScroll = 1
            root.currentIndex = Math.min(root.currentIndex+Math.floor(root.width/root.cellWidth)*Math.floor(root.height/root.cellHeight)*2-1, root.model.count-1)
        } else {
            root.currentIndex = Math.min(root.currentIndex+Math.floor(root.width/root.cellWidth)*Math.floor(root.height/root.cellHeight), root.model.count-1)
        }
    }
}
