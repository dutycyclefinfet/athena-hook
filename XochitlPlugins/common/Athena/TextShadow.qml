import QtQuick 2.6

Text {
    function invertHex(hex) {
        return "#"+(Number("0x" + hex.substr(1)) ^ 0xFFFFFF).toString(16).toUpperCase();
    }
    property string shadowColor: invertHex(color.toString())
    property int shadowThickness: 1
    property real shadowOpacity: 0.5
    
    Text {
        id: shadow_n1n1
        x: -shadowThickness
        y: -shadowThickness
        height: parent.height
        width: parent.width
        color: shadowColor
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        horizontalAlignment: parent.horizontalAlignment
        verticalAlignment: parent.verticalAlignment
        opacity: shadowOpacity
    }
    Text {
        id: shadow_n1p1
        x: -shadowThickness
        y: shadowThickness
        height: parent.height
        width: parent.width
        color: shadowColor
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        horizontalAlignment: parent.horizontalAlignment
        verticalAlignment: parent.verticalAlignment
        opacity: shadowOpacity
    }
    Text {
        id: shadow_p1p1
        x: shadowThickness
        y: shadowThickness
        height: parent.height
        width: parent.width
        color: shadowColor
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        horizontalAlignment: parent.horizontalAlignment
        verticalAlignment: parent.verticalAlignment
        opacity: shadowOpacity
    }
    Text {
        id: shadow_p1n1
        x: shadowThickness
        y: -shadowThickness
        height: parent.height
        width: parent.width
        color: shadowColor
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        horizontalAlignment: parent.horizontalAlignment
        verticalAlignment: parent.verticalAlignment
        opacity: shadowOpacity
    }
    Text {
        id: shadow_inner
        anchors.centerIn: parent
        color: parent.color
        text: parent.text
        font.pixelSize: parent.font.pixelSize
        horizontalAlignment: parent.horizontalAlignment
        verticalAlignment: parent.verticalAlignment
    }
}
