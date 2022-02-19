import QtQuick 2.6
import "HKID.js" as HKID
import "File.js" as File

Item {
    id: root
    anchors.fill: parent
    visible: false

    property int loadingDelay: 100
    property var retVal: {"PLUGIN_SUCCESS": 0, "PLUGIN_DISABLED": 1, "PLUGIN_INVALID_HOOKID": 2, "PLUGIN_INVALID_TYPE": 3, "PLUGIN_EXCEPTION": 4, "PLUGIN_NO_SUCH_FILE": 5, "PLUGIN_OBJECT_NOT_CREATED": 6};

    readonly property Item root_id: hookEntryPoint.root_id
    readonly property string pluginStore_dir: AthenaHook.pluginsPath // "file:///home/root/.xochitlPlugins"
    readonly property string pluginStore_jsonc: AthenaHook.env("HOME") + "/.cache/xochitlPlugins.jsonc" //FIXME
    readonly property var defaultPlugins: [
            {"path": AthenaHook.rootPrefix + "usr/libexec/athenaXochitl/KernelSettings",
             "name": "KernelSettings", "type": "BUNDLE", "unhashable": true}]
    
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
        var path = plugin["path"] + "/main.qml";
        
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
        var f_inj_code = File.read(plugin["path"] + "/main.js");
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
    function loadPluginPROP(plugin, obj_id) {
        for (var key in plugin["data"]) {
            obj_id[key] = plugin["data"][key];
        }
    }
    function loadPlugin(plugin) {
        // Get injection point handle
        try {
            if (plugin["unhashable"] == true) {
                console.info("Plugin " + plugin["name"] + " is uncacheable due to security reasons.");
                plugin["hookid_c"] = [];
            }
            
            var objs = HKID.execute(root.root_id, plugin["hookid"], plugin["hookid_c"]);
            if (objs["hookid_c"][0].length == 0) {
                return retVal.PLUGIN_INVALID_HOOKID;
            }
            
            plugin["hookid_c"] = objs["hookid_c"];
            objs = objs["objects"];
        } catch (error) {
            console.warn("Could not execute HOOKID=" + JSON.stringify(plugin["hookid"]) + "; HOOKIDc=" + JSON.stringify(plugin["hookid_c"]) + ". " + error);
        }
        
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
    function parsePlugins(plugins, stopOnFail = false) {
        var ret = retVal.PLUGIN_SUCCESS;
        for (var ix in plugins) {
            var plugin = plugins[ix];
            if (true) { //config[plugin["name"]]["enabled"] == true) { //FIXME
                if (plugin["type"] == "BUNDLE") {
                    if (plugin["cache"] == undefined) { //Uncached
                        try {
                            var pluginName = plugin["name"];
                            plugins[ix]["cache"] = JSON.parse(File.read(plugin["path"] + "/manifest.json"));
                            for (var subPlugin of plugins[ix]["cache"]) {
                                subPlugin["path"] = plugin["path"] + "/" + subPlugin["name"];
                                subPlugin["name"] = pluginName + "/" + subPlugin["name"];
                            }
                            ret = parsePlugins(plugins[ix]["cache"]);
                        } catch (error) {
                            ret = retVal.PLUGIN_NO_SUCH_FILE;
                        }
                    } else {
                        ret = parsePlugins(plugin["cache"]);
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
                continue;
            } else if (ret == retVal.PLUGIN_DISABLED) {
                console.info("Plugin " + plugin["name"] + " is disabled.");
                continue;
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
            if (stopOnFail)
                return ret;
        }
        return ret;
    }
    /////////////////////////////////////////
    // Read plugins json
    /////////////////////////////////////////
    function readPlugins() {
        if (AthenaKernel.isAthena) {
            // Try to load plugins from cache
            var cache = {};
            try {
                cache = JSON.parse(File.read(pluginStore_jsonc));
                if (AthenaHook.pluginsHash == cache["hash"]) {
                    return parsePlugins(cache["plugins"], true);
                }
                console.warn("Rebuilding plugin cache due to hash mismatch.");
            } catch (error) {
                console.warn("Rebuilding plugin cache due to missing or malformed cache store.");
            }
        }
        
        // Rebuild all plugin cache from scratch
        var plugins = [];
        for (var plugin of defaultPlugins) {
            plugins.push(plugin);
        }
        if (AthenaKernel.isAthena) {
            for (var plugin of AthenaHook.pluginsList) {
                plugins.push({"path": root.pluginStore_dir + plugin, "name": plugin, "type": "BUNDLE"});
            }
            parsePlugins(plugins);
            File.write(pluginStore_jsonc, JSON.stringify({"hash": AthenaHook.pluginsHash, "plugins": plugins}));
        } else {
            parsePlugins(plugins);
        }
    }
}
