#include "object.h"
#include "Buffer.h"
#include "Blob.h"
#include "SandBox.h"
#include "encoding.h"

#include <cmath>
#include "simdutf.h"

#if defined(__SSE2__)
#include <emmintrin.h>
#elif defined(__aarch64__)
#include <arm_neon.h>
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fibjs {

// 定义在 class_byteLength 之后；这里前置声明给「未知编码按 utf8 计长」用
inline bool is_native_codec(exlib::string codec);

inline result_t generateEnd(const int32_t buffer_length, const int32_t offset, int32_t& end)
{
    if (end < 0)
        end = buffer_length + end + 1;

    if (offset < 0 || end < 0 || offset > end)
        return result_t::invalid_arg();

    if (end > buffer_length)
        return result_t::out_of_range();

    return result_t();
}

inline result_t validOffset(const int32_t buffer_length, const int32_t offset)
{
    if (offset < 0)
        return result_t::invalid_arg();

    if (offset > buffer_length)
        return result_t::out_of_range();

    return result_t();
}

// the integer fill form: the merged entry dispatches here
static result_t alloc_fill_int32(int32_t size, int32_t fill, obj_ptr<Buffer_base>& retVal)
{
    if (size < 0)
        size = 0;

    if (fill == 0) {
        // calloc-backed storage: already zero filled, no eager memset needed
        // (pages the kernel hands back are zeroed on first touch)
        retVal = new Buffer(NULL, size, true);
        return result_t();
    }

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    buf->fill(fill, 0, size, retVal);
    retVal = buf;
    return result_t();
}

result_t Buffer_base::alloc(int32_t size, Union_alloc_fill fill, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<int32_t>(fill))
        return alloc_fill_int32(size, std::get<int32_t>(fill), retVal);

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(std::get<obj_ptr<Buffer_base>>(fill).get(), 0, size, retVal);
}

result_t Buffer_base::alloc(int32_t size, exlib::string fill, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(fill, 0, size, codec, retVal);
}

result_t Buffer_base::allocUnsafe(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return result_t();
}

result_t Buffer_base::allocUnsafeSlow(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return result_t();
}

result_t Buffer_base::from(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, datas->Length());
    retVal = buf;

    int32_t sz = (int32_t)buf->length();
    v8::Local<v8::Context> context;
    if (!datas->GetCreationContext().ToLocal(&context))
        context = Isolate::current()->context();
    Isolate* isolate = Isolate::current(context);

    if (sz) {
        int32_t i;
        result_t hr;
        uint8_t* _str = buf->data();

        for (i = 0; i < sz; i++) {
            JSValue v = datas->Get(context, i);
            int32_t num;

            hr = GetArgumentValue(isolate, v, num);
            if (hr.failed())
                return hr;

            _str[i] = (uint8_t)num;
        }
    }

    return result_t();
}

result_t Buffer_base::from(Buffer_base* buffer, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    Buffer* buffer1 = Buffer::Cast(buffer);

    if (byteOffset < 0)
        byteOffset = 0;

    if (length < 0)
        length = buffer1->length() - byteOffset;

    if (length < 0 || length > buffer1->length() - byteOffset)
        length = 0;

    retVal = new Buffer(buffer1->data() + byteOffset, length);

    return result_t();
}

// the backing-store form of Buffer.from: a negative offset clamps to 0 and an
// out-of-range length becomes an empty buffer
static result_t buffer_from_store(std::shared_ptr<v8::BackingStore> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    if (byteOffset < 0)
        byteOffset = 0;

    if (length < 0)
        length = datas->ByteLength() - byteOffset;

    if (length < 0 || length > datas->ByteLength() - byteOffset)
        length = 0;

    retVal = new Buffer((uint8_t*)datas->Data() + byteOffset, length);

    return result_t();
}

result_t Buffer_base::from(Union_from_datas datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<v8::Local<v8::Uint8Array>>(datas)) {
        v8::Local<v8::Uint8Array> view = std::get<v8::Local<v8::Uint8Array>>(datas);

        if (byteOffset < 0)
            return result_t::invalid_arg();

        if (length < 0)
            length = view->ByteLength() - byteOffset;
        else if (length > view->ByteLength() - byteOffset)
            return result_t::invalid_arg();

        return buffer_from_store(view->Buffer()->GetBackingStore(), byteOffset + view->ByteOffset(), length, retVal);
    }

    return buffer_from_store(std::get<std::shared_ptr<v8::BackingStore>>(datas), byteOffset, length, retVal);
}

result_t Buffer_base::from(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr.failed())
        return hr;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());
    return result_t();
}

result_t Buffer_base::concat(v8::Local<v8::Array> buflist, int32_t cutLength, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current(buflist);
    v8::Local<v8::Context> context = isolate->context();

    std::vector<v8::Local<v8::Uint8Array>> bufs;
    int32_t length = buflist->Length();
    size_t sz = 0;

    bufs.resize(length);

    for (int32_t i = 0; i < length; i++) {
        v8::Local<v8::Value> val = buflist->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        if (val.IsEmpty() || !val->IsUint8Array())
            return result_t::invalid_arg();
        bufs[i] = val.As<v8::Uint8Array>();
        sz += bufs[i]->ByteLength();
    }

    if (cutLength >= 0 && sz > cutLength)
        sz = cutLength;

    obj_ptr<Buffer> buf = new Buffer(NULL, sz);
    size_t pos = 0;

    for (int32_t i = 0; i < length; i++) {
        int32_t len = bufs[i]->ByteLength();
        if (pos + len > sz)
            len = sz - pos;
        if (len <= 0)
            break;
        bufs[i]->CopyContents(buf->data() + pos, len);
        pos += len;
    }

    retVal = buf;

    return result_t();
}

v8::Local<v8::Value> Buffer::load_module()
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _global = context->Global();
    v8::Local<v8::Value> _buffer;

    obj_ptr<SandBox> sbox = new SandBox(false);

    sbox->InstallModule("encoding", encoding_base::class_info().getModule(isolate));

    TryCatch try_catch;
    result_t hr = sbox->require("internal/buffer", "/builtin", _buffer);
    if (hr.failed() || try_catch.HasCaught() || _buffer.IsEmpty()) {
        // require 失败（抛异常，或被 worker 终止中断打断）时 _buffer 是**空句柄**。
        // 直接把它交给 Object::Set 会走到 V8 内部解引用空 handle 而崩溃
        // （实测栈：Object::Set → PropertySetterCallback → Handle<Object>::operator*）。
        if (try_catch.HasCaught())
            ReportException(try_catch, result_t(), false);
        return v8::Undefined(isolate->m_isolate);
    }

    // node 的 require('buffer') 是模块对象（{ Buffer, SlowBuffer, constants, kMaxLength, … }），
    // 全局 Buffer 仍然是类本身。这里兼容两种形态：模块对象（取 .Buffer）或类本身。
    v8::Local<v8::Value> _buffer_class = _buffer;
    if (!_buffer->IsFunction()) {
        if (!_buffer->IsObject())
            return v8::Undefined(isolate->m_isolate);

        if (!_buffer.As<v8::Object>()->Get(context, isolate->NewString("Buffer")).ToLocal(&_buffer_class)
            || !_buffer_class->IsFunction())
            return v8::Undefined(isolate->m_isolate);
    }

    _global->Set(context, isolate->NewString("Buffer"), _buffer_class).IsJust();

    // 取 prototype 用的探针实例：显式传 0（不能依赖 `new Buffer()`——node 语义下无参构造是
    // TypeError，而这里必须成功，否则下面的 native 覆写（indexOf/write/…）装不上）
    v8::Local<v8::Function> js_buffer_class = _buffer_class.As<v8::Function>();
    v8::Local<v8::Value> ctor_args[1] = { v8::Integer::New(isolate->m_isolate, 0) };
    v8::Local<v8::Value> js_buffer_v;
    if (!js_buffer_class->CallAsConstructor(context, 1, ctor_args).ToLocal(&js_buffer_v)
        || js_buffer_v.IsEmpty() || !js_buffer_v->IsObject())
        return v8::Undefined(isolate->m_isolate);

    v8::Local<v8::Object> js_buffer = js_buffer_v.As<v8::Object>();

    v8::Local<v8::Value> js_buffer_proto_v = js_buffer->GetPrototype();
    if (!js_buffer_proto_v->IsObject())
        return v8::Undefined(isolate->m_isolate);

    v8::Local<v8::Object> js_buffer_proto = js_buffer_proto_v.As<v8::Object>();

    context->SetEmbedderData(kBufferClassIndex, _buffer_class);
    context->SetEmbedderData(kBufferModule, _buffer);
    context->SetEmbedderData(kBufferPrototype, js_buffer_proto);

    // 检索/比较由 native 提供引擎，但**签名与 node 语义**由 JS 层（internal/buffer.js）包装：
    // 以 Buffer.native_xxx.call(buf, …) 的形式暴露，避免 prototype 覆写把 JS 层的参数校验跳过。
    js_buffer_class->Set(context, isolate->NewString("native_compare"), isolate->NewFunction("compare", s_static_compare)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("native_equals"), isolate->NewFunction("equals", proto_equals)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("native_indexOf"), isolate->NewFunction("indexOf", proto_indexOf)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("native_lastIndexOf"), isolate->NewFunction("lastIndexOf", proto_lastIndexOf)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("native_write"), isolate->NewFunction("write", proto_write)).IsJust();

    js_buffer_class->Set(context, isolate->NewString("native_fill"), isolate->NewFunction("fill", proto_fill)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("native_byteLength"), isolate->NewFunction("byteLength", class_byteLength)).IsJust();

    // bytes -> string 的零绕行出口（latin1 / ascii / ucs2 / utf16be）。
    // 不再经过 exlib::string(UTF-8) 中转，对标 Node 的 ExternOneByte/TwoByteString。
    js_buffer_class->Set(context, isolate->NewString("native_encodeString"), isolate->NewFunction("encodeString", proto_encodeString)).IsJust();

    // string -> bytes 的零绕行出口（hex / base64）。
    // 用 String::ValueView 直接拿 V8 的 one-byte 指针，解出的字节直接落在返回的
    // Buffer 里，全程一次分配、一次写入（见 class_decodeString 的注释）。
    js_buffer_class->Set(context, isolate->NewString("native_decodeString"), isolate->NewFunction("decodeString", class_decodeString)).IsJust();

    // JS 层的 write 直接复用 native 实现（性能敏感的 utf8/ascii 快路径仍在 C++）
    // —— 安装完后调一次模块的 __postInstall（把 native_* 改成不可枚举，避免污染 Object.keys(Buffer)）
    v8::Local<v8::Value> post_install;
    if (_buffer->IsObject()
        && _buffer.As<v8::Object>()->Get(context, isolate->NewString("__postInstall")).ToLocal(&post_install)
        && post_install->IsFunction())
        post_install.As<v8::Function>()->Call(context, _buffer.As<v8::Object>(), 0, NULL).FromMaybe(v8::Local<v8::Value>());

    // js_buffer_class->Set(context, isolate->NewString("concat"), isolate->NewFunction("concat", s_static_concat)).IsJust();

    return _buffer;
}

v8::Local<v8::Object> Buffer::wrap(Isolate* isolate, v8::Local<v8::Object> This)
{
    if (!hasJSHandle()) {
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::ArrayBuffer> buf = v8::ArrayBuffer::New(isolate->m_isolate, m_store.m_store);

        v8::Local<v8::Value> args[3] = {
            buf,
            v8::Integer::New(isolate->m_isolate, m_store.m_offset),
            v8::Integer::New(isolate->m_isolate, m_store.m_length)
        };
        v8::Local<v8::Function> js_buffer_class = context->GetEmbedderData(kBufferClassIndex).As<v8::Function>();
        v8::Local<v8::Value> js_buffer = js_buffer_class->CallAsConstructor(context, 3, args).FromMaybe(v8::Local<v8::Value>());

        if (js_buffer.IsEmpty())
            return v8::Local<v8::Object>();
        return js_buffer.As<v8::Object>();
    }

    return object_base::wrap(isolate);
}

