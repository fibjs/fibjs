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
 * Compute hash for TypeScript content
 * 
 * @param ts_content TypeScript content
 * @param ts_length Length of content
 * @return Hash value for cache key
 */
size_t ts_cache_hash(const uint8_t* ts_content, size_t ts_length);

/**
 * Try to load cached JS for a TypeScript file
 * 
 * @param hash Hash of original TypeScript content
 * @param ts_length Length of original TypeScript content (for size threshold check)
 * @param cached_js Output: cached JS content if found
 * @return true if cache hit, false if cache miss (or file too small)
 */
bool ts_cache_get(size_t hash, size_t ts_length, obj_ptr<Buffer_base>& cached_js);

/**
 * Save stripped JS to cache (async, fire-and-forget)
 * Uses reference counting to avoid memory copy
 * 
 * @param hash Hash of original TypeScript content
 * @param js_buf Buffer containing stripped JS content (will be held by ref count)
 */
void ts_cache_set(size_t hash, Buffer_base* js_buf);

} // namespace fibjs
