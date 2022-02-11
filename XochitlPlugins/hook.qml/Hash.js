.pragma library

function get(s) {
    var h = 0;
    if (s.length == 0) {
        return h;
    }
    for (var i = 0; i < s.length; i++) {
        var c = s.charCodeAt(i);
        h = ((h<<5) - h) + c;
        h = h & h;
    }
    return h;
}