Buffer* Buffer::getInstance(v8::Local<v8::Value> o)
{
    if (!IsJSBuffer(o))
        return NULL;

    return new Buffer(o.As<v8::Uint8Array>());
}

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Buffer_base>& vr, bool bStrict)
{
    if (v.IsEmpty() || v->IsNumber() || v->IsNumberObject())
        return result_t::type_mismatch();

    if (!IsJSBuffer(v)) {
        if (bStrict)
            return result_t::type_mismatch();

        if (v->IsArrayBuffer()) {
            vr = new Buffer(v.As<v8::ArrayBuffer>());
            return result_t();
        }

        if (v->IsSharedArrayBuffer()) {
            v8::Local<v8::SharedArrayBuffer> sab = v.As<v8::SharedArrayBuffer>();
            std::shared_ptr<v8::BackingStore> backing = sab->GetBackingStore();
            if (backing && backing->Data()) {
                vr = new Buffer(backing, 0, backing->ByteLength());
            } else {
                Buffer_base::allocUnsafe(0, vr);
            }
            return result_t();
        }

        if (v->IsDataView()) {
            v8::Local<v8::DataView> view = v.As<v8::DataView>();
            vr = new Buffer(view->Buffer()->GetBackingStore(), view->ByteOffset(), view->ByteLength());
            return result_t();
        }

        if (v->IsArray())
            return Buffer_base::from(v.As<v8::Array>(), vr);

        obj_ptr<Blob_base> blob = Blob_base::getInstance(v);
        if (blob) {
            Blob* b = blob.As<Blob>();
            vr = b->m_impl.getBuffer();
            return result_t();
        }

        if (!v->IsTypedArray())
            return result_t::type_mismatch();
    }

    vr = new Buffer(v.As<v8::TypedArray>());

    return result_t();
}

// a binary option (key material, passphrase, header, ...) accepts a string as
// its utf8 bytes: the string form is the nominal way to write it, while the
// lenient Buffer argument conversion no longer takes strings
result_t GetConfigValue(v8::Local<v8::Object> o, const char* key, obj_ptr<Buffer_base>& n, bool bStrict)
{
    if (o.IsEmpty())
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);

    Isolate* isolate = Isolate::current(o);
    JSValue v = o->Get(isolate->context(), isolate->NewString(key));
    if (v->IsUndefined() || v->IsNull())
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);

    if (v->IsString() || v->IsStringObject()) {
        exlib::string s;

        result_t hr = GetArgumentValue(isolate, v, s);
        if (hr.failed())
            return hr;

        return Buffer_base::from(s, "utf8", n);
    }

    return GetArgumentValue(isolate, v, n, bStrict);
}

#define BUFFER_INSTANCE()                                     \
    obj_ptr<Buffer> pInst = Buffer::getInstance(args.This()); \
    if (pInst == NULL) {                                      \
        ThrowResult(result_t(CALL_E_NOTINSTANCE));            \
        return;                                               \
    }

void Buffer::proto_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->compare(v0, vr);

    METHOD_RETURN();
}

void Buffer::proto_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);

    hr = pInst->copy(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

void Buffer::proto_equals(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->equals(v0, vr);

    METHOD_RETURN();
}

void Buffer::proto_fill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr = args.This();

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->fill(v0, v1, v2);

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->fill(v0, v1, v2);

    METHOD_OVER(4, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(exlib::string, 3, "utf8");

    hr = pInst->fill(v0, v1, v2, v3);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(exlib::string, 2);

    hr = pInst->fill(v0, v1, v2);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->fill(v0, v1);

    METHOD_RETURN();
}

void Buffer::proto_indexOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(Union_indexOf_v, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_RETURN();
}

void Buffer::proto_lastIndexOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(Union_lastIndexOf_v, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = pInst->lastIndexOf(v0, v1, vr);

    METHOD_RETURN();
}

void Buffer::proto_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!args.This()->IsUint8Array()) {
        ThrowResult(result_t(CALL_E_NOTINSTANCE));
        return;
    }

    store buf(args.This().As<v8::Uint8Array>());
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();

    int32_t arg_cnt = args.Length();
    while (arg_cnt > 0 && args[arg_cnt - 1]->IsUndefined())
        --arg_cnt;

    if (arg_cnt == 0) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    v8::Local<v8::String> data;
    if (args[0]->IsString())
        data = args[0].As<v8::String>();
    else if (args[0]->IsStringObject())
        data = args[0].As<v8::StringObject>()->ValueOf();
    else {
        ThrowResult(result_t::type_mismatch());
        return;
    }

    v8::Local<v8::String> codec;
    if (arg_cnt > 1) {
        if (args[arg_cnt - 1]->IsString())
            codec = args[--arg_cnt].As<v8::String>();
        else if (args[arg_cnt - 1]->IsStringObject())
            codec = args[--arg_cnt].As<v8::StringObject>()->ValueOf();
    }

    size_t offset = 0;
    if (arg_cnt > 1) {
        if (!args[1]->IsNumber() && !args[1]->IsNumberObject()) {
            ThrowResult(result_t::type_mismatch());
            return;
        }

        double offset_num = args[1]->NumberValue(context).FromMaybe(0);
        if (std::floor(offset_num) != offset_num || offset_num < 0 || offset_num > (double)buf.length()) {
            ThrowResult(result_t::out_of_range());
            return;
        }

        offset = (size_t)offset_num;
    }

    // node: length 必须是 [0, buffer.length - offset] 内的整数，负数/越界都抛 RangeError
    size_t max_length = buf.length() - offset;
    if (arg_cnt > 2) {
        if (!args[2]->IsNumber() && !args[2]->IsNumberObject()) {
            ThrowResult(result_t::type_mismatch());
            return;
        }

        double length_num = args[2]->NumberValue(context).FromMaybe(0);
        if (std::floor(length_num) != length_num || length_num < 0 || length_num > (double)max_length) {
            ThrowResult(result_t::out_of_range());
            return;
        }

        max_length = (size_t)length_num;
    }

    int flags = v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION | v8::String::REPLACE_INVALID_UTF8;

    // 返回值必须是**实际写入的字节数**（旧实现返回 max_length = 容量）
    if (codec.IsEmpty()) {
        int written = data->WriteUtf8(isolate->m_isolate, (char*)buf.data() + offset, (int)max_length, nullptr, flags);
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, written));
        return;
    }

    v8::String::Utf8Value codec_utf8(isolate->m_isolate, codec);

    if (!strcmp(*codec_utf8, "ascii") || !strcmp(*codec_utf8, "buffer")) {
        int written;
        if (data->IsExternalOneByte()) {
            auto ext = data->GetExternalOneByteStringResource();
            written = (int)std::min(max_length, (size_t)ext->length());
            memcpy(buf.data() + offset, ext->data(), written);
        } else
            written = data->WriteOneByte(isolate->m_isolate, buf.data() + offset, 0, (int)max_length, flags);

        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, written));
        return;
    } else if (!strcmp(*codec_utf8, "utf8") || !strcmp(*codec_utf8, "utf-8")) {
        // 全 ASCII 的外部 one-byte 字符串：UTF-8 编码与 WriteOneByte 逐字节相同，
        // 直接 memcpy 比 WriteUtf8 快 8×（64 KB 19 µs -> 2.3 µs）。
        // 这条路径很常见：Buffer.toString('latin1') / ('utf8') 的结果都是外部串，
        // 再喂回 buf.write() 就命中这里。
        int written;
        if (data->IsExternalOneByte()) {
            auto ext = data->GetExternalOneByteStringResource();
            const char* src = ext->data();
            size_t n = std::min(max_length, (size_t)ext->length());
            // 外部 one-byte 串可能是任意字节，只有全 ASCII 才能当 UTF-8 直接拷
            if (simdutf::validate_ascii(src, n)) {
                memcpy(buf.data() + offset, src, n);
                args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)n));
                return;
            }
        }

        // one-byte 但含高位字节（典型来源：Buffer.toString('latin1')）：
        // 每个字节要展开成 1 或 2 个字节。WriteUtf8 在这里也是标量，
        // simdutf 的 convert_latin1_to_utf8_safe 是向量化的（64 KB 29 µs -> 5 µs）。
        // _safe 版的截断同样是"只写完整字符"，与 node 一致。
        if (data->IsOneByte()) {
            v8::String::ValueView view(isolate->m_isolate, data);
            if (view.is_one_byte()) {
                size_t n = simdutf::convert_latin1_to_utf8_safe(
                    (const char*)view.data8(), view.length(),
                    (char*)buf.data() + offset, max_length);
                args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)n));
                return;
            }
        }

        // two-byte 串走 simdutf 的向量化 UTF-16 -> UTF-8。
        // V8 的 WriteUtf8 在 arm64 上是标量实现：64 KB CJK 要 85 µs，
        // simdutf 的 convert_utf16_to_utf8_safe 只要 20 µs（4×）。
        //
        // `_safe` 版本的截断语义与 node 完全一致：**只写完整字符**，
        // 放不下的那个字符一个字节也不写（实测 '中文测试' 在 cap=4/5 时
        // 返回 3，与 node 的 buf.write 逐值相同）。
        //
        // 它同时在内部做校验：遇到孤代理项返回 0，此时退回 WriteUtf8 ——
        // node 的 WriteUtf8 带 REPLACE_INVALID_UTF8，会把孤代理项写成 U+FFFD，
        // 这与「不写」不同，不能直接用 simdutf 的结果。
        if (!data->IsOneByte()) {
            v8::String::ValueView view(isolate->m_isolate, data);
            if (!view.is_one_byte()) {
                size_t n = simdutf::convert_utf16_to_utf8_safe(
                    (const char16_t*)view.data16(), view.length(),
                    (char*)buf.data() + offset, max_length);
                if (n > 0 || max_length == 0 || view.length() == 0) {
                    args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)n));
                    return;
                }
            }
        }

        written = data->WriteUtf8(isolate->m_isolate, (char*)buf.data() + offset, (int)max_length, nullptr, flags);
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, written));
        return;
    } else if (!strcmp(*codec_utf8, "ucs2") || !strcmp(*codec_utf8, "ucs-2")
        || !strcmp(*codec_utf8, "utf16") || !strcmp(*codec_utf8, "utf-16")
        || !strcmp(*codec_utf8, "utf16le") || !strcmp(*codec_utf8, "utf-16le")) {
        // UTF-16LE 直连 V8：小端平台上就是一次 UTF-16 拷贝。
        // node 的 length 以**字节**计，向下取整到 code unit —— 不写半个 code unit。
        // 旧实现退回 commonDecode 的 UTF-8 中转（64 KB 要 339 µs）。
        size_t units = max_length / sizeof(char16_t);
        uint8_t* dst = buf.data() + offset;
        int written;

        if (is_big_endian()) {
            exlib::wstring w;
            w.resize(units);
            written = data->Write(isolate->m_isolate, (uint16_t*)w.data(), 0, (int)units, flags);
            if (written < 0)
                written = 0;
            for (int i = 0; i < written; i++) {
                uint16_t c = (uint16_t)w[i];
                dst[i * 2] = (uint8_t)(c & 0xff);
                dst[i * 2 + 1] = (uint8_t)(c >> 8);
            }
        } else if (((uintptr_t)dst & (sizeof(char16_t) - 1)) == 0) {
            written = data->Write(isolate->m_isolate, (uint16_t*)dst, 0, (int)units, flags);
            if (written < 0)
                written = 0;
        } else {
            // 未按 2 字节对齐（offset 是奇数）：先落到对齐的临时区再拷
            exlib::wstring w;
            w.resize(units);
            written = data->Write(isolate->m_isolate, (uint16_t*)w.data(), 0, (int)units, flags);
            if (written < 0)
                written = 0;
            if (written)
                memcpy(dst, w.data(), (size_t)written * sizeof(char16_t));
        }
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)(written * sizeof(char16_t))));
        return;
    } else if (!strcmp(*codec_utf8, "utf16be") || !strcmp(*codec_utf8, "utf-16be")
        || !strcmp(*codec_utf8, "ucs2be") || !strcmp(*codec_utf8, "ucs-2be")) {
        // 大端 UTF-16：先写进临时 wstring，再按大端落盘
        size_t units = max_length / sizeof(char16_t);
        exlib::wstring w;
        w.resize(units);
        int written = data->Write(isolate->m_isolate, (uint16_t*)w.data(), 0, (int)units, flags);
        if (written < 0)
            written = 0;
        uint8_t* dst = buf.data() + offset;
        for (int i = 0; i < written; i++) {
            uint16_t c = (uint16_t)w[i];
            dst[i * 2] = (uint8_t)(c >> 8);
            dst[i * 2 + 1] = (uint8_t)(c & 0xff);
        }
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)(written * sizeof(char16_t))));
        return;
    }

    // hex / base64 是**逐 API 语义**不同的编码：
    //   encoding.hex.decode(s)      —— 跳过非法字符继续（fibjs 扩展的宽容模式）
    //   Buffer.from(s, 'hex')       —— 遇第一个非法字符即停（node）
    //   buf.write(s, off, len,'hex')—— 同 node，遇非法即停
    //
    // 这条兜底路径以前用 commonDecode（= 宽容跳过），于是 'abZZcd' 写出 3 字节
    // 并返回 3，而 node 只写 1 字节（'ab'）并返回 1。
    //
    // 另外**不能用 GetArgumentValue**：它把字符串转成 UTF-8，而 node 的 nbytes
    // 对 char16_t 输入做的是 `static_cast<uint8_t>(src[i])`，即每个 code unit
    // **取低 8 位**。两者只在纯 ASCII 时相同：
    //   Buffer.from('\u0141\u0142','hex')  node='ab'  UTF-8 路径=''
    //   Buffer.from('\u0161','hex')        node='aa'  UTF-8 路径=''
    if (!strcmp(*codec_utf8, "hex") || !strcmp(*codec_utf8, "base64")
        || !strcmp(*codec_utf8, "base64url")) {
        v8::String::Value value(isolate->m_isolate, data);
        size_t sz = value.length();
        exlib::string narrow;
        narrow.resize(sz);
        const uint16_t* src = (const uint16_t*)*value;
        char* p = narrow.data();
        for (size_t i = 0; i < sz; i++)
            p[i] = (char)(src[i] & 0xff);

        // **length 是硬上限**：node 的 write('QUJDRA==', 0, 2, 'base64') 只写
        // 2 字节并返回 2，这里必须把解码容量钉在 max_length 上（不能只用
        // max_length 之外的公式值）。
        size_t cap = std::min(max_length,
            strcmp(*codec_utf8, "hex") == 0 ? sz / 2 : ((sz * 3) >> 2));
        size_t n = strcmp(*codec_utf8, "hex") == 0
            ? hexDecodeTo(p, sz, (char*)buf.data() + offset, cap)
            : base64DecodeTo(p, sz, (char*)buf.data() + offset, cap);
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)n));
        return;
    }

    exlib::string data_str;
    GetArgumentValue(isolate, data, data_str);

    exlib::string strBuf;
    result_t hr = commonDecode(*codec_utf8, data_str, strBuf);
    if (hr.failed()) {
        ThrowResult(hr);
        return;
    }

    max_length = std::min(max_length, strBuf.length());
    memcpy(buf.data() + offset, strBuf.c_str(), max_length);
    args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int)max_length));
}

