#!/usr/local/bin/fibjs

global.full_test = process.argv.indexOf('--full') >= 0;

const CI_SUBPROCESS_CHECK = !!process.env.CI_SUBPROCESS_CHECK;

run("./assert_test.js");
run("./assertion_error_test.js");
run("./test_test.js");
run("./class_test.js");

run("./abortcontroller_test.js");
run("./async_local_storage_test.js");
run("./async_resource_test.js");
run("./headers_test.js");
run("./structuredClone_test.js");
run("./message_channel_test.js");
run("./webstream_test.js");

run("./perf_test.js");
run("./console_test.js");
run("./punycode_test.js");
run("./timer_test.js");
run("./buffer_test.js");
run("./encoding_test.js");
run("./atob_btoa_test.js");
run("./path_test.js");
run("./path_match_test.js");
run("./util_test.js");
run("./ts_strip_test.js");
run("./types_test.js");
run("./promise_test.js");
run("./microtask_test.js");
run("./xml_test.js");
run("./querySelector_test.js");

if (process.env.CI)
    run("./xml_suite.js");

run("./addons_test.js");

run("./coroutine_test.js");
run("./stack_test.js");
run("./fibmod_test.js");
run("./trigger_test.js");
run("./lock_test.js");
run("./fs_test.js");
run("./fs_stream_test.js");
run("./fs_glob_test.js");
run("./fswatch_test.js");
run("./ms_test.js");
run("./io_test.js");
run("./os_test.js");
run("./process_test.js");
run("./child_process_test.js");
run("./eval_test.js");
run("./json_test.js");
run("./module_test.js");
run("./esm_test.js");
run("./net_test.js");
run("./dgram_test.js");
run("./buffered_test.js");
run("./hash_test.js");
run("./crypto_test.js");
run("./webcrypto_test.js");
run("./bbs_test.js");
run("./tls_test.js");
run("./string_decoder_test.js");
run("./readline_test.js");
run("./url_search_params_test.js");
run("./blob_test.js");
run("./formdata_test.js");
run("./url_test.js");
run("./querystring_test.js");
run("./mime_test.js");
run("./http_test.js");
run("./http2_test.js");
run("./fetch_test.js");
run("./sse_test.js");
run("./mq_test.js");
run("./rtc_test.js");

if (process.platform != "linux" && process.platform != "android")
    run("./gui_test.js");

run("./registry_test.js");
run("./uuid_test.js");
run("./zlib_test.js");
run("./unzip_test.js");
run("./ws_test.js");
run("./vm_test.js");
run("./db_test.js");
run("./vec_test.js");
run("./wasm_test.js");

if (process.platform != "android") {
    run("./dns_test.js");
    run("./ts_test.js");
    run("./icu_test.js");
    run("./workspaces_test.js");
    run("./scripts_test.js");
    run("./selfzip_test.js");
}

if (global.full_test)
    run("./tty_test.js");

run("./v8_test.js");

run("./v8_bug_test.js");

run("./getter_throw.js")

run("./internal_test/helpers.js")

if (process.env.CI && process.platform != "android") {
    run("./opt_tools_test/index.js")
}
