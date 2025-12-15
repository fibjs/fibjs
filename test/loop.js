var child_process = require('child_process');
var test = require('test');

while (1) {
    gc();
    run('./tls_test.js');
}