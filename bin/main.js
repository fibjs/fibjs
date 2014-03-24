var test = require("test");
var os = require('os');

run("assert_test.js");
run("expect_test.js");
run("collection_test.js");
run("buffer_test.js");
run("path_test.js");
run("utils_test.js");
run("coroutine_test.js");
run("fibmod_test.js");
run("trigger_test.js");
run("lock_test.js");
run("fs_test.js");
run("ms_test.js");
run("os_test.js");
run("process_test.js");
run("encoding_test.js");
run("module_test.js");
run("define_test.js");
run("net_test.js");
run("hash_test.js");
run("buffered_test.js");
run("url_test.js");
run("http_test.js");
run("mq_test.js");
run("rpc_test.js");
run("uuid_test.js");
run("gd_test.js");
run("zlib_test.js");
run("vm_test.js");
run("db_test.js");

run('BUG_simple_api_call.js');

test.run();