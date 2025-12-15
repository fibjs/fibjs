/*
 * cts_loader.cpp
 *
 *  Created on: Sep 19, 2024
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"
#include "ifs/util.h"
#include "v8/src/api/api-inl.h"
#include "v8/src/snapshot/code-serializer.h"
#include "v8/src/snapshot/snapshot-utils.h"
#include "v8/src/snapshot/snapshot.h"
#include "v8/src/base/vector.h"

namespace fibjs {

result_t cts_Loader::ts_compile(Isolate* isolate, Buffer_base* src, obj_ptr<Buffer_base>& retVal)
{
    exlib::string _src;
    src->toString(_src);

    exlib::string _dest;
    result_t hr = util_base::stripTypeScript(_src, _dest);
    if (hr < 0)
        return hr;

    retVal = new Buffer(_dest.c_str(), _dest.length());

    return 0;
}

result_t cts_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();

    exlib::string ts_source;
    src->toString(ts_source);

    exlib::string js_source;
    result_t hr = util_base::stripTypeScript(ts_source, js_source);
    if (hr < 0)
        return hr;

    v8::Local<v8::String> soname = isolate->NewString(name);

    // Handle shebang
    if (ts_source.length() > 2 && ts_source[0] == '#' && ts_source[1] == '!') {
        ts_source[0] = '/';
        ts_source[1] = '/';
        js_source[0] = '/';
        js_source[1] = '/';
    }

    // Wrap source with module arguments
    exlib::string ts_wrapped = arg_names + "\n" + ts_source + "\n});";
    exlib::string js_wrapped = arg_names + "\n" + js_source + "\n});";

    TryCatch try_catch;

    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
    pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
    v8::ScriptOrigin so_origin(soname, -1, 0, false,
        -1, v8::Local<v8::Value>(), false, false, false, pargs);

    // Compile JS to get code cache
    v8::Local<v8::String> v8_js_source = isolate->NewString(js_wrapped);
    v8::ScriptCompiler::Source js_script_source(v8_js_source, so_origin);

    v8::Local<v8::UnboundScript> unbound = v8::ScriptCompiler::CompileUnboundScript(
        isolate->m_isolate, &js_script_source, v8::ScriptCompiler::kEagerCompile)
                                               .FromMaybe(v8::Local<v8::UnboundScript>());

    if (unbound.IsEmpty())
        return throwSyntaxError(try_catch);

    // Create code cache from compiled JS
    const v8::ScriptCompiler::CachedData* cache = v8::ScriptCompiler::CreateCodeCache(unbound);

    // Calculate correct source hash for TS source
    v8::Local<v8::String> v8_ts_source = isolate->NewString(ts_wrapped);
    v8::internal::Isolate* i_isolate = reinterpret_cast<v8::internal::Isolate*>(isolate->m_isolate);
    v8::internal::DirectHandle<v8::internal::String> i_source = v8::Utils::OpenDirectHandle(*v8_ts_source);
    v8::internal::DirectHandle<v8::internal::FixedArray> i_wrapped_args;
    v8::ScriptOriginOptions origin_options;
    uint32_t ts_hash = v8::internal::SerializedCodeData::SourceHash(
        i_source, i_wrapped_args, origin_options);

    // Make a writable copy of the cache data and fix the source hash
    exlib::string cache_data_copy((const char*)cache->data, cache->length);
    uint8_t* writable_cache = (uint8_t*)cache_data_copy.data();

    // SerializedCodeData header layout (all uint32_t):
    // Offset 0:  kMagicNumberOffset
    // Offset 4:  kVersionHashOffset
    // Offset 8:  kSourceHashOffset
    // Offset 12: kFlagHashOffset
    // Offset 16: kReadOnlySnapshotChecksumOffset
    // Offset 20: kPayloadLengthOffset
    // Offset 24: kChecksumOffset
    // Offset 28: kHeaderSize (pointer aligned to 32 on 64-bit)

    const uint32_t kSourceHashOffset = 8;
    const uint32_t kReadOnlySnapshotChecksumOffset = 16;
    const uint32_t kChecksumOffset = 24;
    const uint32_t kHeaderSize = 32;

    // Update the source hash in the cache data
    *(uint32_t*)(writable_cache + kSourceHashOffset) = ts_hash;

    // Update the ReadOnly snapshot checksum to match current isolate's snapshot
    uint32_t expected_ro_checksum = v8::internal::Snapshot::ExtractReadOnlySnapshotChecksum(
        i_isolate->snapshot_blob());
    *(uint32_t*)(writable_cache + kReadOnlySnapshotChecksumOffset) = expected_ro_checksum;

    // Recalculate checksum over the payload
    uint32_t new_checksum = v8::internal::Checksum(
        v8::base::Vector<const uint8_t>(writable_cache + kHeaderSize, cache->length - kHeaderSize));
    *(uint32_t*)(writable_cache + kChecksumOffset) = new_checksum;

    // Load with TS source and modified code cache
    v8::ScriptCompiler::CachedData* ts_cache = new v8::ScriptCompiler::CachedData(
        writable_cache, cache->length);
    v8::ScriptCompiler::Source ts_script_source(v8_ts_source, so_origin, ts_cache);

    script = v8::ScriptCompiler::Compile(isolate->context(), &ts_script_source,
        v8::ScriptCompiler::kConsumeCodeCache)
                 .FromMaybe(v8::Local<v8::Script>());

    delete cache;

    // If code cache was rejected, V8 will try to compile TS source directly
    // which will fail. Fall back to compiling JS source.
    if (ts_cache->rejected || script.IsEmpty()) {
        v8::ScriptCompiler::Source js_fallback_source(v8_js_source, so_origin);
        script = v8::Script::Compile(isolate->context(), v8_js_source, &so_origin)
                     .FromMaybe(v8::Local<v8::Script>());
    }

    if (script.IsEmpty())
        return throwSyntaxError(try_catch);

    return 0;
}
}