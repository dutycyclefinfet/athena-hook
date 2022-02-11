import QtQuick 2.6

Timer {
    id: root
    interval: 5
    running: false
    repeat: true
    property int state: 0
    property int stateMax: 0
    property int stateMaxNow: 0
    function exec(f, t) {
        if (f) {
            state = f;
        } else {
            state = 0;
        }
        if (t) {
            stateMaxNow = t;
        } else {
            stateMaxNow = stateMax;
        }
        running = true;
    }
    signal fire();
    onTriggered: {
        fire();
        if (state <= stateMaxNow) {
            state = state + 1;
        } else {
            state = 0;
            running = false;
        }
    }
}
