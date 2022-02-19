pragma Singleton

import QtQuick 2.0

Item {
    readonly property string _: AthenaHook.pluginsPath+"/.common/fonts/"
    
    readonly property FontLoader material: FontLoader {
        source: _+"material.ttf"
    }
    readonly property FontLoader icomoon: FontLoader {
        source: _+"icomoon.ttf"
    }
    readonly property FontLoader d7mono: FontLoader {
        source: _+"d7mono.ttf"
    }
}