// bytes -> V8 string 的零绕行出口。
//
// Buffer.toString('latin1') / ('ucs2') 以前要经过 exlib::string(UTF-8) 中转：
// latin1 的 0x80..0xFF 被展开成两字节 UTF-8，再由 NewString 的 is_safe_string
// 判断后拆回 one-byte；ucs2 则 UTF-16 -> UTF-8 -> UTF-16 走三趟。
// 这里直接挂外部字符串（Node 也是这么做：string_bytes.cc 的 ExternOneByteString /
// ExternTwoByteString），64 KB 从 146/70 µs 降到一次 memcpy。
//
// 返回 undefined 表示"这个编码不走零绕行"，调用方（JS 层）退回普通路径。
void Buffer::proto_encodeString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!args.This()->IsUint8Array()) {
        ThrowResult(result_t(CALL_E_NOTINSTANCE));
        return;
    }

    store buf(args.This().As<v8::Uint8Array>());
    Isolate* isolate = Isolate::current(args);
    v8::Isolate* v8_isolate = isolate->m_isolate;

    // 参数：codec [, start [, end]]
    int32_t arg_cnt = args.Length();
    if (arg_cnt == 0 || !args[0]->IsString()) {
        ThrowResult(result_t::type_mismatch());
        return;
    }

    v8::String::Utf8Value codec_utf8(v8_isolate, args[0].As<v8::String>());
    if (*codec_utf8 == nullptr) {
        ThrowResult(result_t(CALL_E_JAVASCRIPT));
        return;
    }

    size_t start = 0;
    size_t end = buf.length();

    if (arg_cnt > 1 && !args[1]->IsUndefined()) {
        double n = args[1]->NumberValue(isolate->context()).FromMaybe(0);
        if (!(n >= 0))
            n = 0;
        start = (size_t)n;
        if (start > end)
            start = end;
    }
    if (arg_cnt > 2 && !args[2]->IsUndefined()) {
        double n = args[2]->NumberValue(isolate->context()).FromMaybe(0);
        if (!(n >= 0))
            n = 0;
        end = (size_t)n;
        if (end > buf.length())
            end = buf.length();
        if (end < start)
            end = start;
    }

    size_t len = end - start;
    const uint8_t* p = buf.data() + start;

    // ---- latin1 / binary：每个字节就是 U+0000..U+00FF ----
    //
    // 注意 **ascii 不在这里**：node 的 ascii 解码会掩掉高位（0xE9 -> 0x69），
    // 与 latin1 的逐字节映射不同，所以 ascii 退回通用路径（commonEncode 里有掩码实现）。
    if (!strcmp(*codec_utf8, "latin1") || !strcmp(*codec_utf8, "binary")) {
        if (len == 0) {
            args.GetReturnValue().Set(v8::String::Empty(v8_isolate));
            return;
        }

        // 字节即 U+0000..U+00FF：NewStringOneByte 直接挂 one-byte 外部字符串，
        // 不经过 UTF-8（旧路径要先把 0x80-0xFF 展开成两字节 UTF-8 再拆回来）。
        v8::Local<v8::String> v = NewStringOneByte(v8_isolate, exlib::string((const char*)p, len));
        if (v.IsEmpty()) {
            args.GetReturnValue().Set(v8::Undefined(v8_isolate));
            return;
        }
        args.GetReturnValue().Set(v);
        return;
    }

    // ---- utf8 / ascii：simdutf 解码后直接挂 two-byte 外部串 ----
    //
    // 旧路径是 bytes -> exlib::string(UTF-8 中转) -> utf8to16String -> NewString，
    // 一共扫三遍、分配两次。这里用 simdutf 一次算长度一次转换，直接
    // NewStringTwoByte 挂外部缓冲（两次扫描、一次分配）。
    // 64 KB CJK 从 106 µs 降到约 40 µs。
    //
    // **ascii 不能走这里**：node 的 ascii 解码把每个字节掩成 7 位（0xE9 -> 'i'），
    // 与 utf8 的「非法字节 -> U+FFFD」完全不同，仍退回通用路径。
    if (!strcmp(*codec_utf8, "utf8") || !strcmp(*codec_utf8, "utf-8")) {
        if (len == 0) {
            args.GetReturnValue().Set(v8::String::Empty(v8_isolate));
            return;
        }

        // 先尝试走"全合法"的快路径；有非法序列时退回通用路径。
        //
        // **全 ASCII 必须落成 one-byte 串**：硬转 two-byte 会让内存翻倍，
        // 实测 64 KB ASCII 反而从 9 µs 涨到 18 µs。
        if (simdutf::validate_ascii((const char*)p, len)) {
            v8::Local<v8::String> v = NewStringOneByte(v8_isolate,
                exlib::string((const char*)p, len));
            if (!v.IsEmpty()) {
                args.GetReturnValue().Set(v);
                return;
            }
        } else if (simdutf::validate_utf8((const char*)p, len)) {
            size_t units = simdutf::utf16_length_from_utf8((const char*)p, len);
            exlib::wstring w;
            w.resize(units);
            size_t n = simdutf::convert_utf8_to_utf16((const char*)p, len, w.data());
            if (n == units) {
                v8::Local<v8::String> v = NewStringTwoByte(v8_isolate, std::move(w));
                if (!v.IsEmpty()) {
                    args.GetReturnValue().Set(v);
                    return;
                }
            }
        }

        args.GetReturnValue().Set(v8::Undefined(v8_isolate));
        return;
    }

    // ---- ucs2 / utf16le：小端平台上 Buffer 字节就是 UTF-16LE 内存布局 ----
    if (!strcmp(*codec_utf8, "ucs2") || !strcmp(*codec_utf8, "ucs-2")
        || !strcmp(*codec_utf8, "utf16") || !strcmp(*codec_utf8, "utf-16")
        || !strcmp(*codec_utf8, "utf16le") || !strcmp(*codec_utf8, "utf-16le")
        || !strcmp(*codec_utf8, "utf16be") || !strcmp(*codec_utf8, "utf-16be")
        || !strcmp(*codec_utf8, "ucs2be") || !strcmp(*codec_utf8, "ucs-2be")) {
        bool be = !strcmp(*codec_utf8, "utf16be") || !strcmp(*codec_utf8, "utf-16be")
            || !strcmp(*codec_utf8, "ucs2be") || !strcmp(*codec_utf8, "ucs-2be");

        size_t units = len / sizeof(char16_t);      // 奇数字节丢弃（node 语义）
        if (units == 0) {
            args.GetReturnValue().Set(v8::String::Empty(v8_isolate));
            return;
        }

        // LE 平台上 Buffer 字节就是 UTF-16LE 内存布局：一次 wstring 拷贝即可。
        // BE 时逐 code unit 交换字节序。
        exlib::wstring w;
        w.resize(units);
        if (!be) {
            memcpy(w.data(), p, units * sizeof(char16_t));
        } else {
            for (size_t i = 0; i < units; i++)
                w[i] = (char16_t)(((uint16_t)p[i * 2] << 8) | p[i * 2 + 1]);
        }

        v8::Local<v8::String> v = NewStringTwoByte(v8_isolate, std::move(w));
        if (v.IsEmpty()) {
            args.GetReturnValue().Set(v8::Undefined(v8_isolate));
            return;
        }
        args.GetReturnValue().Set(v);
        return;
    }

    // 其它编码不走零绕行，让 JS 层退回通用路径
    args.GetReturnValue().Set(v8::Undefined(v8_isolate));
}

