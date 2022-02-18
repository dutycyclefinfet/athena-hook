.pragma library

function _getQMLObjectType(obj) {
    return obj.toString().replace(/\([0-9a-fx]+\)/g, "").replace(/_QML.*/g, "");
}
function _testQMLObjectType(rgxp, obj) {
    let objType = _getQMLObjectType(obj);
    if (rgxp.test(objType)) {
        return 1;
    }
    if (objType == "QQuickLoader") {
        if (obj.item !== null) {
            let val = rgxp.test(_getQMLObjectType(obj.item));
            if (val) {
                return 2;
            }
        }
    }
    return 0;
}

/////////////////////////////////////////
// Convert a list of compiled object chains into an array of Qt5 objects for injection
/////////////////////////////////////////
function _execute(item, hkid_c) {
    var objs = [];
    for (var ix in hkid_c) {
        var c_id = item;
        for (var iy in hkid_c[ix]) {
            c_id = c_id.children[hkid_c[ix][iy]];
        }
        objs.push(c_id);
    }
    return objs;
}
/////////////////////////////////////////
// Search for a single object chain
/////////////////////////////////////////
function _compileDump(item, tree, res, res_obj, hkid_tp) {
    for (var n=0; n<item.children.length; n+=1) {
        var child = item.children[n];
        
        if (_testQMLObjectType(hkid_tp, child) == 2) {
            res.push([].concat(tree, [n, 0]));
            res_obj.push(child.children[0]);
        } else if (_testQMLObjectType(hkid_tp, child) == 1) {
            res.push([].concat(tree, n));
            res_obj.push(child);
        } else {
            _compileDump(child, [].concat(tree, n), res, res_obj, hkid_tp);
        }
    }
}
function _compile(item, hkid) {
    var hkid_c_last_length = 0;
    var hkid_c = [];   // Create an empty array of arrays, each subarray will have a set of results.
                       // This variable will always have a single subarray, because it only gets appended
                       // at the "fresh" execution i.e. not when inside if(hkid_ix=="ALL"). When "ALL" is
                       // present, hkid_c_nest will get returned from inside the if(hkid_ix="ALL").
    for (var i in hkid) {
        var hkid_tp = new RegExp(hkid[i][0]); // Object type regex
        var hkid_ix = hkid[i][1];             // Object index (e.g. third Rectangle: ["Rectangle", 2]),
                                              // "ALL" for all objects of the type hkid_tp under the current parent
                                              // or "DUMP" to dump all first occurences of an object when traveling
                                              // along the tree of QtObjects
        if (hkid_ix == "ALL") {
            var hkid_c_nest = [];
            for (var n=0; n<item.children.length; n+=1) { // Gather all instances of the searched object and recursively rerun the hkid analysis
                var child = item.children[n];
                var hkid_c_glue = [n];
                // Copy the existing array, then push it with a copy of the recursion result. After looping over
                // all children we will return this array of arrays, because there is nothing more to be done.
                // Everything was already done deeper in the recursion and we are simply exiting upwards in the stack.
                if (_testQMLObjectType(hkid_tp, child) == 2) {
                    child = child.children[0];
                    hkid_c_glue.push(0);
                }
                if (_testQMLObjectType(hkid_tp, child) == 1) {
                    for (var hkid_c_rec of _compile(child, hkid.slice(i+1, hkid.length))) {
                        if (hkid_c_rec.length) {
                            hkid_c_nest.push([].concat(hkid_c, hkid_c_glue, hkid_c_rec));
                        }
                    }
                }
            }
            return hkid_c_nest;
        } else if (hkid_ix == "DUMP") {
            var hkid_c_nest = [];
            var dump_tree = [];
            var dump_hkidc = [];
            var dump_items = [];
            _compileDump(item, dump_tree, dump_hkidc, dump_items, hkid_tp);
            
            if (dump_hkidc.length && dump_items.length) {
                for (var n in dump_items) {
                    for (var hkid_c_rec of _compile(dump_items[n], hkid.slice(i+1, hkid.length))) {
                        if (hkid_c_rec.length) {
                            hkid_c_nest.push([].concat(hkid_c, dump_hkidc[n], hkid_c_rec));
                        }
                    }
                }
            }
            return hkid_c_nest;
        } else {
            var n_of_type = 0;
            for (var n=0; n<item.children.length; n+=1) {
                var child = item.children[n];
                if (_testQMLObjectType(hkid_tp, child)) {
                    if (hkid_ix == n_of_type) { // Is this the correct object hkid_ix-th of the type hkid_tp?
                        item = child;
                        hkid_c.push(n); // Push to the array of arrays, but first dereference it.
                        
                        if (_testQMLObjectType(hkid_tp, child) == 2) {
                            hkid_c.push(0);
                        }
                        break;
                    }
                    n_of_type++;
                }
            }
        }
        if (hkid_c.length <= hkid_c_last_length) {
            return [];
        }
        hkid_c_last_length = hkid_c.length;
    }
    return [hkid_c];
}
/////////////////////////////////////////
// Search for a single object chain
/////////////////////////////////////////
function execute(item, hkid, hkid_c) {
    if (!hkid_c || !hkid_c.length) {
        hkid_c = _compile(item, hkid);
        hkid_c = (hkid_c.length) ? hkid_c : [[]];
    }
    return {objects: _execute(item, hkid_c), hookid_c: hkid_c};
}
