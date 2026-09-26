/*
 * ts_cache.h
 *
 * TypeScript to JavaScript cache layer
 * Caches stripped JS to avoid re-parsing TypeScript on subsequent loads
 *
 *  Created on: Dec 28, 2024
 *      Author: lion
 */

#pragma once

#include "Buffer.h"
#include "qstring.h"

namespace fibjs {

// Minimum file size to cache (10KB)
// Files smaller than this threshold are not cached because:
// - Cache read/write overhead exceeds strip time savings
// - Small files strip very quickly anyway
constexpr size_t TS_CACHE_MIN_SIZE = 10 * 1024;

/**
 * Compute the cache key of a TypeScript file
 *
 * The key is the content hash, so identical sources share one entry no matter where
 * they live, and a changed source simply maps to a different entry (nothing has to be
 * invalidated by path or mtime). Hashing the content is not a cost worth avoiding:
 * it runs at 12-16 GB/s, about 1% of what stripping the same bytes costs.
 *
 * @param data Pointer to the TypeScript source
 * @param length Length of the source in bytes
 * @return Cache key
 */
size_t ts_cache_key(const uint8_t* data, size_t length);

/**
 * Try to load cached JS for a TypeScript file
 * 
 * @param key Key returned by ts_cache_key()
 * @param ts_length Length of original TypeScript content (for size threshold check)
 * @param cached_js Output: cached JS content if found
 * @return true if cache hit, false if cache miss (or file too small)
 */
bool ts_cache_get(size_t key, size_t ts_length, obj_ptr<Buffer_base>& cached_js);

/**
 * Save stripped JS to cache (async, fire-and-forget)
 * Uses reference counting to avoid memory copy
 * 
 * @param key Key returned by ts_cache_key()
 * @param js_buf Buffer containing stripped JS content (will be held by ref count)
 */
void ts_cache_set(size_t key, Buffer_base* js_buf);

} // namespace fibjs
