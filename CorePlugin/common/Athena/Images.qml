pragma Singleton

import QtQuick 2.0

QtObject {
    readonly property string w: "file:///home/kat/Sources/RM2-kernel/Athena-kernel/mmcblk2p4/root/.xochitlPlugins/.common/images/weather/"
    
    readonly property var weather: {
        "01d": w+"01d.png",
        "01n": w+"01n.png",
        "02d": w+"02d.png",
        "02n": w+"02n.png",
        "03d": w+"03d.png",
        "03n": w+"03n.png",
        "04d": w+"04d.png",
        "04n": w+"04n.png",
        "09d": w+"09d.png",
        "09n": w+"09n.png",
        "10d": w+"10d.png",
        "10n": w+"10n.png",
        "11d": w+"11d.png",
        "11n": w+"11n.png",
        "13d": w+"13d.png",
        "13n": w+"13n.png",
        "50d": w+"50d.png",
        "50n": w+"50n.png"
    }
}
