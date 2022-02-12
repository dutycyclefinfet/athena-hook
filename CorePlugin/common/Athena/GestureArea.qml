import QtQuick 2.11
import QtQuick.Controls 2.4

MultiPointTouchArea {
    anchors.fill: parent
    maximumTouchPoints: 1
    mouseEnabled: true
    property real dragThreshold: 1000.0
    onReleased: function asd(mTouchPoint) {
        var dragX = mTouchPoint[0].startX-mTouchPoint[0].x;
        var dragY = mTouchPoint[0].startY-mTouchPoint[0].y;
        if ((Math.abs(dragX) < dragThreshold) && (Math.abs(dragY) < dragThreshold)) {
            tap();
        } else if (Math.abs(dragX) > Math.abs(dragY)) {
            if (dragX > 0) {
                dragLeft();
            } else {
                dragRight();
            }
        } else {
            if (dragY > 0) {
                dragUp();
            } else {
                dragDown();
            }
        }
    }
    onGestureStarted: function asd(mGesture) {
        dragThreshold = mGesture.dragThreshold
    }
    
    signal tap()
    signal dragLeft()
    signal dragRight()
    signal dragUp()
    signal dragDown()
}
