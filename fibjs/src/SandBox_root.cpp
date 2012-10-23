/*
 * SandBox_root.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/assert.h"
#include "ifs/path.h"
#include "ifs/os.h"
#include "ifs/coroutine.h"
#include "ifs/encoding.h"

#include "ifs/process.h"

#include "ifs/io.h"
#include "ifs/fs.h"
#include "ifs/net.h"
#include "ifs/zmq.h"
#include "ifs/vm.h"

#include "ifs/mq.h"

#include "ifs/db.h"
#include "ifs/hash.h"
#include "ifs/zlib.h"

#include "ifs/gd.h"

#include "ifs/uuid.h"
#include "ifs/re.h"
#include "ifs/collection.h"

#include "ifs/http.h"

#include "ifs/rpc.h"

namespace fibjs
{

void SandBox::initRoot()
{
	date_t now;
	now.now();

	InstallNativeModule("encoding", encoding_base::class_info(), now);
	InstallNativeModule("uuid", uuid_base::class_info(), now);

	InstallNativeModule("re", re_base::class_info(), now);
	InstallNativeModule("collection", collection_base::class_info(), now);

	InstallNativeModule("assert", assert_base::class_info(), now);
	InstallNativeModule("path", path_base::class_info(), now);

	InstallNativeModule("coroutine", coroutine_base::class_info(), now);
	InstallNativeModule("process", process_base::class_info(), now);

	InstallNativeModule("vm", vm_base::class_info(), now);
	InstallNativeModule("io", io_base::class_info(), now);
	InstallNativeModule("fs", fs_base::class_info(), now);
	InstallNativeModule("os", os_base::class_info(), now);
	InstallNativeModule("net", net_base::class_info(), now);
	InstallNativeModule("zmq", zmq_base::class_info(), now);

	InstallNativeModule("mq", mq_base::class_info(), now);

	InstallNativeModule("http", http_base::class_info(), now);
	InstallNativeModule("rpc", rpc_base::class_info(), now);

	InstallNativeModule("db", db_base::class_info(), now);
	InstallNativeModule("hash", hash_base::class_info(), now);
	InstallNativeModule("zlib", zlib_base::class_info(), now);

	InstallNativeModule("gd", gd_base::class_info(), now);
}

}
