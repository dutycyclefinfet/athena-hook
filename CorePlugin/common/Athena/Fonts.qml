pragma Singleton

import QtQuick 2.0

Item {
    readonly property string _: "file:///home/kat/Sources/RM2-kernel/Athena-kernel/mmcblk2p4/root/.xochitlPlugins/.common/fonts/"
    
    FontLoader {
        id: material
        source: _+"material.ttf"
    }
    FontLoader {
        id: icomoon
        source: _+"icomoon.ttf"
    }
    FontLoader {
        id: d7mono
        source: _+"d7mono.ttf"
    }
}
