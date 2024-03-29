var child_process = require('child_process');
var test = require('test');
test.setup();

while (1) {
    gc();
    run('./tls_test.js');
    var result = test.run();
    if (result.failed > 0)
        break;
}