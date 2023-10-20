/*
 * modules.cpp
 *
 *  Created on: Oct 16, 2021
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

void importBuiltinModule()
{
    IMPORT_MODULE(assert);
    IMPORT_MODULE(base32);
    IMPORT_MODULE(base58);
    IMPORT_MODULE(base64);
    IMPORT_MODULE(child_process);
    IMPORT_MODULE(coroutine);
    IMPORT_MODULE(console);
    IMPORT_MODULE(constants);
    IMPORT_MODULE(crypto);
    IMPORT_MODULE(db);
    IMPORT_MODULE(dgram);
    IMPORT_MODULE(dns);
    IMPORT_MODULE(encoding);
    IMPORT_MODULE(events);
    IMPORT_MODULE(fasttext);
    IMPORT_MODULE(fs);
    IMPORT_MODULE(gd);
    IMPORT_MODULE(gui);
    IMPORT_MODULE(hash);
    IMPORT_MODULE(hex);
    IMPORT_MODULE(http);
    IMPORT_MODULE(https);
    IMPORT_MODULE(iconv);
    IMPORT_MODULE(io);
    IMPORT_MODULE(json);
    IMPORT_MODULE(msgpack);
    IMPORT_MODULE(mq);
    IMPORT_MODULE(multibase);
    IMPORT_MODULE(net);
    IMPORT_MODULE(os);
    IMPORT_MODULE(path);
    IMPORT_MODULE(perf_hooks);
    IMPORT_MODULE(process);
    IMPORT_MODULE(profiler);
    IMPORT_MODULE(punycode);
    IMPORT_MODULE(querystring);
    IMPORT_MODULE(ssl);
    IMPORT_MODULE(tls);
    IMPORT_MODULE(string_decoder);
    IMPORT_MODULE(test);
    IMPORT_MODULE(timers);
    IMPORT_MODULE(tty);
    IMPORT_MODULE(url);
    IMPORT_MODULE(util);
    IMPORT_MODULE(uuid);
    IMPORT_MODULE(vm);
    IMPORT_MODULE(worker_threads);
    IMPORT_MODULE(ws);
    IMPORT_MODULE(xml);
    IMPORT_MODULE(zip);
    IMPORT_MODULE(zlib);

#ifdef _WIN32
    IMPORT_MODULE(registry);
#endif
}
}
