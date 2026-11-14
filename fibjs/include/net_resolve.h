/*
 * net_resolve.h
 *
 * Internal address resolution used by the connection paths.  It deliberately
 * sits on dns.lookup (libuv), not on the c-ares record API: net/tls/http all
 * need a socket-ready address, not a DNS record set.
 */

#pragma once

#include "object.h"
#include "AsyncCall.h"

namespace fibjs {

struct ResolvedHost {
    exlib::string address;
    int32_t family = 0; // 4 or 6
};

// The connect paths pass family 0 or 4/6 and hints from dns.ADDRCONFIG/ALL/
// V4MAPPED.  order is "verbatim", "ipv4first" or "ipv6first"; an empty order
// keeps the isolate's configured default.  The lookup blocks the calling
// fiber, matching the old cc_resolve sequencing.
result_t resolve_host(const exlib::string& host, int32_t family, int32_t hints,
    const exlib::string& order, ResolvedHost& retVal, Isolate* isolate);

// Node's net.connect adds ADDRCONFIG when neither family nor hints are given
// on non-Windows.  dns.resolve keeps its own caller-supplied hints.
int32_t connect_lookup_hints(int32_t family, int32_t hints);

// Check the getaddrinfo flags accepted by dns.lookup.
bool valid_lookup_hints(int32_t hints);

} // namespace fibjs