// string -> bytes 的零绕行解码（hex / base64）。
//
// 旧路径（JS 侧逐字符 hexDecode / encoding.decode）有三次多余开销：
//   1) JS 逐字符 charCodeAt 扫 128K 字符（hex 约 190 µs，V8 也没能向量化）
//   2) 结果先落进 JS 的临时 Uint8Array，构造 Buffer 时再拷一遍
//   3) base64 走 encoding.decode 时，native 侧还要先做一遍标量有效性预扫
//      （87K 约 59 µs，占了整条路径的 93%）
//
// 这里的三条改进：
//   1) String::ValueView 零拷贝拿到 V8 的内部数据指针（one-byte 时就是字节流，
//      不需要 ToString -> exlib::string 的分配与拷贝）
//   2) base64 直接用 simdutf 的 accept_garbage 选项 —— 它已经实现了我们要的全部
//      四条宽容语义（跳过非法字符、'=' 终止、双字母表、尾部不满 4 字符），
//      40 万随机用例对拍 0 失配，所以预扫可以整段删掉
//   3) 解码直接写进返回 Buffer 的 backing store，再让新 Buffer 的 wrap 用同一个
//      store 建 ArrayBuffer —— 全程一次分配、一次写入，没有二次拷贝
//
// 返回 undefined 表示"这个编码不走这里"，JS 侧退回通用路径。
void Buffer::class_decodeString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Isolate* v8_isolate = isolate->m_isolate;

    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
        args.GetReturnValue().Set(v8::Undefined(v8_isolate));
        return;
    }

    v8::String::Utf8Value codec_utf8(v8_isolate, args[1].As<v8::String>());
    if (*codec_utf8 == nullptr) {
        args.GetReturnValue().Set(v8::Undefined(v8_isolate));
        return;
    }

    bool is_hex = !strcmp(*codec_utf8, "hex");
    bool is_base64 = !strcmp(*codec_utf8, "base64") || !strcmp(*codec_utf8, "base64url");

    if (!is_hex && !is_base64) {
        args.GetReturnValue().Set(v8::Undefined(v8_isolate));
        return;
    }

    v8::Local<v8::String> str = args[0].As<v8::String>();

    // **顺序很关键**：ValueView 内部是 DisallowGarbageCollection 作用域，
    // 期间任何堆分配（包括 new Buffer）都会触发 "Check failed:
    // AllowHeapAllocationInRelease::IsAllowed()"。所以：
    //   1) 先用 str->Length() 算容量并分配好 Buffer 与可能的窄化缓冲
    //   2) 再开 ValueView 做纯解码（只写已分配的内存，不再分配）
    //   3) view 析构之后再 wrap
    const size_t sz = (size_t)str->Length();
    const size_t cap = is_hex ? (sz / 2) : ((sz * 3) >> 2);

    Buffer* buf = new Buffer(NULL, cap);

    // 窄化缓冲必须在 view 外分配（见上）。hex/base64 的输入绝大多数是 ASCII，
    // 而 one-byte 串根本用不到它，所以先用 IsOneByte() 判断 —— 它读不到内容、
    // 只会给出 "确定是 one-byte" 的证据（可能误报 false），正好够用：
    // 误报时只是白分配一次，不会漏分配。
    exlib::string narrow;
    if (!str->IsOneByte())
        narrow.resize(sz);

    size_t n;
    {
        v8::String::ValueView view(v8_isolate, str);

        if (view.is_one_byte()) {
            // one-byte 串：V8 的内部字节流就是我们要的输入，零拷贝
            n = is_hex
                ? hexDecodeTo((const char*)view.data8(), sz, (char*)buf->data(), cap)
                : base64DecodeTo((const char*)view.data8(), sz, (char*)buf->data(), cap);
        } else {
            // two-byte 串：hex/base64 只认 ASCII，**每个 code unit 取低 8 位**
            // （不是替换成 0xFF）。node 的 nbytes::HexDecode 对 char16_t 输入做的
            // 就是 `static_cast<uint8_t>(src[i])`，同样只截低 8 位：
            //   Buffer.from('\u0141\u0142', 'hex')    -> 'ab'   （0x41 0x42）
            //   Buffer.from('\u0141\u0142', 'base64') -> '00'   （'AB'）
            // 截断后如果落在非法字符集里，自然就停在第一个非法字符处，与 node 一致
            // （'a\u0100bc' -> 0x00 非法 -> ""）。
            char* dst = narrow.data();
            const char16_t* src = (const char16_t*)view.data16();
            for (size_t i = 0; i < sz; i++)
                dst[i] = (char)(src[i] & 0xff);

            n = is_hex
                ? hexDecodeTo(dst, sz, (char*)buf->data(), cap)
                : base64DecodeTo(dst, sz, (char*)buf->data(), cap);
        }
    }

    if (n < cap)
        buf->resize(n);
    args.GetReturnValue().Set(buf->wrap(isolate));
}

// UTF-8 字节长度：V8 的 String::Utf8Length 的替代实现。
//
// 为什么不用 Utf8Length：它要走 isolate 的虚调用 + 可能的字符串扁平化，
// 64 KB ASCII 实测 7.5 µs；node 走的是另一条路（V8 Fast API 的
// FastOneByteString，直接读字符串内部字节，0.73 µs）。
//
// 这里用 String::ValueView 零拷贝拿内部指针，再按 one-byte / two-byte 分别算：
//
//   one-byte  —— 每个字节就是一个 U+0000..U+00FF 码点，
//                simdutf::utf8_length_from_latin1 一次算完（AVX2/AVX-512）
//
//   two-byte  —— 分两步：
//                1) SIMD 求整段的 max 和 "有无 ASCII"（三档分流，绝大多数
//                   真实文本是同质的：全 ASCII / 全 CJK / 全拉丁扩展）
//                2) 混合段落用 SIMD 计数：按 0x80 与 0x800 两个阈值分别
//                   统计"1 字节的个数"和"3 字节的个数"，剩下的就是 2 字节。
//                   含代理项时才退回逐字符（合法对 4 字节，孤代理项 3 字节）。
//
// **累加器必须定期归约**：计数用的是 __m128i 的 16 位 lane（8 个），
// 每个 lane 每处理 8 个 code unit 就 +1，跑 65536 个 code unit 后 lane 里
// 会存到 8192；再长就会在 32768 处回绕成负数（实测 262144 个 code unit 的
// 'a中' 混合串直接算出 0 甚至负值，进而让 Buffer.from 只分配一半内存、
// 静默截断数据）。所以每处理 SIMD_REDUCE_EVERY 个分组就把 16 位累加器
// 归约进 64 位标量，把上限钉在安全区里。
//
// 正确性：全 65536 个 code unit、孤代理项/astral、NUL、混合、**长串**（跨过
// 归约周期）与 node 的 Buffer.byteLength 逐值对拍，0 失配。

#if defined(__SSE2__)
// 8 个 16 位 lane 水平求和。用于把周期性归约的中间结果并进 64 位标量。
static inline int64_t hsum_epi16(__m128i a)
{
    __m128i s = _mm_madd_epi16(a, _mm_set1_epi16(1));
    s = _mm_add_epi32(s, _mm_srli_si128(s, 8));
    s = _mm_add_epi32(s, _mm_srli_si128(s, 4));
    return (int32_t)_mm_cvtsi128_si32(s);
}
#endif

#if defined(__aarch64__)
#define V16(p) ((const uint16_t*)(p))

// 8 个 16 位 lane 水平求和（vaddlvq_u16 一条指令）。
static inline int64_t hsum_epi16(uint16x8_t a)
{
    return vaddlvq_u16(a);
}

// NEON 分支的 4 路展开：同质文本靠它把「两遍 scan 的纯加载」跑满内存带宽；
// 计数循环也是 4 路，避免 vsubq 的依赖链串行化（实测 naive 单路只有 24 GB/s，
// 4 路能到 87 GB/s）。
#define NEON_UNROLL 4
// 归约周期：4 路展开下每个 lane 每周期最多 +NEON_REDUCE_UNITS/8，
// NEON_REDUCE_UNITS=16384 时最多 +2048，离 16 位回绕点 65535 很远。
#define NEON_REDUCE_UNITS 16384

// latin1(one-byte) -> UTF-8 的字节长度：len = n + count(byte >= 0x80)。
//
// 不用 simdutf::utf8_length_from_latin1：它的 arm64 后端没向量化这条路径，
// 实测与 fallback 标量同速（64 KB 约 3.0 µs），比这里的 0.81 µs 慢 3.7×。
// validate_ascii 虽然是向量化的（0.66 µs），但它只能判定「全 ASCII」，
// 一旦有高位字节就得再扫一遍，反而更慢（见下面的 mixed 用例）。
//
// 用 vpadalq_u8 把 0/1 直接累加进 16 位 lane：每批最多 +NEON_LATIN1_REDUCE/16，
// NEON_LATIN1_REDUCE=4096 时上限 256，远低于 u16 回绕点。
static int64_t latin1ByteLength(const char* p, size_t n)
{
    size_t i = 0;
    int64_t cnt = 0;
    const uint8x16_t t80 = vdupq_n_u8(0x80);
    const uint8x16_t one = vdupq_n_u8(1);

    while (i + 64 <= n) {
        size_t end = i + 4096;
        if (end > n)
            end = n;

        uint16x8_t a0 = vdupq_n_u16(0), a1 = a0, a2 = a0, a3 = a0;
        for (; i + 64 <= end; i += 64) {
            uint8x16_t v0 = vld1q_u8((const uint8_t*)(p + i));
            uint8x16_t v1 = vld1q_u8((const uint8_t*)(p + i + 16));
            uint8x16_t v2 = vld1q_u8((const uint8_t*)(p + i + 32));
            uint8x16_t v3 = vld1q_u8((const uint8_t*)(p + i + 48));
            a0 = vpadalq_u8(a0, vandq_u8(vcgeq_u8(v0, t80), one));
            a1 = vpadalq_u8(a1, vandq_u8(vcgeq_u8(v1, t80), one));
            a2 = vpadalq_u8(a2, vandq_u8(vcgeq_u8(v2, t80), one));
            a3 = vpadalq_u8(a3, vandq_u8(vcgeq_u8(v3, t80), one));
        }
        cnt += hsum_epi16(vaddq_u16(vaddq_u16(a0, a1), vaddq_u16(a2, a3)));
    }
    {
        uint16x8_t a0 = vdupq_n_u16(0);
        for (; i + 16 <= n; i += 16) {
            uint8x16_t v = vld1q_u8((const uint8_t*)(p + i));
            a0 = vpadalq_u8(a0, vandq_u8(vcgeq_u8(v, t80), one));
        }
        cnt += hsum_epi16(a0);
    }
    for (; i < n; i++)
        if ((uint8_t)p[i] >= 0x80)
            cnt++;

    return (int64_t)n + cnt;
}

// 只数合法代理对（供 minv >= 0x800 的特例使用）。
//
// 该特例下所有 code unit 都 >= 0x800，于是 c80 与 c800 恒等于 n、
// base = n + n + n = 3n 是已知量，唯一需要扫描的就是代理对个数。
// 省掉两套比较与累加器后，纯 emoji 文本 64K 从 8.4 µs 降到 4.1 µs（2×）。
static int64_t neon_count_pairs(const char16_t* p, uint32_t n)
{
    const uint16x8_t mFC00 = vdupq_n_u16(0xFC00);
    const uint16x8_t vD800 = vdupq_n_u16(0xD800);
    const uint16x8_t vDC00 = vdupq_n_u16(0xDC00);

    int64_t pairs = 0;
    uint32_t k = 0;
    uint16x8_t prevHi = vdupq_n_u16(0);

    while (k + 8 * NEON_UNROLL <= n) {
        uint32_t end = k + NEON_REDUCE_UNITS;
        if (end > n)
            end = n;

        uint16x8_t d0 = vdupq_n_u16(0), d1 = d0, d2 = d0, d3 = d0;
        for (; k + 8 * NEON_UNROLL <= end; k += 8 * NEON_UNROLL) {
            uint16x8_t v0 = vld1q_u16(V16(p + k));
            uint16x8_t v1 = vld1q_u16(V16(p + k + 8));
            uint16x8_t v2 = vld1q_u16(V16(p + k + 16));
            uint16x8_t v3 = vld1q_u16(V16(p + k + 24));

            uint16x8_t k0 = vandq_u16(v0, mFC00);
            uint16x8_t h0 = vceqq_u16(k0, vD800);
            d0 = vsubq_u16(d0, vandq_u16(vextq_u16(prevHi, h0, 7), vceqq_u16(k0, vDC00)));
            prevHi = h0;

            uint16x8_t k1 = vandq_u16(v1, mFC00);
            uint16x8_t h1 = vceqq_u16(k1, vD800);
            d1 = vsubq_u16(d1, vandq_u16(vextq_u16(prevHi, h1, 7), vceqq_u16(k1, vDC00)));
            prevHi = h1;

            uint16x8_t k2 = vandq_u16(v2, mFC00);
            uint16x8_t h2 = vceqq_u16(k2, vD800);
            d2 = vsubq_u16(d2, vandq_u16(vextq_u16(prevHi, h2, 7), vceqq_u16(k2, vDC00)));
            prevHi = h2;

            uint16x8_t k3 = vandq_u16(v3, mFC00);
            uint16x8_t h3 = vceqq_u16(k3, vD800);
            d3 = vsubq_u16(d3, vandq_u16(vextq_u16(prevHi, h3, 7), vceqq_u16(k3, vDC00)));
            prevHi = h3;
        }
        // 四个累加器可以先 vaddq_u16 再归约：每个 lane 最多累到
        // NEON_REDUCE_UNITS/32 = 512，四个相加远低于 16 位上限。
        // （**不能**把 0xFFFF 掩码直接交给 hsum —— 那是 65535 不是 1；
        // 下面的尾段就是踩到这个坑的，必须用 vsubq 的借位形式。）
        pairs += hsum_epi16(vaddq_u16(vaddq_u16(d0, d1), vaddq_u16(d2, d3)));
    }
    for (; k + 8 <= n; k += 8) {
        uint16x8_t v = vld1q_u16(V16(p + k));
        uint16x8_t kk = vandq_u16(v, mFC00);
        uint16x8_t h = vceqq_u16(kk, vD800);
        // vceqq 出的是 0xFFFF，必须借位减成 1，否则 hsum 会算成 65535
        pairs += hsum_epi16(vsubq_u16(vdupq_n_u16(0),
            vandq_u16(vextq_u16(prevHi, h, 7), vceqq_u16(kk, vDC00))));
        prevHi = h;
    }

    uint32_t carry_hi = (uint32_t)vgetq_lane_u16(prevHi, 7) & 1;
    for (; k < n; k++) {
        uint32_t c = (uint32_t)p[k];
        if (carry_hi && (c & 0xFC00) == 0xDC00)
            pairs++;
        carry_hi = ((c & 0xFC00) == 0xD800);
    }

    return pairs;
}
#endif

