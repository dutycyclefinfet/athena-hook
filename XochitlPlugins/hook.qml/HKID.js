.pragma library

function _getQMLObjectType(obj) {
    return obj.toString().replace(/\([0-9a-fx]+\)/g, "").replace(/_QML.*/g, "");
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
function _compile(item, hkid) {
    var hkid_c = [];   // Create an empty array of arrays, each subarray will have a set of results.
                       // This variable will always have a single subarray, because it only gets appended
                       // at the "fresh" execution i.e. not when inside if(hkid_ix=="ALL"). When "ALL" is
                       // present, hkid_c_nest will get returned from inside the if(hkid_ix="ALL").
    for (var i in hkid) {
        var hkid_tp = new RegExp(hkid[i][0]); // Object type regex
        var hkid_ix = hkid[i][1];             // Object index (e.g. third Rectangle: ["Rectangle", 2])
                                              // or "ALL" for all objects of the type hkid_tp
        if (hkid_ix == "ALL") {
            var hkid_c_nest = [];
            for (var n in item.children) { // Gather all instances of the searched object and recursively rerun the hkid analysis
                var child = item.children[n];
                if (hkid_tp.test(_getQMLObjectType(child))) {
                    // Copy the existing array, then push it with a copy of the recursion result. After looping over
                    // all children we will return this array of arrays, because there is nothing more to be done.
                    // Everything was already done deeper in the recursion and we are simply exiting upwards in the stack.
                    for (var hkid_c_rec of _compile(child, hkid.slice(i, hkid.length))) {
                        if (hkid_c.length) {
                            hkid_c_nest.push([].concat(hkid_c, [n], hkid_c_rec));
                        }
                    }
                }
            }
            return hkid_c_nest;
        } else {
            var n_of_type = 0;
            for (var n in item.children) {
                var child = item.children[n];
                if (hkid_tp.test(_getQMLObjectType(child))) {
                    if (hkid_ix == n_of_type) { // Is this the correct object hkid_ix-th of the type hkid_tp?
                        item = child;
                        hkid_c.push(n); // Push to the array of arrays, but first dereference it.
                        break;
                    }
                    n_of_type++;
                }
            }
        }
    }
    return (hkid_c.length==hkid.length) ? [hkid_c] : [];
}
/////////////////////////////////////////
// Search for a single object chain
/////////////////////////////////////////
function execute(pid, hkid, hkid_c) {
    if (!hkid_c || !hkid_c.length) {
        hkid_c = _compile(pid, hkid);
        hkid_c = (hkid_c.length) ? hkid_c : [[]];
    }
    return {objects: _execute(pid, hkid_c), hookid_c: hkid_c};
}
