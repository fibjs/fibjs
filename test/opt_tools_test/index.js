var test = require("test");
test.setup();

if (process.env.CI) {
    require('./install_from_pkgjson');
    require('./install_raw');
}

require.main === module && test.run(console.DEBUG);