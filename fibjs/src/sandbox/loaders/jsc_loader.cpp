/*
 * jsc_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Buffer.h"
#include "ifs/zlib.h"
#include "loaders.h"
#include "version.h"
#include "v8/src/api/api-inl.h"
#include "v8/src/snapshot/code-serializer.h"
#include "v8/src/snapshot/snapshot-utils.h"
#include "v8/src/snapshot/snapshot.h"
#include "v8/src/base/vector.h"

namespace fibjs {

result_t jsc_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    result_t hr;

    obj_ptr<Buffer_base> unz;
    hr = zlib_base::cc_gunzip(src, -1, unz);
    if (hr < 0)
        return hr;

    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewString(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    Buffer* code = Buffer::Cast(unz);

    int32_t code_len = (int32_t)code->length() - sizeof(int32_t);

    if (*(int32_t*)(code->data() + code_len) != jsc_version)
        return CHECK_ERROR(Runtime::setError("SandBox: bad jsc version."));

    code_len -= sizeof(int32_t);

    exlib::string s_temp_source;
    int32_t src_len = *(int32_t*)(code->data() + code_len);
    int32_t i;

    s_temp_source.resize(src_len);
    char* _temp_source = s_temp_source.data();

    for (i = 0; i < src_len; i++)
        _temp_source[i] = '.';

    code_len -= sizeof(int32_t);
    int32_t line_count = *(int32_t*)(code->data() + code_len);

    code_len -= sizeof(int32_t) * line_count;

    int32_t pos = 0;
    int32_t* p = (int32_t*)(code->data() + code_len);

    for (i = 0; i < line_count; i++) {
        pos += p[i];
        if (pos >= src_len)
            break;
        _temp_source[pos] = '\n';
        pos++;
    }

    {
        TryCatch try_catch;

        // Create placeholder source string
        v8::Local<v8::String> placeholder_source = isolate->NewString(s_temp_source);
        
        // Calculate correct source hash using V8's internal API
        v8::internal::Isolate* i_isolate = reinterpret_cast<v8::internal::Isolate*>(isolate->m_isolate);
        v8::internal::DirectHandle<v8::internal::String> i_source = v8::Utils::OpenDirectHandle(*placeholder_source);
        
        // Use empty wrapped args (null) to match compilation time
        // The script was compiled without wrapped_arguments in util_compile.cpp
        v8::internal::DirectHandle<v8::internal::FixedArray> i_wrapped_args;  // empty/null handle
        
        v8::ScriptOriginOptions origin_options;
        uint32_t correct_hash = v8::internal::SerializedCodeData::SourceHash(
            i_source, i_wrapped_args, origin_options);
        
        // Make a writable copy of the cache data to fix the source hash and other checksums
        exlib::string cache_data_copy((const char*)code->data(), code_len);
        uint8_t* writable_cache = (uint8_t*)cache_data_copy.data();
        
        // SerializedCodeData header layout (all uint32_t):
        // Offset 0:  kMagicNumberOffset
        // Offset 4:  kVersionHashOffset
        // Offset 8:  kSourceHashOffset
        // Offset 12: kFlagHashOffset
        // Offset 16: kReadOnlySnapshotChecksumOffset
        // Offset 20: kPayloadLengthOffset
        // Offset 24: kChecksumOffset
        // Offset 28: kHeaderSize (pointer aligned)
        
        const uint32_t kSourceHashOffset = 8;
        const uint32_t kReadOnlySnapshotChecksumOffset = 16;
        const uint32_t kChecksumOffset = 24;
        const uint32_t kHeaderSize = 32;  // POINTER_SIZE_ALIGN(28) on 64-bit = 32
        
        // Update the source hash in the cache data
        uint32_t* hash_ptr = (uint32_t*)(writable_cache + kSourceHashOffset);
        *hash_ptr = correct_hash;
        
        // Update the ReadOnly snapshot checksum to match current isolate's snapshot
        uint32_t expected_ro_checksum = v8::internal::Snapshot::ExtractReadOnlySnapshotChecksum(
            i_isolate->snapshot_blob());
        uint32_t* ro_checksum_ptr = (uint32_t*)(writable_cache + kReadOnlySnapshotChecksumOffset);
        *ro_checksum_ptr = expected_ro_checksum;
        
        // Recalculate checksum over the payload (everything after header)
        // Only needed if v8_flags.verify_snapshot_checksum is true
        uint32_t new_checksum = v8::internal::Checksum(
            v8::base::Vector<const uint8_t>(writable_cache + kHeaderSize, code_len - kHeaderSize));
        uint32_t* checksum_ptr = (uint32_t*)(writable_cache + kChecksumOffset);
        *checksum_ptr = new_checksum;
        
        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData(writable_cache, code_len);

        v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
        pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
        v8::ScriptOrigin so_origin(soname, -1, 0, false,
            -1, v8::Local<v8::Value>(), false, false, false, pargs);

        v8::ScriptCompiler::Source source(placeholder_source, so_origin, cache);

        script = v8::ScriptCompiler::Compile(isolate->context(), &source,
            v8::ScriptCompiler::kConsumeCodeCache)
                     .FromMaybe(v8::Local<v8::Script>());

        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
        
        if (cache->rejected) {
            return CHECK_ERROR(Runtime::setError("SandBox: code cache was rejected."));
        }
    }

    return 0;
}
}