#!/usr/local/bin/fibjs

var test = require("test");
test.setup();

var coroutine = require('coroutine');

global.full_test = process.argv.indexOf('full') >= 0;

run("./assert_test.js");
run("./expect_test.js");
run("./test_test.js");
run("./console_test.js");
run("./collection_test.js");
run("./re_test.js");
run("./int64_test.js");
run("./timer_test.js");
run("./buffer_test.js");
run("./path_test.js");
run("./util_test.js");
run("./xml_test.js");

if (global.full_test)
    run("./xml_suite.js");

run("./coroutine_test.js");
run("./fibmod_test.js");
run("./trigger_test.js");
run("./lock_test.js");
run("./fs_test.js");
run("./rados_test.js");
run("./ms_test.js");
run("./os_test.js");
run("./process_test.js");
run("./encoding_test.js");
run("./json_test.js");
run("./module_test.js");
run("./net_test.js");
run("./buffered_test.js");
run("./hash_test.js");
run("./crypto_test.js");
run("./ssl_test.js");
run("./url_test.js");
run("./querystring_test.js");
run("./http_test.js");
run("./mq_test.js");
run("./gui_test.js");
run("./registry_test.js");
run("./uuid_test.js");
run("./gd_test.js");
run("./zlib_test.js");
run("./unzip_test.js");
run("./ws_test.js");
run("./vm_test.js");
run("./db_test.js");

if (global.full_test) {
    run("./mongo_test.js");
    run("./redis_test.js");
}

run("./profiler_test.js");

run("./BUG_simple_api_call.js");

process.exit(-test.run());