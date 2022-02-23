import QtQuick 2.6
import QtQuick.Controls 2.4
import com.remarkable 1.0

import "." as A
import Athena 9.9

Item {
    id: root
    visible: parent.parent.index==99
    anchors.fill: parent

    property var featured: ({})
    property var themes: ({})
    property var packagesCache: ({})
    
    property var packages: AthenaOPKG.allPackages
    property var installed: AthenaOPKG.installedPackages
    property var upgradable: AthenaOPKG.upgradablePackages
    
    property string opkgState: AthenaOPKG.state;
 
    function refreshOPKG() {
        root_delay.running = true;
    }
    onOpkgStateChanged: {
        if (typeof(opkgState) == 'string' && opkgState!="") {
            showModal(opkgState);
        }
    }
    onPackagesChanged: {
        showModal("Available packages updated.\n ");
    }
    onInstalledChanged: {
        showModal("Installed packages updated.\n ");
    }
    onUpgradableChanged: {
        showModal("Upgradable packages updated.\n ");
        if (upgradable.length > 0) {
            header.active = header.cells[3];
        }
    }
    onVisibleChanged: {
        if (visible) {
            showModal("Preparing package manager...\nPlease wait...")
            refreshOPKG();
        }
    }

    GridView {
        id: header
        height: 64
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        property string active: cells[0]
        property var cells: ["Featured", "Themes", "Other", "Updates"]
        cellWidth: width/model.length
        interactive: false

        model: cells
        delegate: Item {
            Rectangle {
                width: header.cellWidth
                height: header.height
                property bool isActive: header.active == header.cells[index]
                color: isActive?"#FFFFFF":"#353535"
                Text {
                    anchors.centerIn: parent
                    text: header.cells[index];
                    font.pixelSize: header.height/3
                    color: parent.isActive?"#353535":"#FFFFFF"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: header.active = header.cells[index];
                }
            }
        }
    }
    
    Item {
        id: pageContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        Item {
            id: featured
            visible: header.active==header.cells[0]
            anchors.fill: parent
            A.GridView {
                defIcon: Icons.empty.application
                appDatabase: root.featured
                installedDatabase: root.installed
                onDragLeft: header.active = header.cells[1]
                onDragRight: header.active = header.cells[header.cells.length-1]
                onTap: activatePopup(name, defIcon);
            }
        }
        Item {
            id: themes
            visible: header.active==header.cells[1]
            anchors.fill: parent
            A.GridView {
                defIcon: Icons.empty.theme
                appDatabase: root.themes
                installedDatabase: root.installed
                onDragLeft: header.active = header.cells[2]
                onDragRight: header.active = header.cells[0]
                onTap: activatePopup(name, defIcon);
            }
        }
        Item {
            id: others
            visible: header.active==header.cells[2]
            anchors.fill: parent
            A.ListView {
                defIcon: Icons.empty.executable
                appDatabase: root.packages
                installedDatabase: root.installed
                onDragLeft: header.active = header.cells[3]
                onDragRight: header.active = header.cells[1]
                onTap: activatePopup(name, defIcon);
            }
        }
        Item {
            id: updates
            visible: header.active==header.cells[3]
            anchors.fill: parent
            A.ListView {
                defIcon: Icons.empty.update
                appDatabase: root.upgradable
                installedDatabase: root.installed
                onDragLeft: header.active = header.cells[0]
                onDragRight: header.active = header.cells[2]
                onTap: activatePopup(name, defIcon, true);
            }
        }
    }
    function activatePopup(name, defIcon, upgrade=false) {
        popup.packageName = name;
        popup.defIcon = defIcon;
        popup.upgrade = upgrade;
    }
    function showModal(text) {
        modal.text = text;
    }
    function showModalIMG(ix=0, sources=[]) {
        modalIMG.ix = ix;
        modalIMG.sources = sources;
    }
    
    Item {
        id: popup
        anchors.fill: parent
        property string packageName: ""
        property string packageDescription: ""
        property bool installed: false
        property string defIcon: ""
        property var images: []
        property bool upgrade: false
        visible: (packageName != "")
        
        onDefIconChanged: {
            if (root.featured[popup.packageName]) {
                popup_icon.source = root.featured[popup.packageName]["icon"];
            } else if (root.themes[popup.packageName]) {
                popup_icon.source = root.themes[popup.packageName]["icon"];
            }
            popup_icon.statusChanged(popup_icon.status);
        }
        onPackageNameChanged: {
            if (popup.packageName) {
                installed = (root.installed.indexOf(popup.packageName)!=-1);
                if (root.featured[popup.packageName]) {
                    packageDescription = root.featured[popup.packageName]["description"];
                    popup_carousel.images = root.featured[popup.packageName]["images"];
                } else if (root.themes[popup.packageName]) {
                    packageDescription = root.themes[popup.packageName]["description"];
                    popup_carousel.images = root.themes[popup.packageName]["images"];
                } else if (root.packagesCache[popup.packageName]) {
                    packageDescription = root.packagesCache[popup.packageName]["description"];
                } else {
                    showModal("Parsing new package data...\nPlease wait...");
                    
                    var info = AthenaOPKG.getInfo(popup.packageName); //system("opkg info")
                    if (info) {
                        var dct = {};
                        for (var ix in info) {
                            if (info[ix]) {
                                var spl = info[ix].split(/^(.*?):\s*/);
                                if (spl.length == 3) {
                                    spl.shift();
                                    if (spl.length == 2) {
                                        dct[spl[0].toLowerCase()] = spl[1];
                                    }
                                }
                            }
                        }
                        root.packagesCache[popup.packageName] = dct;
                    }
                    packageDescription = root.packagesCache[popup.packageName]["description"];
                    showModal("");
                }
            }
        }
        
        Rectangle {
            anchors.fill: parent
            color: "#000000"
            opacity: 0.4
            MouseArea {
                anchors.fill: parent
                onClicked: activatePopup("", "");
            }
        }
        Rectangle {
            id: popup_container
            anchors.fill: parent
            property int margin: 50
            anchors.topMargin: margin
            anchors.leftMargin: margin
            anchors.rightMargin: margin
            anchors.bottomMargin: margin
            
            border.width: 2
            border.color: "black"
            radius: 10
            clip: true
            MouseArea {
                anchors.fill: parent
            }
            
            Image {
                id: popup_icon
                source: popup.defIcon
                onStatusChanged: if(status==Image.Error||status==Image.Null) source=popup.defIcon
                mipmap: true
                asynchronous: true
                sourceSize.height: 64
                fillMode: Image.PreserveAspectFit
                anchors.topMargin: 10
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Carousel {
                id: popup_carousel
                height: parent.height*0.4
                images: popup.images
                margin: 10
                anchors.top: popup_icon.bottom
                onTap: showModalIMG(ix, images);
            }
            Text {
                id: popup_appName
                
                property int margin: 10
                font.pixelSize: 38
                text: popup.packageName
                anchors.top: popup_carousel.bottom
                anchors.topMargin: margin
                anchors.leftMargin: margin
                anchors.rightMargin: margin
                anchors.left: parent.left
                wrapMode: Text.WordWrap
            }
            Text {
                id: popup_appDescription
                
                property int margin: 10
                font.pixelSize: 24
                text: popup.packageDescription
                anchors.top: popup_appName.bottom
                anchors.topMargin: margin
                anchors.leftMargin: margin
                anchors.rightMargin: margin
                anchors.left: parent.left
                anchors.right: parent.right
                wrapMode: Text.WordWrap
            }
            
            Rectangle {
                id: popup_buttons
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 128+radius
                border.width: popup_container.border.width
                border.color: popup_container.border.color
                radius: popup_container.radius
                
                Rectangle {
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: parent.width/2
                    height: parent.height
                    
                    border.width: parent.border.width
                    border.color: parent.border.color
                    radius: parent.radius
                    
                    color: (!popup.installed || popup.upgrade) ? "#FFFFFF" : "#B0B0B0"
                    Text {
                        text: popup.upgrade ? "Upgrade" : "Install"
                        color: "#000000"
                        font.pixelSize: (parent.height-parent.radius)/4
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: parent.radius/2
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (popup.upgrade) {
                                showModal("Upgrading " + popup.packageName + "...");
                                var err = AthenaOPKG.upgrade(popup.packageName);
                                if (err) {
                                    showModal("Failed to upgrade " + popup.packageName + "!\n" + err);
                                } else {
                                    showModal("Upgrade successful.");
                                }
                            } else if (!popup.installed) {
                                showModal("Installing " + popup.packageName + "...");
                                var err = AthenaOPKG.install(popup.packageName);
                                if (err) {
                                    showModal("Failed to install " + popup.packageName + "!\n" + err);
                                } else {
                                    refreshOPKG();
                                    showModal("Installation successful.");
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    width: parent.width/2
                    height: parent.height
                    
                    border.width: parent.border.width
                    border.color: parent.border.color
                    radius: parent.radius
                    
                    color: popup.installed ? "#FFFFFF" : "#B0B0B0"
                    Text {
                        text: "Remove"
                        color: "#000000"
                        font.pixelSize: (parent.height-parent.radius)/4
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: parent.radius/2
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (popup.installed) {
                                showModal("Removing " + popup.packageName + "...");
                                var err = AthenaOPKG.remove(popup.packageName);
                                if (err) {
                                    showModal("Failed to remove " + popup.packageName + "!\n" + err);
                                } else {
                                    refreshOPKG();
                                    showModal("Removal successful.");
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    anchors.left: parent.left
                    anchors.leftMargin: parent.border.width
                    anchors.right: parent.right
                    anchors.rightMargin: parent.border.width
                    anchors.top: parent.top
                    height: parent.radius
                    color: "#FFFFFF"
                }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: parent.radius
                    height: parent.height-parent.border.width-parent.radius
                    width: parent.radius*2
                    color: "#FFFFFF"
                }
            }
        }
    }
    Item {
        id: modal
        anchors.fill: parent
        property string text: ""
        visible: text!=""
        
        Rectangle {
            anchors.fill: parent
            color: "#000000"
            opacity: 0.4
            MouseArea {
                anchors.fill: parent
                onClicked: showModal("");
            }
        }
        Rectangle {
            id: modal_container
            width: parent.width/2
            height: modal_text.height+50
            anchors.centerIn: parent
            
            border.width: 2
            border.color: "black"
            radius: 10
            clip: true
            Text {
                id: modal_text
                text: modal.text
                color: "#000000"
                font.pixelSize: 24
                anchors.top: parent.top
                anchors.topMargin: 25
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            MouseArea {
                anchors.fill: parent
            }
        }
    }
    Item {
        id: modalIMG
        anchors.fill: parent
        property int ix: 0
        property var sources: []
        property string defIcon: Icons.empty.image
        visible: sources[ix]!=undefined
        
        Rectangle {
            anchors.fill: parent
            color: "#000000"
            opacity: 0.4
            MouseArea {
                anchors.fill: parent
                onClicked: showModalIMG();
            }
        }
        Rectangle {
            id: modalIMG_container
            width: parent.width-50
            height: parent.height-50
            anchors.centerIn: parent
            
            border.width: 2
            border.color: "black"
            radius: 10
            clip: true
            Image {
                id: modalIMG_image
                anchors.fill: parent
                source: modalIMG.sources[modalIMG.ix] ? modalIMG.sources[modalIMG.ix] : ""
                mipmap: true
                asynchronous: true
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                anchors.topMargin: 5
                anchors.bottomMargin: 5
            }
            GestureArea {
                onTap: showModalIMG()
                onDragRight: showModalIMG(modalIMG.ix>0 ? modalIMG.ix-1 : modalIMG.sources.length-1, modalIMG.sources)
                onDragLeft: showModalIMG(modalIMG.ix<modalIMG.sources.length-1 ? modalIMG.ix+1 : 0, modalIMG.sources)
            }
        }
    }
    
    Timer {
        id: root_delay
        interval: 500
        running: false
        
        function downloadBlock(url) {
            var x = new XMLHttpRequest();
            x.open('GET', url, false);
            x.send();
            return JSON.parse(x.responseText)
        }
        onTriggered: {
            if (WifiManager.enabled && WifiManager.isOnline) {
                showModal("Downloading featured app list...\nPlease wait...");
                var featured = downloadBlock("https://lonelytransistor.github.io/athena/featured.json");
                root.featured = featured ? featured : {};
                showModal("Downloading theme list...\nPlease wait...");
                var themes = downloadBlock("https://lonelytransistor.github.io/athena/themes.json");
                root.themes = themes ? themes : {};
            
                AthenaOPKG.update();
            } else {
                showModal("No internet connection detected.\nPlease retry when connected to the internet.");
            }
        }
    }
}
