import QtQuick 2.6

import "../Common" as Q
import "../Common/common.js" as C

Item {
    id: root
    property alias model: listView.model
    property alias delegate: listView.delegate
    property int itemsNum: 1
    anchors.fill: parent
    
    Rectangle {
        clip: true
        anchors.fill: parent
        anchors.bottomMargin: C.D.text_size
        color: "transparent"
        
        ListView {
            id: listView
            anchors.fill: parent
            interactive: false
            focus: false
            highlightMoveDuration: 0
            property int lastDirScroll: -1
        }
    }
    Q.TextShadow {
        parentText: indicator
    }
    Text {
        id: indicator
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: {
            var text = ""
            var n = 0
            var f = Math.floor(listView.currentIndex/root.itemsNum)
            for (n; n <= f; n++) {
                text += "●"
            }
            for (n; n < Math.floor(listView.model.count/root.itemsNum)+((listView.model.count%root.itemsNum&&listView.model.count>root.itemsNum)?1:0); n++) {
                text += "◎"
            }
            return text
        }
        font.pixelSize: C.D.text_size
        color: "#000000"
        horizontalAlignment: Text.AlignHCenter
    }
    
    signal scrollDown()
    signal scrollUp()
    onScrollUp: {
        if (listView.lastDirScroll == 1) {
            listView.lastDirScroll = -1
            listView.currentIndex -= (listView.currentIndex+1) % root.itemsNum
            listView.currentIndex = Math.max(listView.currentIndex - root.itemsNum*2 + 1, 0)
        } else {
            listView.currentIndex = Math.max(listView.currentIndex - root.itemsNum, 0)
        }
    }
    onScrollDown: {
        if (listView.lastDirScroll == -1) {
            listView.lastDirScroll = 1
            listView.currentIndex = Math.min(listView.currentIndex + root.itemsNum*2 - 1, listView.model.count-1)
        } else {
            listView.currentIndex = Math.min(listView.currentIndex + root.itemsNum, listView.model.count-1)
        }
    }
}