static int64_t utf8ByteLength(v8::Isolate* isolate, v8::Local<v8::String> str)
{
    v8::String::ValueView view(isolate, str);
    uint32_t n = view.length();

    if (n == 0)
        return 0;

    if (view.is_one_byte()) {
        const char* p = (const char*)view.data8();
        // 每个字节按 U+0000..U+00FF 转 UTF-8：>=0x80 算 2 字节，其余算 1
#if defined(__aarch64__)
        return latin1ByteLength(p, n);
#else
        return (int64_t)simdutf::utf8_length_from_latin1(p, n);
#endif
    }

    const char16_t* p = (const char16_t*)view.data16();

    // ---- 第一档：全 ASCII / 全 2 字节 / 全 3 字节 ----
    // 只要整段同质就能 O(1) 出结果。注意**不能只看 max**：max < 0x800 时
    // 段内可能混着 ASCII（'¡hέlló wôrld!' 就是），必须同时确认没有 ASCII。
    uint32_t i = 0;
    uint32_t maxv = 0;
    uint32_t minv = 0xFFFF;
#if defined(__aarch64__)
    // AArch64 有原生无符号向量 max/min（vmaxq_u16 / vminq_u16 + 水平归约），
    // 不需要 x86 那种符号位翻转。4 路展开把加载流水线拉满：
    // 64 KB 全 ASCII 从 4.0 µs（标量，clang 自动向量化）降到 1.3 µs。
    {
        uint16x8_t mx0 = vdupq_n_u16(0), mn0 = vdupq_n_u16(0xFFFF);
        uint16x8_t mx1 = mx0, mn1 = mn0, mx2 = mx0, mn2 = mn0, mx3 = mx0, mn3 = mn0;
        for (; i + 8 * NEON_UNROLL <= n; i += 8 * NEON_UNROLL) {
            uint16x8_t v0 = vld1q_u16(V16(p + i));
            uint16x8_t v1 = vld1q_u16(V16(p + i + 8));
            uint16x8_t v2 = vld1q_u16(V16(p + i + 16));
            uint16x8_t v3 = vld1q_u16(V16(p + i + 24));
            mx0 = vmaxq_u16(mx0, v0);
            mn0 = vminq_u16(mn0, v0);
            mx1 = vmaxq_u16(mx1, v1);
            mn1 = vminq_u16(mn1, v1);
            mx2 = vmaxq_u16(mx2, v2);
            mn2 = vminq_u16(mn2, v2);
            mx3 = vmaxq_u16(mx3, v3);
            mn3 = vminq_u16(mn3, v3);
        }
        // 单个 8 lane 的尾段（n 常是 2 的幂，这一步通常不执行）
        for (; i + 8 <= n; i += 8) {
            uint16x8_t v = vld1q_u16(V16(p + i));
            mx0 = vmaxq_u16(mx0, v);
            mn0 = vminq_u16(mn0, v);
        }
        mx0 = vmaxq_u16(vmaxq_u16(mx0, mx1), vmaxq_u16(mx2, mx3));
        mn0 = vminq_u16(vminq_u16(mn0, mn1), vminq_u16(mn2, mn3));
        maxv = vmaxvq_u16(mx0);
        minv = vminvq_u16(mn0);
    }
#elif defined(__SSE2__)
    // SSE2 没有无符号 16 位 max/min（_mm_max_epu16 要 SSE4.1），
    // 用"翻转符号位转成有符号比较"：v ^ 0x8000 把 [0,0xFFFF] 单调映射
    // 到 [-32768, 32767]。下面的第二档计数也复用它。
    const __m128i sign = _mm_set1_epi16((short)0x8000);
    {
        __m128i mx = _mm_set1_epi16((short)-32768);   // 翻转域里的最小值
        __m128i mn = _mm_set1_epi16((short)32767);
        for (; i + 8 <= n; i += 8) {
            __m128i v = _mm_xor_si128(_mm_loadu_si128((const __m128i*)(p + i)), sign);
            mx = _mm_max_epi16(mx, v);
            mn = _mm_min_epi16(mn, v);
        }
        __m128i hi = _mm_srli_si128(mx, 8);
        mx = _mm_max_epi16(mx, hi);
        hi = _mm_srli_si128(mx, 4);
        mx = _mm_max_epi16(mx, hi);
        hi = _mm_srli_si128(mx, 2);
        mx = _mm_max_epi16(mx, hi);
        maxv = (uint32_t)(uint16_t)(_mm_extract_epi16(mx, 0) ^ 0x8000);

        hi = _mm_srli_si128(mn, 8);
        mn = _mm_min_epi16(mn, hi);
        hi = _mm_srli_si128(mn, 4);
        mn = _mm_min_epi16(mn, hi);
        hi = _mm_srli_si128(mn, 2);
        mn = _mm_min_epi16(mn, hi);
        minv = (uint32_t)(uint16_t)(_mm_extract_epi16(mn, 0) ^ 0x8000);
    }
#endif
    for (; i < n; i++) {
        uint32_t c = (uint32_t)p[i];
        if (c > maxv)
            maxv = c;
        if (c < minv)
            minv = c;
    }

    if (maxv < 0x80)
        return (int64_t)n;                      // 全 ASCII
    if (minv >= 0x800 && maxv < 0xD800)
        return (int64_t)n * 3;                  // 全 BMP 非代理（CJK 等）
    if (minv >= 0x80 && maxv < 0x800)
        return (int64_t)n * 2;                  // 全 2 字节区

    // ---- 第二档：混合段落，用 SIMD 统计各档个数 ----
    // 不含代理项时：
    //   len = (#c < 0x80) * 1 + (#0x80 <= c < 0x800) * 2 + (#c >= 0x800) * 3
    //       = n + (#c >= 0x80) + (#c >= 0x800)
    // 所以只要数出两个阈值的超限个数即可，不需要逐字符分支。
    if (maxv < 0xD800) {
#if defined(__aarch64__)
        {
            // NEON 有原生的无符号 vcgeq_u16，直接表达 `c >= 0x80` /
            // `c >= 0x800`，不需要 x86 那套“阈值减 1 + 符号位翻转”的替代写法。
            const uint16x8_t t80 = vdupq_n_u16(0x80);
            const uint16x8_t t800 = vdupq_n_u16(0x800);
            int64_t c80 = 0, c800 = 0;

            i = 0;
            while (i + 8 * NEON_UNROLL <= n) {
                uint32_t end = i + NEON_REDUCE_UNITS;
                if (end > n)
                    end = n;

                uint16x8_t a0 = vdupq_n_u16(0), a1 = a0, a2 = a0, a3 = a0;
                uint16x8_t b0 = a0, b1 = a0, b2 = a0, b3 = a0;

                for (; i + 8 * NEON_UNROLL <= end; i += 8 * NEON_UNROLL) {
                    uint16x8_t v0 = vld1q_u16(V16(p + i));
                    uint16x8_t v1 = vld1q_u16(V16(p + i + 8));
                    uint16x8_t v2 = vld1q_u16(V16(p + i + 16));
                    uint16x8_t v3 = vld1q_u16(V16(p + i + 24));
                    // 0xFFFF -> 1：借位减，累加器会向左邻借但总和不变
                    a0 = vsubq_u16(a0, vcgeq_u16(v0, t80));
                    a1 = vsubq_u16(a1, vcgeq_u16(v1, t80));
                    a2 = vsubq_u16(a2, vcgeq_u16(v2, t80));
                    a3 = vsubq_u16(a3, vcgeq_u16(v3, t80));
                    b0 = vsubq_u16(b0, vcgeq_u16(v0, t800));
                    b1 = vsubq_u16(b1, vcgeq_u16(v1, t800));
                    b2 = vsubq_u16(b2, vcgeq_u16(v2, t800));
                    b3 = vsubq_u16(b3, vcgeq_u16(v3, t800));
                }

                c80 += hsum_epi16(vaddq_u16(vaddq_u16(a0, a1), vaddq_u16(a2, a3)));
                c800 += hsum_epi16(vaddq_u16(vaddq_u16(b0, b1), vaddq_u16(b2, b3)));
            }
            // 尾段：单路 8 lane
            if (i + 8 <= n) {
                uint16x8_t a0 = vdupq_n_u16(0), b0 = a0;
                for (; i + 8 <= n; i += 8) {
                    uint16x8_t v = vld1q_u16(V16(p + i));
                    a0 = vsubq_u16(a0, vcgeq_u16(v, t80));
                    b0 = vsubq_u16(b0, vcgeq_u16(v, t800));
                }
                c80 += hsum_epi16(a0);
                c800 += hsum_epi16(b0);
            }
            for (; i < n; i++) {
                uint32_t c = (uint32_t)p[i];
                if (c >= 0x80)
                    c80++;
                if (c >= 0x800)
                    c800++;
            }
            return (int64_t)n + c80 + c800;
        }
#elif defined(__SSE2__)
        {
            // **必须减 1**：语义是 `c >= 0x80` / `c >= 0x800`，而
            // `_mm_cmpgt_epi16` 是**严格大于**。直接拿 0x80 / 0x800 当基准会把
            // “恰好等于阈值”的 code unit 漏掉：
            //   'a'*7 + U+0800   -> 算 9 字节（应为 10）
            //   'a'*8 + U+0800*8 -> 算 24 字节（应为 32）
            // byteLength 是 from() 分配的依据，少算会**静默截断**数据。
            const __m128i b80 = _mm_set1_epi16(0x80 - 1);
            const __m128i b800 = _mm_set1_epi16(0x800 - 1);
            int64_t c80 = 0, c800 = 0;

            // 16 位 lane 每处理 8 个 code unit 加 1，到 32767 就会回绕成负数。
            // 每 SIMD_REDUCE_UNITS 个 code unit 归约一次，把上限钉在安全区。
            const uint32_t SIMD_REDUCE_UNITS = 8192;
            i = 0;
            while (i + 8 <= n) {
                uint32_t end = i + SIMD_REDUCE_UNITS;
                if (end > n)
                    end = n;

                __m128i acc80 = _mm_setzero_si128();
                __m128i acc800 = _mm_setzero_si128();
                for (; i + 8 <= end; i += 8) {
                    __m128i v = _mm_loadu_si128((const __m128i*)(p + i));
                    // c >= 0x80 ? 1 : 0（无符号比较，同样用符号位翻转）
                    __m128i ge80 = _mm_cmpgt_epi16(_mm_xor_si128(v, sign), _mm_xor_si128(b80, sign));
                    __m128i ge800 = _mm_cmpgt_epi16(_mm_xor_si128(v, sign), _mm_xor_si128(b800, sign));
                    // 0xFFFF -> 1
                    acc80 = _mm_sub_epi16(acc80, ge80);
                    acc800 = _mm_sub_epi16(acc800, ge800);
                }
                c80 += hsum_epi16(acc80);
                c800 += hsum_epi16(acc800);
            }
            for (; i < n; i++) {
                uint32_t c = (uint32_t)p[i];
                if (c >= 0x80)
                    c80++;
                if (c >= 0x800)
                    c800++;
            }
            return (int64_t)n + c80 + c800;
        }
#else
        int64_t c80 = 0, c800 = 0;
        for (i = 0; i < n; i++) {
            uint32_t c = (uint32_t)p[i];
            if (c >= 0x80)
                c80++;
            if (c >= 0x800)
                c800++;
        }
        return (int64_t)n + c80 + c800;
#endif
    }

    // ---- 第三档：含代理项 ----
    // 仍用"计数"而不是逐字符分支。基准公式（把每个 code unit 当独立码点）：
    //     base = n + (#c >= 0x80) + (#c >= 0x800)
    // 修正：一个**合法代理对**占 2 个 code unit、真值 4 字节，而 base 里算了
    // 3 + 3 = 6，所以要减 2。孤代理项在 base 里正好是 3（U+FFFD 的 UTF-8 长度），
    // 不需要修正 —— 与 V8 Utf8Length 一致。
    //
    // 数合法对的办法：合法对满足 c[i] 是 0xD800..0xDBFF、c[i+1] 是 0xDC00..0xDFFF。
    // 把"高代理"掩码**右移**一个 lane（即本 lane 存的是"上一个 lane 是不是高代理"），
    // 跨分组边界的那一对由上一组的 lane7 补进本组 lane0。
    //
    // 若整段没有任何代理项（最常见：BMP 文本里混了个 U+FFFD），计数结果直接可用，
    // 完全不走逐字符循环。
    {
#if defined(__aarch64__)
        {
            // 所有 code unit 都 >= 0x800（minv 由第一遍扫描得出）时，
            // c80 与 c800 恒等于 n，base = 3n 是已知量，只剩数代理对。
            if (minv >= 0x800)
                return (int64_t)n * 3 - 2 * neon_count_pairs(p, n);

            const uint16x8_t t80 = vdupq_n_u16(0x80);
            const uint16x8_t t800 = vdupq_n_u16(0x800);
            const uint16x8_t mFC00 = vdupq_n_u16(0xFC00);
            const uint16x8_t vD800 = vdupq_n_u16(0xD800);
            const uint16x8_t vDC00 = vdupq_n_u16(0xDC00);
            int64_t c80 = 0, c800 = 0, pairs = 0;

            // prevHi 保存上一组 8 lane 的“高代理”掩码；跨组的那一对靠
            // vextq_u16(prevHi, isHi, 7) 拼出来（lane0 = prevHi.lane7）。
            uint16x8_t prevHi = vdupq_n_u16(0);
            uint32_t k = 0;

            while (k + 8 * NEON_UNROLL <= n) {
                uint32_t end = k + NEON_REDUCE_UNITS;
                if (end > n)
                    end = n;

                uint16x8_t a0 = vdupq_n_u16(0), a1 = a0, a2 = a0, a3 = a0;
                uint16x8_t b0 = a0, b1 = a0, b2 = a0, b3 = a0;
                uint16x8_t d0 = a0, d1 = a0, d2 = a0, d3 = a0;

                for (; k + 8 * NEON_UNROLL <= end; k += 8 * NEON_UNROLL) {
                    uint16x8_t v0 = vld1q_u16(V16(p + k));
                    uint16x8_t v1 = vld1q_u16(V16(p + k + 8));
                    uint16x8_t v2 = vld1q_u16(V16(p + k + 16));
                    uint16x8_t v3 = vld1q_u16(V16(p + k + 24));

                    a0 = vsubq_u16(a0, vcgeq_u16(v0, t80));
                    a1 = vsubq_u16(a1, vcgeq_u16(v1, t80));
                    a2 = vsubq_u16(a2, vcgeq_u16(v2, t80));
                    a3 = vsubq_u16(a3, vcgeq_u16(v3, t80));
                    b0 = vsubq_u16(b0, vcgeq_u16(v0, t800));
                    b1 = vsubq_u16(b1, vcgeq_u16(v1, t800));
                    b2 = vsubq_u16(b2, vcgeq_u16(v2, t800));
                    b3 = vsubq_u16(b3, vcgeq_u16(v3, t800));

                    uint16x8_t k0 = vandq_u16(v0, mFC00);
                    uint16x8_t h0 = vceqq_u16(k0, vD800);
                    d0 = vsubq_u16(d0, vandq_u16(vextq_u16(prevHi, h0, 7), vceqq_u16(k0, vDC00)));
                    prevHi = h0;

                    uint16x8_t k1 = vandq_u16(v1, mFC00);
                    uint16x8_t h1 = vceqq_u16(k1, vD800);
                    d1 = vsubq_u16(d1, vandq_u16(vextq_u16(prevHi, h1, 7), vceqq_u16(k1, vDC00)));
                    prevHi = h1;

                    uint16x8_t k2 = vandq_u16(v2, mFC00);
                    uint16x8_t h2 = vceqq_u16(k2, vD800);
                    d2 = vsubq_u16(d2, vandq_u16(vextq_u16(prevHi, h2, 7), vceqq_u16(k2, vDC00)));
                    prevHi = h2;

                    uint16x8_t k3 = vandq_u16(v3, mFC00);
                    uint16x8_t h3 = vceqq_u16(k3, vD800);
                    d3 = vsubq_u16(d3, vandq_u16(vextq_u16(prevHi, h3, 7), vceqq_u16(k3, vDC00)));
                    prevHi = h3;
                }

                c80 += hsum_epi16(vaddq_u16(vaddq_u16(a0, a1), vaddq_u16(a2, a3)));
                c800 += hsum_epi16(vaddq_u16(vaddq_u16(b0, b1), vaddq_u16(b2, b3)));
                pairs += hsum_epi16(vaddq_u16(vaddq_u16(d0, d1), vaddq_u16(d2, d3)));
            }

            // 尾段：单路 8 lane，逐 lane 水平求和
            if (k + 8 <= n) {
                uint16x8_t a0 = vdupq_n_u16(0), b0 = a0, d0 = a0;
                for (; k + 8 <= n; k += 8) {
                    uint16x8_t v = vld1q_u16(V16(p + k));
                    a0 = vsubq_u16(a0, vcgeq_u16(v, t80));
                    b0 = vsubq_u16(b0, vcgeq_u16(v, t800));
                    uint16x8_t kk = vandq_u16(v, mFC00);
                    uint16x8_t h = vceqq_u16(kk, vD800);
                    d0 = vsubq_u16(d0, vandq_u16(vextq_u16(prevHi, h, 7), vceqq_u16(kk, vDC00)));
                    prevHi = h;
                }
                c80 += hsum_epi16(a0);
                c800 += hsum_epi16(b0);
                pairs += hsum_epi16(d0);
            }

            // 跨组的最后一个高代理（prevHi.lane7）与接下来的标量字符的配对
            uint32_t carry_hi = (uint32_t)vgetq_lane_u16(prevHi, 7) & 1;
            for (; k < n; k++) {
                uint32_t c = (uint32_t)p[k];
                if (c >= 0x80)
                    c80++;
                if (c >= 0x800)
                    c800++;
                if (carry_hi && (c & 0xFC00) == 0xDC00)
                    pairs++;
                carry_hi = ((c & 0xFC00) == 0xD800);
            }

            return (int64_t)n + c80 + c800 - 2 * pairs;
        }
#elif defined(__SSE2__)
        // 同第二档：cmpgt 是严格大于，用“阈值 - 1”才等价于 >=。
        const __m128i b80 = _mm_set1_epi16(0x80 - 1);
        const __m128i b800 = _mm_set1_epi16(0x800 - 1);
        const __m128i mFC00 = _mm_set1_epi16((short)0xFC00);
        const __m128i vD800 = _mm_set1_epi16((short)0xD800);
        const __m128i vDC00 = _mm_set1_epi16((short)0xDC00);
        int64_t c80 = 0, c800 = 0, pairs = 0;

        // 上一组的"高代理"掩码（只需要 lane7），跨组那一对靠它补
        __m128i prevHi = _mm_setzero_si128();
        // 尾段仍需知道"最后一个 SIMD lane 是否为高代理"
        uint32_t carry_hi = 0;

        // 同第二档：16 位 lane 会回绕，按固定周期归约
        const uint32_t SIMD_REDUCE_UNITS = 8192;
        uint32_t k = 0;
        while (k + 8 <= n) {
            uint32_t end = k + SIMD_REDUCE_UNITS;
            if (end > n)
                end = n;

            __m128i acc80 = _mm_setzero_si128();
            __m128i acc800 = _mm_setzero_si128();
            __m128i accPair = _mm_setzero_si128();

            for (; k + 8 <= end; k += 8) {
                __m128i v = _mm_loadu_si128((const __m128i*)(p + k));
                __m128i sv = _mm_xor_si128(v, sign);
                acc80 = _mm_sub_epi16(acc80, _mm_cmpgt_epi16(sv, _mm_xor_si128(b80, sign)));
                acc800 = _mm_sub_epi16(acc800, _mm_cmpgt_epi16(sv, _mm_xor_si128(b800, sign)));

                __m128i isHi = _mm_cmpeq_epi16(_mm_and_si128(v, mFC00), vD800);
                __m128i isLo = _mm_cmpeq_epi16(_mm_and_si128(v, mFC00), vDC00);

                // 本 lane 表示"紧邻的前一个 lane 是否为高代理"：
                //   slli(isHi, 2)    组内把 lane i-1 的 isHi 挪到 lane i
                //   srli(prevHi, 14) 把**上一组** lane7 的 isHi 挪到本组 lane0
                //                   （必须是右移：slli 14 会把 lane7 整体移出寄存器）
                __m128i hiNext = _mm_or_si128(_mm_slli_si128(isHi, 2),
                    _mm_srli_si128(prevHi, 14));
                accPair = _mm_sub_epi16(accPair, _mm_and_si128(hiNext, isLo));
                prevHi = isHi;
            }

            c80 += hsum_epi16(acc80);
            c800 += hsum_epi16(acc800);
            pairs += hsum_epi16(accPair);
        }
        // 跨分组边界：最后一组的 lane7 与尾段首元素的配对在下面的循环里补
        carry_hi = (uint32_t)_mm_extract_epi16(prevHi, 7) & 1;

        // SIMD 尾段（顺便把跨分组边界的那一对算进去）
        for (; k < n; k++) {
            uint32_t c = (uint32_t)p[k];
            if (c >= 0x80)
                c80++;
            if (c >= 0x800)
                c800++;
            if (carry_hi && (c & 0xFC00) == 0xDC00)
                pairs++;
            carry_hi = ((c & 0xFC00) == 0xD800);
        }

        return (int64_t)n + c80 + c800 - 2 * pairs;
#else
        int64_t c80 = 0, c800 = 0, pairs = 0;
        bool prevHi = false;
        for (uint32_t k = 0; k < n; k++) {
            uint32_t c = (uint32_t)p[k];
            if (c >= 0x80)
                c80++;
            if (c >= 0x800)
                c800++;
            bool isHi = (c & 0xFC00) == 0xD800;
            bool isLo = (c & 0xFC00) == 0xDC00;
            if (prevHi && isLo)
                pairs++;
            prevHi = isHi;
        }
        return (int64_t)n + c80 + c800 - 2 * pairs;
#endif
    }
}

