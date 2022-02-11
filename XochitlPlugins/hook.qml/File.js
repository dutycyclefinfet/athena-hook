.pragma library

function read(fn, cb, cb_err) {
    var xhr = new XMLHttpRequest;
    xhr.open("GET", fn, false);
    if (cb) {
        xhr.onreadystatechange = function() {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                if (xhr.responseText) {
                    cb(xhr.responseText);
                } else if (cb_err) {
                    cb_err(retVal.PLUGIN_NO_SUCH_FILE);
                }
            }
        };
        xhr.onerror = function() {
            if (cb_err) {
                cb_err(retVal.PLUGIN_NO_SUCH_FILE);
            }
        }
        xhr.send();
    } else {
        xhr.send();
        return xhr.responseText;
    }
}
function write(fn, data) {
    var xhr = new XMLHttpRequest;
    xhr.open("PUT", fn);
    xhr.send(data);
}
