var test = require("test");
var os = require('os');

run("assert_test.js");
run("expect_test.js");
run("collection_test.js");
run("buffer_test.js");
run("path_test");
run("utils_test");
run("coroutine_test");
run("fibmod_test");
run("trigger_test");
run("lock_test");
run("fs_test");
run("ms_test");
run("os_test");
run("process_test");
run("encoding_test");
run("module_test");
run("define_test");
run("net_test");
run("hash_test");
run("buffered_test");
run("url_test");
run("http_test");
run("mq_test");
run("rpc_test");
run("uuid_test");
run("gd_test");
run("zlib_test");
run("vm_test");
run("db_test");

run('BUG_simple_api_call');

test.run();