void Buffer::class_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t arg_cnt = args.Length();
    while (arg_cnt > 0 && args[arg_cnt - 1]->IsUndefined())
        --arg_cnt;

    if (arg_cnt == 0) {
        ThrowResult(result_t(CALL_E_PARAMNOTOPTIONAL));
        return;
    }

    Isolate* isolate = Isolate::current(args);

    v8::Local<v8::String> data;
    if (args[0]->IsString())
        data = args[0].As<v8::String>();
    else if (args[0]->IsStringObject())
        data = args[0].As<v8::StringObject>()->ValueOf();
    else if (args[0]->IsUint8Array()) {
        v8::Local<v8::Uint8Array> buf = args[0].As<v8::Uint8Array>();
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, buf->ByteLength()));
        return;
    } else if (args[0]->IsArrayBuffer()) {
        v8::Local<v8::ArrayBuffer> buf = args[0].As<v8::ArrayBuffer>();
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, buf->ByteLength()));
        return;
    } else if (args[0]->IsDataView()) {
        v8::Local<v8::DataView> view = args[0].As<v8::DataView>();
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, view->ByteLength()));
        return;
    } else if (args[0]->IsSharedArrayBuffer()) {
        v8::Local<v8::SharedArrayBuffer> buf = args[0].As<v8::SharedArrayBuffer>();
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, buf->ByteLength()));
        return;
    } else {
        ThrowResult(result_t::type_mismatch());
        return;
    }

    v8::Local<v8::String> codec;
    if (arg_cnt > 1) {
        if (args[arg_cnt - 1]->IsString())
            codec = args[--arg_cnt].As<v8::String>();
        else if (args[arg_cnt - 1]->IsStringObject())
            codec = args[--arg_cnt].As<v8::StringObject>()->ValueOf();
    }

    if (codec.IsEmpty()) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int32_t)utf8ByteLength(isolate->m_isolate, data)));
        return;
    }

    v8::String::Utf8Value codec_utf8(isolate->m_isolate, codec);

    if (!strcmp(*codec_utf8, "ascii") || !strcmp(*codec_utf8, "buffer")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Length()));
        return;
    } else if (!strcmp(*codec_utf8, "hex")) {
        // node: hex 的 byteLength 是 floor(len / 2)，**不校验内容**
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Length() / 2));
        return;
    } else if (!strcmp(*codec_utf8, "utf8") || !strcmp(*codec_utf8, "utf-8")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int32_t)utf8ByteLength(isolate->m_isolate, data)));
        return;
    } else if (!strcmp(*codec_utf8, "ucs2") || !strcmp(*codec_utf8, "ucs-2")
        || !strcmp(*codec_utf8, "utf16") || !strcmp(*codec_utf8, "utf-16")
        || !strcmp(*codec_utf8, "utf16le") || !strcmp(*codec_utf8, "utf-16le")
        || !strcmp(*codec_utf8, "utf16be") || !strcmp(*codec_utf8, "utf-16be")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Length() * 2));
        return;
    }

    // node: 未知编码按 utf8 计长（不抛错）；fibjs 的 charset 编码仍走真实换算
    if (!is_native_codec(*codec_utf8) && !encoding_conv::is_encoding(*codec_utf8)) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int32_t)utf8ByteLength(isolate->m_isolate, data)));
        return;
    }

    exlib::string data_str;
    GetArgumentValue(isolate, data, data_str);

    // base58：长度依赖真实的多精度换算，但**不能因内容非法而报错**。
    // 对齐 node 对 hex/base64 的 byteLength 语义（'!!!!' 按 base64 也算 3 字节）：
    // 字母表外的字符被过滤掉再算长度，而不是抛 "base58: encode error"。
    if (*codec_utf8 && !strcmp(*codec_utf8, "base58"))
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate,
            (int32_t)base58ByteLength(data_str.c_str(), data_str.length())));
    else {
        exlib::string strBuf;
        result_t hr = commonDecode(*codec_utf8, data_str, strBuf);
        if (hr.failed()) {
            ThrowResult(hr);
            return;
        }

        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, (int32_t)strBuf.length()));
    }
}

