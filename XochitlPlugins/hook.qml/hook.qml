import QtQuick 2.6
import HKID.js as HKID
import File.js as File
import Hash.js as Hash

Item {
    id: root
    anchors.fill: parent
    visible: false

    property int loadingDelay: 100
    property var retVal: {"PLUGIN_SUCCESS": 0, "PLUGIN_DISABLED": 1, "PLUGIN_INVALID_HOOKID": 2, "PLUGIN_INVALID_TYPE": 3, "PLUGIN_EXCEPTION": 4, "PLUGIN_NO_SUCH_FILE": 5, "PLUGIN_OBJECT_NOT_CREATED": 6};

    readonly property Item root_id: hookEntryPoint.root_id
    readonly property string pluginStore_dir: AthenaHook.storePath // "file:///home/root/.xochitlPlugins"
    readonly property string pluginStore_json: "plugins.json"
    readonly property string pluginStore_jsonc: "pluginsc.json"
    readonly property var defaultPlugins: [{"name": ".hook.qml/settings", "type": "BUNDLE"}]
    
    property var hookedObjects: {"names": []}

    Timer {
        id: root_delay
        interval: root.loadingDelay
        triggeredOnStart: false
        repeat: false
        running: true
        
        onTriggered: {
            readPlugins();
        }
    }
    /////////////////////////////////////////
    // Key handler
    /////////////////////////////////////////
    function keyHandler(ev_type, ev) {
        for (var obj of hookedObjects.names) {
            var func = hookedObjects[obj]["keyHandler"];
            if (func) {
                if (func(ev_type, ev)) {
                    console.log("Key handler intercepted by: ", obj);
                    return true;
                }
            }
        }
    }
    /////////////////////////////////////////
    // Load plugin
    /////////////////////////////////////////
    function loadPluginQML(plugin, obj_id) {
        var path = root.pluginStore_dir + "/" + plugin["name"] + "/main.qml";
        
        var component = Qt.createComponent(path);
        if (component.status == Component.Ready) {
            var obj = component.createObject(obj_id, {"hookedObjects": hookedObjects});
            if (!obj)
                return retVal.PLUGIN_OBJECT_NOT_CREATED;
            
            hookedObjects[plugin["name"]] = {"obj": obj, "keyHandler": obj.keyHandler};
            hookedObjects["names"].push(plugin["name"]);
        } else if (component.status == Component.Error) {
            console.error("Component fatal error: " + component.errorString());

            return retVal.PLUGIN_EXCEPTION;
        }
        return retVal.PLUGIN_SUCCESS;
    }
    function loadPluginJS(plugin, obj_id) {
        var f_inj_code = File.read(root.pluginStore_dir + "/" + plugin["name"] + "/main.js");
        if (!f_inj_code)
            return retVal.PLUGIN_NO_SUCH_FILE;
        
        try {
            var f_inj_func = Function('qtObj', f_inj_code); //Pass injection point as qtObj
            f_inj_func(obj_id); //And call the script
        } catch (error) {
            console.error("JS injection fatal error: " + error);
            return retVal.PLUGIN_EXCEPTION;
        }
        return retVal.PLUGIN_SUCCESS;
    }
    function loadPROP(plugin, obj_id) {
        for (var key in plugin["data"]) {
            obj_id[key] = plugin["data"][key];
        }
    }
    function loadPlugin(plugin) {
        // Get injection point handle
        try {
            var objs = HKID.execute(root.root_id, plugin["hookid"], plugin["hookid_c"]);
            plugin["hookid_c"] = objs["hookid_c"];
            objs = objs["objects"];
        } catch (error) {
            console.warn("Could not find the hookid: " + error);
        }
        if (objs.length == 0)
            return retVal.PLUGIN_INVALID_HOOKID;
        
        // Process payload
        while (objs.length > 0) {
            var obj_id = objs.pop();
            switch (plugin["type"]) {
                case "QML":
                    return loadPluginQML(plugin, obj_id);
                case "JS":
                    return loadPluginJS(plugin, obj_id);
                case "PROP":
                    return loadPluginPROP(plugin, obj_id);
                default:
                    return retVal.PLUGIN_INVALID_TYPE;
            }
        }
        return retVal.PLUGIN_SUCCESS;
    }
    /////////////////////////////////////////
    // Parse plugins
    /////////////////////////////////////////
    function parsePlugins(plugins, config) {
        var ret = retVal.PLUGIN_SUCCESS;
        
        for (var ix in plugins) {
            var plugin = plugins[ix];
            if (config[plugin["name"]]["enabled"] == true) {
                if (plugin["type"] == "BUNDLE") {
                    if (plugin["cache"] == undefined) { //Uncached
                        try {
                            var pluginName = plugin["name"];
                            var subPlugins = JSON.parse(File.read(root.pluginStore_dir + pluginName + "/manifest.json"));
                            plugins[ix]["cache"] = []
                            for (var subPlugin of subPlugins) {
                                subPlugin["name"] = pluginName + "/" + subPlugin["name"];
                                ret = parsePlugins(subPlugin);
                                plugins[ix]["cache"].push(subPlugin);
                            }
                        } catch (error) {
                            ret = retVal.PLUGIN_NO_SUCH_FILE;
                        }
                    } else {
                        for (var subPlugin of plugin["cache"]) {
                            ret = parsePlugins(subPlugin);
                        }
                    }
                } else {
                    ret = loadPlugin(plugin);
                }
            } else {
                ret = retVal.PLUGIN_DISABLED;
            }
            
            // Parse error value
            if (ret == retVal.PLUGIN_SUCCESS) {
                console.info("Plugin " + plugin["name"] + " has been loaded.");
            } else if (ret == retVal.PLUGIN_DISABLED) {
                console.info("Plugin " + plugin["name"] + " is disabled.");
            } else if (ret == retVal.PLUGIN_INVALID_TYPE) {
                console.error("Plugin " + plugin["name"] + " has an invalid definition [PLUGIN_INVALID_TYPE].");
            } else if (ret == retVal.PLUGIN_INVALID_HOOKID) {
                console.error("Plugin " + plugin["name"] + " has an invalid definition [PLUGIN_INVALID_HOOKID].");
            } else if (ret == retVal.PLUGIN_OBJECT_NOT_CREATED) {
                console.info("Plugin " + plugin["name"] + " failed. Engine could not spawn a new object [PLUGIN_OBJECT_NOT_CREATED].");
            } else if (ret == retVal.PLUGIN_EXCEPTION) {
                console.error("Plugin " + plugin["name"] + " has an invalid code [PLUGIN_EXCEPTION].");
            } else if (ret == retVal.PLUGIN_NO_SUCH_FILE) {
                console.error("Plugin " + plugin["name"] + " does not exist [PLUGIN_NO_SUCH_FILE].");
            }
        }
        return ret;
    }
    /////////////////////////////////////////
    // Read plugins json
    /////////////////////////////////////////
    function readPluginsData(data) {
        var plugins_info = JSON.parse(data);
        if (plugins_info) {
            parsePlugins(plugins_info);
            plugins_info[0].hash = Hash.get(data);
            File.write(pluginStore_jsonc, JSON.stringify(plugins_info));
        }
    }
    function readPluginsAll() {
        File.read(pluginStore_jsonc, function(data_compiled) {
        if (AthenaHook.data)
        File.read(pluginStore_jsonc, function(data_compiled) {
            File.read(pluginStore_json, function(data_uncompiled) {
                if (data_uncompiled && JSON.parse(data_compiled)[0] &&
                JSON.parse(data_compiled)[0].hash &&
                Hash.get(data_uncompiled)==JSON.parse(data_compiled)[0].hash) {
                    readPluginsData(data_compiled);
                } else {
                    console.warn("Recompiling due to plugins.json hash mismatch.");
                    readPluginsData(data_uncompiled);
                }
            }, function() {
                File.read(pluginStore_json, function(data_uncompiled) {
                    readPluginsData(data_uncompiled);
                });
            });
        }, function() {
            File.read(pluginStore_json, function(data_uncompiled) {
                readPluginsData(data_uncompiled);
            });
        });
    }
    function readPlugins() {
        parsePlugins(defaultPlugins);
        if (AthenaKernel.isRunning) {
            readPluginsAll();
        }
    }
}
