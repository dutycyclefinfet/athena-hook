pragma Singleton

import QtQuick 2.0

Item {
    readonly property string _: AthenaHook.pluginsPath+"/.common/fonts/"
    
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