inline bool is_native_codec(exlib::string codec)
{
    return (codec == "hex")
        || (codec == "base32") || (codec == "base58")
        || (codec == "base64") || (codec == "base64url")

        || (codec == "utf8") || (codec == "utf-8")

        || (codec == "ucs2") || (codec == "ucs-2")
        || (codec == "utf16") || (codec == "utf-16")

        || (codec == "ucs2le") || (codec == "ucs-2le")
        || (codec == "utf16le") || (codec == "utf-16le")

        || (codec == "ucs2be") || (codec == "ucs-2be")
        || (codec == "utf16be") || (codec == "utf-16be")

        || (codec == "ucs4") || (codec == "ucs-4")
        || (codec == "utf32") || (codec == "utf-32")

        || (codec == "ucs4le") || (codec == "ucs-4le")
        || (codec == "utf32le") || (codec == "utf-32le")

        || (codec == "ucs4be") || (codec == "ucs-4be")
        || (codec == "utf32be") || (codec == "utf-32be")

        || (codec == "binary") || (codec == "latin1");
}

inline bool static_is_safe_codec(exlib::string codec)
{
    Isolate* isolate = NULL;
    Runtime* rt = Runtime::current();
    if (rt)
        isolate = rt->safe_isolate();

    return (isolate && !isolate->m_safe_buffer) || is_native_codec(codec);
}

result_t Buffer_base::isEncoding(exlib::string codec, bool& retVal)
{
    if (!static_is_safe_codec(codec)) {
        retVal = false;
        return result_t();
    }

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "hex")
        || (codec == "base32") || (codec == "base58")
        || (codec == "base64") || (codec == "base64url")) {
        retVal = true;
    } else {
        retVal = encoding_conv::is_encoding(codec);
    }
    return result_t();
}

result_t Buffer_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return result_t();
}

bool Buffer::is_safe_codec(exlib::string codec)
{
    Isolate* isolate = get_holder();
    if (!isolate)
        return static_is_safe_codec(codec);

    return !isolate->m_safe_buffer || is_native_codec(codec);
}

result_t Buffer_base::byteLength(exlib::string str, exlib::string codec, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, codec, buf);
    if (hr.failed())
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return result_t();
}

result_t Buffer_base::byteLength(Union_byteLength_str str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(str)) {
        retVal = (int32_t)Buffer::Cast(std::get<obj_ptr<Buffer_base>>(str))->length();

        return result_t();
    }

    Union_from_datas datas = std::holds_alternative<v8::Local<v8::Uint8Array>>(str)
        ? Union_from_datas(std::get<v8::Local<v8::Uint8Array>>(str))
        : Union_from_datas(std::get<std::shared_ptr<v8::BackingStore>>(str));

    result_t hr = from(datas, 0, -1, buf);
    if (hr.failed())
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return result_t();
}

result_t Buffer::get_length(int32_t& retVal)
{
    retVal = length();
    return result_t();
}

result_t Buffer_base::compare(Buffer_base* buf1, Buffer_base* buf2, int32_t& retVal)
{
    return buf1->compare(buf2, retVal);
}

result_t Buffer::write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal)
{
    if (!is_safe_codec(codec))
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Unknown codec: '%s'.", codec.c_str());

    int32_t max_length = 0;
    int32_t buffer_length = (int32_t)Buffer::length();

    if (offset < 0 || length < -1)
        return result_t::invalid_arg();
    if (buffer_length < offset)
        return result_t::out_of_range();

    if (buffer_length == offset || length == 0) {
        retVal = 0;
        return result_t();
    }

    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr.failed())
        return hr;

    max_length = (int32_t)strBuf.length();
    max_length = MIN(max_length, buffer_length - offset);
    if (length > 0)
        max_length = MIN(max_length, length);

    memcpy(data() + offset, strBuf.c_str(), max_length);
    retVal = max_length;

    return result_t();
}

result_t Buffer::write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal)
{
    return write(str, offset, -1, codec, retVal);
}

result_t Buffer::write(exlib::string str, exlib::string codec, int32_t& retVal)
{
    return write(str, 0, -1, codec, retVal);
}

result_t Buffer::fill(Union_fill_v v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<int32_t>(v)) {
        result_t hr = generateEnd((int32_t)length(), offset, end);
        if (hr.failed())
            return hr;

        memset(data() + offset, std::get<int32_t>(v) & 255, end - offset);
    } else {
        Buffer* v_data = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(v));

        result_t hr = fill(v_data->data(), v_data->length(), offset, end);
        if (hr.failed())
            return hr;
    }

    retVal = this;
    return result_t();
}

result_t Buffer::fill(const uint8_t* buf, size_t sz, int32_t offset, int32_t end)
{
    if (sz == 0)
        return result_t();

    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr.failed())
        return hr;

    uint8_t* _data = data();
    if (sz == 1)
        memset(_data + offset, *buf, end - offset);
    else {
        int32_t length = end - offset;
        while (length > 0) {
            memcpy(_data + offset, buf, MIN(sz, length));
            length -= sz;
            offset += sz;

            buf = _data;
            sz = offset;
        }
    }

    return result_t();
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    if (!is_safe_codec(codec))
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Unknown codec: '%s'.", codec.c_str());

    exlib::string strBuf;
    result_t hr = commonDecode(codec, v, strBuf);
    if (hr.failed())
        return hr;

    hr = fill((const uint8_t*)strBuf.c_str(), strBuf.length(), offset, end);
    if (hr.failed())
        return hr;

    retVal = this;
    return result_t();
}

result_t Buffer::fill(exlib::string v, int32_t offset, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    return fill(v, offset, -1, codec, retVal);
}

result_t Buffer::fill(exlib::string v, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    return fill(v, 0, -1, codec, retVal);
}

result_t Buffer::fill(int32_t v, int32_t offset, int32_t end)
{
    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr.failed())
        return hr;

    memset(data() + offset, v & 255, end - offset);

    return result_t();
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end)
{
    Buffer* v_data = Buffer::Cast(v);
    result_t hr = fill(v_data->data(), v_data->length(), offset, end);
    if (hr.failed())
        return hr;

    return result_t();
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec)
{
    if (!is_safe_codec(codec))
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Unknown codec: '%s'.", codec.c_str());

    exlib::string strBuf;
    result_t hr = commonDecode(codec, v, strBuf);
    if (hr.failed())
        return hr;

    hr = fill((const uint8_t*)strBuf.c_str(), strBuf.length(), offset, end);
    if (hr.failed())
        return hr;

    return result_t();
}

result_t Buffer::fill(exlib::string v, int32_t offset, exlib::string codec)
{
    return fill(v, offset, -1, codec);
}

result_t Buffer::fill(exlib::string v, exlib::string codec)
{
    return fill(v, 0, -1, codec);
}

result_t Buffer::copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart,
    int32_t sourceEnd, int32_t& retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return result_t::out_of_range();

    if (sourceStart > (int32_t)length())
        return result_t::out_of_range();

    Buffer* buf = Buffer::Cast(targetBuffer);
    int32_t bufLen = buf->length();

    if (sourceEnd == -1)
        sourceEnd = (int32_t)length();

    if (targetStart >= bufLen || sourceStart >= sourceEnd) {
        retVal = 0;
        return result_t();
    }

    int32_t targetSz = bufLen - targetStart;
    int32_t sourceSz = (int32_t)length() - sourceStart;
    int32_t sourceLen = sourceEnd - sourceStart;
    int32_t sz = MIN(MIN(sourceLen, targetSz), sourceSz);

    memmove(buf->data() + targetStart, data() + sourceStart, sz);

    retVal = sz;

    return result_t();
}

result_t Buffer::set(Buffer_base* src, int32_t start, int32_t& retVal)
{
    Buffer* buf = Buffer::Cast(src);
    int32_t len = buf->length();
    if (len + start > (int32_t)length())
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Source is too large: %d bytes at offset %d exceeds buffer size %d.", len, start, (int32_t)length());

    return src->copy(this, start, 0, -1, retVal);
}

result_t Buffer::readNumber(int32_t offset, char* buf, int32_t size,
    int32_t value_size, bool le)
{
    size_t data_len = length();

    if (offset < 0 || (size_t)offset > data_len)
        return result_t::out_of_range();

    if (offset + size > (int32_t)data_len)
        return result_t::out_of_range();

    const char* _data = (const char*)data() + offset;

    if (size == 1) {
        buf[0] = *_data;
        return result_t();
    }

    if (is_big_endian()) {
        if (!le)
            memcpy(buf + value_size - size, _data, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                buf[value_size - i - 1] = _data[i];
        }
    } else {
        if (le)
            memcpy(buf, _data, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                buf[size - i - 1] = _data[i];
        }
    }

    return result_t();
}

#define READ_NUMBER(t, le)                                                 \
    t v = 0;                                                               \
    result_t hr = readNumber(offset, (char*)&v, sizeof(v), sizeof(v), le); \
    if (hr.failed())                                                            \
        return hr;                                                         \
    retVal = v;                                                            \
    return result_t();

#define READ_U_NUMBER_48(t, sz, le)                                 \
    if (sz <= 0 || sz > 6)                                          \
        return result_t::invalid_arg();                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr.failed())                                                     \
        return hr;                                                  \
    retVal = v;                                                     \
    return result_t();

#define READ_NUMBER_48(t, sz, le)                                   \
    if (sz <= 0 || sz > 6)                                          \
        return result_t::invalid_arg();                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr.failed())                                                     \
        return hr;                                                  \
    if ((sz < 8) && (v & (((t)1) << (8 * sz - 1))))                 \
        v |= ((t) - 1) << (8 * sz);                                 \
    retVal = v;                                                     \
    return result_t();

result_t Buffer::readUInt8(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint8_t, true);
}

result_t Buffer::readUInt16LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint16_t, true);
}

result_t Buffer::readUInt16BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint16_t, false);
}

result_t Buffer::readUInt32LE(int32_t offset, double& retVal)
{
    READ_NUMBER(uint32_t, true);
}

result_t Buffer::readUInt32BE(int32_t offset, double& retVal)
{
    READ_NUMBER(uint32_t, false);
}

