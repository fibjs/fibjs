/*
 * SandBox_root.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/assert.h"
#include "ifs/test.h"

#include "ifs/path.h"
#include "ifs/os.h"
#include "ifs/coroutine.h"
#include "ifs/encoding.h"

#include "ifs/process.h"

#include "ifs/io.h"
#include "ifs/fs.h"
#include "ifs/net.h"
#include "ifs/vm.h"

#include "ifs/hash.h"
#include "ifs/crypto.h"
#include "ifs/ssl.h"

#include "ifs/websocket.h"

#include "ifs/mq.h"
#include "ifs/xml.h"

#include "ifs/db.h"
#include "ifs/zlib.h"

#include "ifs/gd.h"
#include "ifs/profiler.h"

#include "ifs/uuid.h"
#include "ifs/re.h"
#include "ifs/collection.h"

#include "ifs/http.h"

#include "ifs/rpc.h"

#include "ifs/util.h"

namespace fibjs
{

void SandBox::initRoot()
{
    Isolate* isolate = holder();

    InstallModule("encoding", encoding_base::class_info().getFunction(isolate));

    InstallModule("base32", base32_base::class_info().getFunction(isolate));
    InstallModule("base64", base64_base::class_info().getFunction(isolate));
    InstallModule("base64vlq", base64vlq_base::class_info().getFunction(isolate));
    InstallModule("bson", bson_base::class_info().getFunction(isolate));
    InstallModule("hex", hex_base::class_info().getFunction(isolate));
    InstallModule("iconv", iconv_base::class_info().getFunction(isolate));
    InstallModule("json", json_base::class_info().getFunction(isolate));

    InstallModule("uuid", uuid_base::class_info().getFunction(isolate));

    InstallModule("re", re_base::class_info().getFunction(isolate));
    InstallModule("collection", collection_base::class_info().getFunction(isolate));

    InstallModule("test", test_base::class_info().getFunction(isolate));
    InstallModule("assert", assert_base::class_info().getFunction(isolate));
    InstallModule("expect", createV8Function("expect", isolate->m_isolate, test_base::s_expect));

    InstallModule("path", path_base::class_info().getFunction(isolate));

    InstallModule("coroutine", coroutine_base::class_info().getFunction(isolate));
    InstallModule("process", process_base::class_info().getFunction(isolate));

    InstallModule("vm", vm_base::class_info().getFunction(isolate));
    InstallModule("io", io_base::class_info().getFunction(isolate));
    InstallModule("fs", fs_base::class_info().getFunction(isolate));
    InstallModule("os", os_base::class_info().getFunction(isolate));
    InstallModule("net", net_base::class_info().getFunction(isolate));

    InstallModule("websocket", websocket_base::class_info().getFunction(isolate));

    InstallModule("hash", hash_base::class_info().getFunction(isolate));
    InstallModule("crypto", crypto_base::class_info().getFunction(isolate));
    InstallModule("ssl", ssl_base::class_info().getFunction(isolate));

    InstallModule("mq", mq_base::class_info().getFunction(isolate));
    InstallModule("xml", xml_base::class_info().getFunction(isolate));

    InstallModule("http", http_base::class_info().getFunction(isolate));
    InstallModule("rpc", rpc_base::class_info().getFunction(isolate));

    InstallModule("db", db_base::class_info().getFunction(isolate));
    InstallModule("zlib", zlib_base::class_info().getFunction(isolate));

    InstallModule("gd", gd_base::class_info().getFunction(isolate));

    InstallModule("profiler", profiler_base::class_info().getFunction(isolate));
    InstallModule("util", util_base::class_info().getFunction(isolate));
}

}
