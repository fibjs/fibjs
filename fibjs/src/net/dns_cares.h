/*
 * dns_cares.h
 *
 * The resolver side of the dns module (c-ares).  The implementation lives in
 * dns_cares.cpp; lookup/lookupService stay in dns.cpp and use libuv.
 */

#pragma once

#include "object.h"

namespace fibjs {

class Isolate;

// Per-isolate default resolver state (channel + default result order).
result_t dns_cares_get_default_order(Isolate* isolate, exlib::string& retVal);
result_t dns_cares_set_default_order(Isolate* isolate, exlib::string order);
void dns_cares_cleanup_isolate(Isolate* isolate);

} // namespace fibjs