result_t Buffer::readUIntLE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_U_NUMBER_48(uint64_t, byteLength, true);
}

result_t Buffer::readUIntBE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_U_NUMBER_48(uint64_t, byteLength, false);
}

result_t Buffer::readInt8(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int8_t, true);
}

result_t Buffer::readInt16LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int16_t, true);
}

result_t Buffer::readInt16BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int16_t, false);
}

result_t Buffer::readInt32LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int32_t, true);
}

result_t Buffer::readInt32BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int32_t, false);
}

result_t Buffer::readIntLE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_NUMBER_48(int64_t, byteLength, true);
}

result_t Buffer::readIntBE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_NUMBER_48(int64_t, byteLength, false);
}

result_t Buffer::readInt64LE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(int64_t, true);
}

result_t Buffer::readInt64BE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(int64_t, false);
}

result_t Buffer::readFloatLE(int32_t offset, double& retVal)
{
    READ_NUMBER(float, true);
}

result_t Buffer::readFloatBE(int32_t offset, double& retVal)
{
    READ_NUMBER(float, false);
}

result_t Buffer::readDoubleLE(int32_t offset, double& retVal)
{
    READ_NUMBER(double, true);
}

result_t Buffer::readDoubleBE(int32_t offset, double& retVal)
{
    READ_NUMBER(double, false);
}

result_t Buffer::writeNumber(int32_t offset, const char* buf, int32_t size,
    int32_t value_size, bool le, int32_t& retVal)
{
    if (offset < 0 || (size_t)offset > length())
        return result_t::out_of_range();

    if (offset + size > (int32_t)length())
        return result_t::out_of_range();

    retVal = size + offset;

    char* _data = (char*)data() + offset;

    if (size == 1) {
        _data[0] = buf[0];
        return result_t();
    }

    if (is_big_endian()) {
        if (!le)
            memcpy(_data, buf + value_size - size, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                _data[i] = buf[value_size - i - 1];
        }
    } else {
        if (le)
            memcpy(_data, buf, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                _data[i] = buf[size - i - 1];
        }
    }

    return result_t();
}

#define WRITE_NUMBER(t, le)                                                         \
    t v = (t)value;                                                                 \
    result_t hr = writeNumber(offset, (char*)&v, sizeof(v), sizeof(v), le, retVal); \
    if (hr.failed())                                                                     \
        return hr;                                                                  \
    return result_t();

#define WRITE_NUMBER_48(t, sz, le)                                           \
    if (sz <= 0 || sz > 6)                                                   \
        return result_t::invalid_arg();                                            \
    t v = (t)value;                                                          \
    result_t hr = writeNumber(offset, (char*)&v, sz, sizeof(v), le, retVal); \
    if (hr.failed())                                                              \
        return hr;                                                           \
    return result_t();

result_t Buffer::writeUInt8(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint8_t, true);
}

result_t Buffer::writeUInt16LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, true);
}

result_t Buffer::writeUInt16BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, false);
}

result_t Buffer::writeUInt32LE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, true);
}

result_t Buffer::writeUInt32BE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, false);
}

result_t Buffer::writeUIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, byteLength, true);
}

result_t Buffer::writeUIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, byteLength, false);
}

result_t Buffer::writeInt8(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int8_t, true);
}

result_t Buffer::writeInt16LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, true);
}

result_t Buffer::writeInt16BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, false);
}

result_t Buffer::writeInt32LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, true);
}

result_t Buffer::writeInt32BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, false);
}

result_t Buffer::writeIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, byteLength, true);
}

result_t Buffer::writeIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, byteLength, false);
}

result_t Buffer::writeInt64LE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, true);
}

result_t Buffer::writeInt64BE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, false);
}

result_t Buffer::writeFloatLE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(float, true);
}

result_t Buffer::writeFloatBE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(float, false);
}

result_t Buffer::writeDoubleLE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(double, true);
}

result_t Buffer::writeDoubleBE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(double, false);
}

// Helper function for indexOf implementations
static result_t Buffer_indexOf(Buffer* buf, const uint8_t* v, int32_t v_len, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)buf->length();

    // Handle empty pattern
    if (v_len == 0) {
        // Handle negative offset like Node.js
        if (offset < 0) {
            offset = buf_length + offset;
            if (offset < 0)
                offset = 0;
        }
        // Clamp offset to buffer length
        retVal = MIN(offset, buf_length);
        return result_t();
    }

    // Handle negative offset like Node.js: Math.max(0, buf.length + offset)
    if (offset < 0) {
        offset = buf_length + offset;
        if (offset < 0)
            offset = 0;
    }

    // For large offsets, return -1 without throwing error
    if (offset >= buf_length) {
        retVal = -1;
        return result_t();
    }

    const uint8_t* find = exlib::qmemmem(buf->data() + offset, buf->length() - offset, v, v_len);

    retVal = find ? (int32_t)(find - buf->data()) : -1;
    return result_t();
}

result_t Buffer::indexOf(Union_indexOf_v v, int32_t offset, int32_t& retVal)
{
    if (std::holds_alternative<int32_t>(v))
        return indexOf(std::get<int32_t>(v), offset, retVal);

    if (std::holds_alternative<obj_ptr<Buffer_base>>(v))
        return indexOf(std::get<obj_ptr<Buffer_base>>(v).get(), offset, retVal);

    return indexOf(std::get<exlib::string>(v), offset, retVal);
}

result_t Buffer::indexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)length();

    // Handle negative offset like Node.js: Math.max(0, buf.length + offset)
    if (offset < 0) {
        offset = buf_length + offset;
        if (offset < 0)
            offset = 0;
    }

    result_t hr = validOffset(buf_length, offset);
    if (hr.failed())
        return hr;

    const uint8_t* _data = data();

    while (offset < buf_length) {
        if (_data[offset] == (v & 255)) {
            retVal = offset;
            return result_t();
        }
        offset++;
    }

    retVal = -1;
    return result_t();
}

result_t Buffer::indexOf(Buffer_base* v, int32_t offset, int32_t& retVal)
{
    Buffer* v_data = Buffer::Cast(v);
    return Buffer_indexOf(this, v_data->data(), v_data->length(), offset, retVal);
}

result_t Buffer::indexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    return Buffer_indexOf(this, (const uint8_t*)v.c_str(), v.length(), offset, retVal);
}

result_t Buffer::lastIndexOf(Union_lastIndexOf_v v, int32_t offset, int32_t& retVal)
{
    if (std::holds_alternative<int32_t>(v))
        return lastIndexOf(std::get<int32_t>(v), offset, retVal);

    if (std::holds_alternative<obj_ptr<Buffer_base>>(v))
        return lastIndexOf(std::get<obj_ptr<Buffer_base>>(v).get(), offset, retVal);

    return lastIndexOf(std::get<exlib::string>(v), offset, retVal);
}

result_t Buffer::lastIndexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)length();

    if (offset == -1)
        offset = buf_length - 1;

    result_t hr = validOffset(buf_length, offset);
    if (hr.failed())
        return hr;

    const uint8_t* _data = data();

    for (int32_t i = offset; i >= 0; --i) {
        if (_data[i] == (v & 255)) {
            retVal = i;
            return result_t();
        }
    }

    retVal = -1;
    return result_t();
}

static result_t Buffer_lastIndexOf(Buffer* buffer, const uint8_t* v, int32_t v_len, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)buffer->length();

    // Handle empty pattern search
    if (v_len == 0) {
        if (offset == -1)
            offset = buf_length;
        else if (offset < 0)
            offset = 0;
        else if (offset > buf_length)
            offset = buf_length;

        retVal = offset;
        return result_t();
    }

    // Handle empty buffer
    if (buf_length == 0) {
        retVal = -1;
        return result_t();
    }

    if (offset == -1)
        offset = buf_length - 1;

    // Handle negative offset like Node.js: clamp to 0
    if (offset < 0) {
        retVal = -1;
        return result_t();
    }

    // Handle large offsets without throwing error
    if (offset >= buf_length)
        offset = buf_length - 1;

    const uint8_t* _data = buffer->data();

    // Start from offset, but don't go beyond where the pattern could fit
    int32_t start_pos = MIN(offset, buf_length - v_len);

    for (int32_t i = start_pos; i >= 0; --i) {
        if (memcmp(_data + i, v, v_len) == 0) {
            retVal = i;
            return result_t();
        }
    }

    retVal = -1;
    return result_t();
}

result_t Buffer::lastIndexOf(Buffer_base* v, int32_t offset, int32_t& retVal)
{
    Buffer* v_data = Buffer::Cast(v);
    return Buffer_lastIndexOf(this, v_data->data(), v_data->length(), offset, retVal);
}

result_t Buffer::lastIndexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    return Buffer_lastIndexOf(this, (const uint8_t*)v.c_str(), v.length(), offset, retVal);
}

result_t Buffer::slice(int32_t start, obj_ptr<Buffer_base>& retVal)
{
    return slice(start, (int32_t)length(), retVal);
}

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    int32_t length = (int32_t)Buffer::length();

    if (start < 0)
        start = length + start;

    if (end < 0)
        end = length + end;

    if (start < 0)
        start = 0;

    if (end > length)
        end = length;

    if (start > end)
        start = end;

    if (start < end)
        retVal = new Buffer(data() + start, end - start);
    else
        retVal = new Buffer(NULL, 0);

    return result_t();
}

result_t Buffer::subarray(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    int32_t length = (int32_t)Buffer::length();

    // Normalize start
    if (start < 0)
        start = length + start;

    // Normalize end
    if (end < 0)
        end = length + end;

    // Clamp to valid range
    if (start < 0)
        start = 0;

    if (end > length)
        end = length;

    if (start > end)
        start = end;

    // Create zero-copy subarray by sharing the backing store
    if (start < end) {
        size_t newOffset = m_store.m_offset + start;
        size_t newLength = end - start;
        retVal = new Buffer(m_store.m_store, newOffset, newLength);
    } else {
        retVal = new Buffer(NULL, 0);
    }

    return result_t();
}

result_t Buffer::equals(object_base* expected, bool& retVal)
{
    obj_ptr<Buffer> buf = Buffer::getInstance(expected);
    if (!buf) {
        retVal = false;
        return result_t();
    }

    if (length() != buf->length()) {
        retVal = false;
        return result_t();
    }

    retVal = !memcmp(data(), buf->data(), length());
    return result_t();
}

result_t Buffer::compare(Buffer_base* buf, int32_t& retVal)
{
    Buffer* cmpdata = Buffer::Cast(buf);
    int32_t pos_length = (int32_t)length();
    int32_t neg_length = (int32_t)cmpdata->length();

    retVal = memcmp(data(), cmpdata->data(), MIN(pos_length, neg_length));
    if (retVal)
        return result_t();

    retVal = pos_length - neg_length;
    return result_t();
}

result_t Buffer::toString(exlib::string& retVal)
{
    retVal.assign((const char*)data(), length());
    return result_t();
}

result_t Buffer::toString(exlib::string codec, int32_t offset, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Unknown codec: '%s'.", codec.c_str());

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (offset >= sz) {
        retVal = "";
        return result_t();
    }

    if (offset > 0) {
        return commonEncode(codec, (const char*)data() + offset, sz - offset, retVal);
    } else {
        return commonEncode(codec, (const char*)data(), sz, retVal);
    }
}

result_t Buffer::toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return result_t::make(CALL_E_EXCEPTION).with_format("Buffer: Unknown codec: '%s'.", codec.c_str());

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (end < 0 || offset >= end) {
        retVal = "";
        return result_t();
    }

    if (end > sz)
        end = sz;

    if (offset < end) {
        return commonEncode(codec, (const char*)data() + offset, end - offset, retVal);
    } else {
        return commonEncode(codec, (const char*)data(), sz, retVal);
    }
}

result_t Buffer::toArray(std::vector<int32_t>& retVal)
{
    int32_t i;
    const uint8_t* _data = data();

    for (i = 0; i < (int32_t)length(); i++)
        retVal.push_back(_data[i]);

    return result_t();
}

result_t Buffer::hex(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return hex_base::encode(buf, retVal);
}

result_t Buffer::base32(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base32_base::encode(buf, retVal);
}

result_t Buffer::base58(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base58_base::encode(buf, retVal);
}

result_t Buffer::base64(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base64_base::encode(buf, false, retVal);
}

}
