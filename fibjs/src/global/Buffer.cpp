#include "object.h"
#include "Buffer.h"
#include "SandBox.h"
#include "encoding.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fibjs {

inline result_t generateEnd(const int32_t buffer_length, const int32_t offset, int32_t& end)
{
    if (end < 0)
        end = buffer_length + end + 1;

    if (offset < 0 || end < 0 || offset > end)
        return CALL_E_INVALIDARG;

    if (end > buffer_length)
        return CALL_E_OUTRANGE;

    return 0;
}

inline result_t validOffset(const int32_t buffer_length, const int32_t offset)
{
    if (offset < 0)
        return CALL_E_INVALIDARG;

    if (offset > buffer_length)
        return CALL_E_OUTRANGE;

    return 0;
}

result_t Buffer_base::_new(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, retVal);
}

result_t Buffer_base::_new(v8::Local<v8::ArrayBuffer> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, byteOffset, length, retVal);
}

result_t Buffer_base::_new(v8::Local<v8::Uint8Array> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, byteOffset, length, retVal);
}

result_t Buffer_base::_new(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(str, codec, retVal);
}

result_t Buffer_base::_new(int32_t size, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return allocUnsafe(size, retVal);
}

result_t Buffer_base::alloc(int32_t size, int32_t fill, obj_ptr<Buffer_base>& retVal)
{
    if (size < 0)
        size = 0;

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    buf->fill(fill, 0, size, retVal);
    retVal = buf;
    return 0;
}

result_t Buffer_base::alloc(int32_t size, exlib::string fill, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(fill, 0, size, codec, retVal);
}

result_t Buffer_base::alloc(int32_t size, Buffer_base* fill, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(fill, 0, size, retVal);
}

result_t Buffer_base::allocUnsafe(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return 0;
}

result_t Buffer_base::allocUnsafeSlow(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return 0;
}

result_t Buffer_base::from(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, datas->Length());
    retVal = buf;

    int32_t sz = (int32_t)buf->length();
    v8::Local<v8::Context> context = datas->GetCreationContextChecked();
    Isolate* isolate = Isolate::current(context);

    if (sz) {
        int32_t i;
        result_t hr;
        uint8_t* _str = buf->data();

        for (i = 0; i < sz; i++) {
            JSValue v = datas->Get(context, i);
            int32_t num;

            hr = GetArgumentValue(isolate, v, num);
            if (hr < 0)
                return CHECK_ERROR(hr);

            _str[i] = (uint8_t)num;
        }
    }

    return 0;
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

    return 0;
}

result_t Buffer_base::from(v8::Local<v8::ArrayBuffer> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    std::shared_ptr<v8::BackingStore> cnt = datas->GetBackingStore();

    if (byteOffset < 0)
        byteOffset = 0;

    if (length < 0)
        length = cnt->ByteLength() - byteOffset;

    if (length < 0 || length > cnt->ByteLength() - byteOffset)
        length = 0;

    retVal = new Buffer((uint8_t*)cnt->Data() + byteOffset, length);

    return 0;
}

result_t Buffer_base::from(v8::Local<v8::Uint8Array> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    if (byteOffset < 0)
        return CALL_E_INVALIDARG;

    if (length < 0)
        length = datas->ByteLength() - byteOffset;
    else if (length > datas->ByteLength() - byteOffset)
        return CALL_E_INVALIDARG;

    return from(datas->Buffer(), byteOffset + datas->ByteOffset(), length, retVal);
}

result_t Buffer_base::from(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr < 0)
        return hr;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());
    return 0;
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
            return CALL_E_INVALIDARG;
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

    return 0;
}

v8::Local<v8::Value> Buffer::load_module()
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _global = context->Global();
    v8::Local<v8::Value> _buffer;

    obj_ptr<SandBox> sbox = new SandBox(false);

    sbox->InstallModule("encoding", encoding_base::class_info().getModule(isolate));
    sbox->require("internal/buffer", "/builtin", _buffer);

    _global->Set(context, isolate->NewString("Buffer"), _buffer).IsJust();
    v8::Local<v8::Object> js_buffer = _buffer.As<v8::Function>()->CallAsConstructor(context, 0, NULL).FromMaybe(v8::Local<v8::Value>()).As<v8::Object>();
    v8::Local<v8::Object> js_buffer_proto = js_buffer->GetPrototype().As<v8::Object>();

    context->SetEmbedderData(kBufferClassIndex, _buffer);
    context->SetEmbedderData(kBufferPrototype, js_buffer_proto);

    js_buffer_proto->Set(context, isolate->NewString("compare"), isolate->NewFunction("compare", proto_compare)).IsJust();
    js_buffer_proto->Set(context, isolate->NewString("equals"), isolate->NewFunction("equals", proto_equals)).IsJust();
    js_buffer_proto->Set(context, isolate->NewString("indexOf"), isolate->NewFunction("indexOf", proto_indexOf)).IsJust();
    js_buffer_proto->Set(context, isolate->NewString("lastIndexOf"), isolate->NewFunction("lastIndexOf", proto_indexOf)).IsJust();

    v8::Local<v8::Object> js_buffer_class = _buffer.As<v8::Object>();
    js_buffer_class->Set(context, isolate->NewString("compare"), isolate->NewFunction("compare", s_static_compare)).IsJust();

    // js_buffer_proto->Set(context, isolate->NewString("copy"), isolate->NewFunction("copy", proto_copy)).IsJust();
    js_buffer_proto->Set(context, isolate->NewString("write"), isolate->NewFunction("write", proto_write)).IsJust();

    js_buffer_class->Set(context, isolate->NewString("native_fill"), isolate->NewFunction("fill", proto_fill)).IsJust();
    js_buffer_class->Set(context, isolate->NewString("byteLength"), isolate->NewFunction("byteLength", class_byteLength)).IsJust();

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

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Buffer_base>& vr, bool bStrict, const char* encoding)
{
    if (v.IsEmpty() || v->IsNumber() || v->IsNumberObject())
        return CALL_E_TYPEMISMATCH;

    if (!IsJSBuffer(v)) {
        if (bStrict)
            return CALL_E_TYPEMISMATCH;

        if (v->IsArrayBuffer()) {
            vr = new Buffer(v.As<v8::ArrayBuffer>());
            return 0;
        }

        if (v->IsDataView()) {
            v8::Local<v8::DataView> view = v.As<v8::DataView>();
            vr = new Buffer(view->Buffer()->GetBackingStore(), view->ByteOffset(), view->ByteLength());
            return 0;
        }

        if (v->IsString() || v->IsStringObject()) {
            exlib::string str;
            GetArgumentValue(isolate, v, str);
            return Buffer_base::from(str, encoding, vr);
        }

        if (v->IsArray())
            return Buffer_base::from(v.As<v8::Array>(), vr);

        if (!v->IsTypedArray())
            return CALL_E_TYPEMISMATCH;
    }

    vr = new Buffer(v.As<v8::TypedArray>());

    return 0;
}

#define BUFFER_INSTANCE()                                     \
    obj_ptr<Buffer> pInst = Buffer::getInstance(args.This()); \
    if (pInst == NULL) {                                      \
        ThrowResult(CALL_E_NOTINSTANCE);                      \
        return;                                               \
    }

void Buffer::proto_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer>, 0);

    hr = pInst->compare(v0, vr);

    METHOD_RETURN();
}

void Buffer::proto_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    BUFFER_INSTANCE();
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer>, 0);
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

    ARG(obj_ptr<Buffer>, 0);

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

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer>, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
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

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->lastIndexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer>, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->lastIndexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->lastIndexOf(v0, v1, vr);

    METHOD_RETURN();
}

void Buffer::proto_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!args.This()->IsUint8Array()) {
        ThrowResult(CALL_E_NOTINSTANCE);
        return;
    }

    store buf(args.This().As<v8::Uint8Array>());
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();

    int32_t arg_cnt = args.Length();
    while (args[arg_cnt - 1]->IsUndefined())
        --arg_cnt;

    if (arg_cnt == 0) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    v8::Local<v8::String> data;
    if (args[0]->IsString())
        data = args[0].As<v8::String>();
    else if (args[0]->IsStringObject())
        data = args[0].As<v8::StringObject>()->ValueOf();
    else {
        ThrowResult(CALL_E_TYPEMISMATCH);
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
            ThrowResult(CALL_E_TYPEMISMATCH);
            return;
        }

        offset = args[1]->Int32Value(context).FromMaybe(0);
        if (offset < 0 || offset > buf.length()) {
            ThrowResult(CALL_E_OUTRANGE);
            return;
        }
    }

    size_t max_length = 0;
    if (arg_cnt > 2) {
        if (!args[2]->IsNumber() && !args[2]->IsNumberObject()) {
            ThrowResult(CALL_E_TYPEMISMATCH);
            return;
        }
        max_length = args[2]->Int32Value(context).FromMaybe(0);
        max_length = std::min(buf.length() - offset, max_length);
    } else
        max_length = buf.length() - offset;

    int flags = v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION | v8::String::REPLACE_INVALID_UTF8;

    if (codec.IsEmpty()) {
        data->WriteUtf8(isolate->m_isolate, (char*)buf.data() + offset, max_length, nullptr, flags);
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, max_length));
        return;
    }

    v8::String::Utf8Value codec_utf8(isolate->m_isolate, codec);

    if (!strcmp(*codec_utf8, "ascii") || !strcmp(*codec_utf8, "buffer")) {
        if (data->IsExternalOneByte()) {
            auto ext = data->GetExternalOneByteStringResource();
            max_length = std::min(max_length, ext->length());
            memcpy(buf.data() + offset, ext->data(), max_length);
        } else
            max_length = data->WriteOneByte(isolate->m_isolate, buf.data() + offset, 0, max_length, flags);

        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, max_length));
        return;
    } else if (!strcmp(*codec_utf8, "utf8") || !strcmp(*codec_utf8, "utf-8")) {
        max_length = data->WriteUtf8(isolate->m_isolate, (char*)buf.data() + offset, max_length, nullptr, flags);
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, max_length));
        return;
    }

    exlib::string data_str;
    GetArgumentValue(isolate, data, data_str);

    exlib::string strBuf;
    result_t hr = commonDecode(*codec_utf8, data_str, strBuf);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    max_length = std::min(max_length, strBuf.length());
    memcpy(buf.data() + offset, strBuf.c_str(), max_length);
    args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, max_length));
}

void Buffer::class_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t arg_cnt = args.Length();
    while (args[arg_cnt - 1]->IsUndefined())
        --arg_cnt;

    if (arg_cnt == 0) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
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
    } else {
        ThrowResult(CALL_E_TYPEMISMATCH);
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
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Utf8Length(isolate->m_isolate)));
        return;
    }

    v8::String::Utf8Value codec_utf8(isolate->m_isolate, codec);

    if (!strcmp(*codec_utf8, "ascii") || !strcmp(*codec_utf8, "buffer")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Length()));
        return;
    } else if (!strcmp(*codec_utf8, "utf8") || !strcmp(*codec_utf8, "utf-8")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Utf8Length(isolate->m_isolate)));
        return;
    } else if (!strcmp(*codec_utf8, "ucs2") || !strcmp(*codec_utf8, "ucs-2")
        || !strcmp(*codec_utf8, "utf16") || !strcmp(*codec_utf8, "utf-16")
        || !strcmp(*codec_utf8, "utf16le") || !strcmp(*codec_utf8, "utf-16le")
        || !strcmp(*codec_utf8, "utf16be") || !strcmp(*codec_utf8, "utf-16be")) {
        args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, data->Length() * 2));
        return;
    }

    exlib::string data_str;
    GetArgumentValue(isolate, data, data_str);

    exlib::string strBuf;
    result_t hr = commonDecode(*codec_utf8, data_str, strBuf);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(v8::Integer::New(isolate->m_isolate, strBuf.length()));
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
        return 0;
    }

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "hex")
        || (codec == "base32") || (codec == "base58")
        || (codec == "base64") || (codec == "base64url")) {
        retVal = true;
    } else {
        iconv_base::isEncoding(codec, retVal);
    }
    return 0;
}

result_t Buffer_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
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
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(v8::Local<v8::ArrayBuffer> str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(v8::Local<v8::Uint8Array> str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(Buffer_base* str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer::get_length(int32_t& retVal)
{
    retVal = length();
    return 0;
}

result_t Buffer_base::compare(Buffer_base* buf1, Buffer_base* buf2, int32_t& retVal)
{
    return buf1->compare(buf2, retVal);
}

result_t Buffer::write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t max_length = 0;
    int32_t buffer_length = (int32_t)Buffer::length();

    if (offset < 0 || length < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    if (buffer_length < offset)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (buffer_length == offset || length == 0) {
        retVal = 0;
        return 0;
    }

    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr < 0)
        return hr;

    max_length = (int32_t)strBuf.length();
    max_length = MIN(max_length, buffer_length - offset);
    if (length > 0)
        max_length = MIN(max_length, length);

    memcpy(data() + offset, strBuf.c_str(), max_length);
    retVal = max_length;

    return 0;
}

result_t Buffer::write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal)
{
    return write(str, offset, -1, codec, retVal);
}

result_t Buffer::write(exlib::string str, exlib::string codec, int32_t& retVal)
{
    return write(str, 0, -1, codec, retVal);
}

result_t Buffer::fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    memset(data() + offset, v & 255, end - offset);

    retVal = this;
    return 0;
}

result_t Buffer::fill(const uint8_t* buf, size_t sz, int32_t offset, int32_t end)
{
    if (sz == 0)
        return 0;

    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

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

    return 0;
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    Buffer* v_data = Buffer::Cast(v);
    result_t hr = fill(v_data->data(), v_data->length(), offset, end);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    exlib::string strBuf;
    result_t hr = commonDecode(codec, v, strBuf);
    if (hr < 0)
        return hr;

    hr = fill((const uint8_t*)strBuf.c_str(), strBuf.length(), offset, end);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
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
    if (hr < 0)
        return CHECK_ERROR(hr);

    memset(data() + offset, v & 255, end - offset);

    return 0;
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end)
{
    Buffer* v_data = Buffer::Cast(v);
    result_t hr = fill(v_data->data(), v_data->length(), offset, end);
    if (hr < 0)
        return hr;

    return 0;
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    exlib::string strBuf;
    result_t hr = commonDecode(codec, v, strBuf);
    if (hr < 0)
        return hr;

    hr = fill((const uint8_t*)strBuf.c_str(), strBuf.length(), offset, end);
    if (hr < 0)
        return hr;

    return 0;
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
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (sourceStart > (int32_t)length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    Buffer* buf = Buffer::Cast(targetBuffer);
    int32_t bufLen = buf->length();

    if (sourceEnd == -1)
        sourceEnd = (int32_t)length();

    if (targetStart >= bufLen || sourceStart >= sourceEnd) {
        retVal = 0;
        return 0;
    }

    int32_t targetSz = bufLen - targetStart;
    int32_t sourceSz = (int32_t)length() - sourceStart;
    int32_t sourceLen = sourceEnd - sourceStart;
    int32_t sz = MIN(MIN(sourceLen, targetSz), sourceSz);

    memmove(buf->data() + targetStart, data() + sourceStart, sz);

    retVal = sz;

    return 0;
}

result_t Buffer::set(Buffer_base* src, int32_t start, int32_t& retVal)
{
    Buffer* buf = Buffer::Cast(src);
    int32_t len = buf->length();
    if (len + start > (int32_t)length())
        return CHECK_ERROR(Runtime::setError("Buffer: Source is too large."));

    return src->copy(this, start, 0, -1, retVal);
}

result_t Buffer::readNumber(int32_t offset, char* buf, int32_t size,
    int32_t value_size, bool le)
{
    size_t data_len = length();

    if (offset < 0 || (size_t)offset > data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    const char* _data = (const char*)data() + offset;

    if (size == 1) {
        buf[0] = *_data;
        return 0;
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

    return 0;
}

#define READ_NUMBER(t, le)                                                 \
    t v = 0;                                                               \
    result_t hr = readNumber(offset, (char*)&v, sizeof(v), sizeof(v), le); \
    if (hr < 0)                                                            \
        return hr;                                                         \
    retVal = v;                                                            \
    return 0;

#define READ_U_NUMBER_48(t, sz, le)                                 \
    if (sz <= 0 || sz > 6)                                          \
        return CALL_E_INVALIDARG;                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr < 0)                                                     \
        return hr;                                                  \
    retVal = v;                                                     \
    return 0;

#define READ_NUMBER_48(t, sz, le)                                   \
    if (sz <= 0 || sz > 6)                                          \
        return CALL_E_INVALIDARG;                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr < 0)                                                     \
        return hr;                                                  \
    if ((sz < 8) && (v & (((t)1) << (8 * sz - 1))))                 \
        v |= ((t) - 1) << (8 * sz);                                 \
    retVal = v;                                                     \
    return 0;

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
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    retVal = size + offset;

    char* _data = (char*)data() + offset;

    if (size == 1) {
        _data[0] = buf[0];
        return 0;
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

    return 0;
}

#define WRITE_NUMBER(t, le)                                                         \
    t v = (t)value;                                                                 \
    result_t hr = writeNumber(offset, (char*)&v, sizeof(v), sizeof(v), le, retVal); \
    if (hr < 0)                                                                     \
        return hr;                                                                  \
    return 0;

#define WRITE_NUMBER_48(t, sz, le)                                           \
    if (sz <= 0 || sz > 6)                                                   \
        return CALL_E_INVALIDARG;                                            \
    t v = (t)value;                                                          \
    result_t hr = writeNumber(offset, (char*)&v, sz, sizeof(v), le, retVal); \
    if (hr < 0)                                                              \
        return hr;                                                           \
    return 0;

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

result_t Buffer::indexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)length();
    result_t hr = validOffset(buf_length, offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* _data = data();

    while (offset < buf_length) {
        if (_data[offset] == (v & 255)) {
            retVal = offset;
            return 0;
        }
        offset++;
    }

    retVal = -1;
    return 0;
}

result_t Buffer::indexOf(Buffer_base* v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    Buffer* v_data = Buffer::Cast(v);
    const uint8_t* find = exlib::qmemmem(data() + offset, length() - offset,
        v_data->data(), v_data->length());

    retVal = find ? (int32_t)(find - data()) : -1;
    return 0;
}

result_t Buffer::indexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* find = exlib::qmemmem(data() + offset, length() - offset,
        (const uint8_t*)v.c_str(), v.length());

    retVal = find ? (int32_t)(find - data()) : -1;
    return 0;
}

result_t Buffer::lastIndexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)length();
    result_t hr = validOffset(buf_length, offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* _data = data();

    for (int32_t i = buf_length - 1; i >= offset; --i) {
        if (_data[i] == (v & 255)) {
            retVal = i;
            return 0;
        }
    }

    retVal = -1;
    return 0;
}

result_t Buffer::lastIndexOf(Buffer_base* v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    Buffer* v_data = Buffer::Cast(v);
    const uint8_t* _data = data();
    const uint8_t* v_data_ptr = v_data->data();
    int32_t v_length = v_data->length();

    for (int32_t i = length() - v_length; i >= offset; --i) {
        if (memcmp(_data + i, v_data_ptr, v_length) == 0) {
            retVal = i;
            return 0;
        }
    }

    retVal = -1;
    return 0;
}

result_t Buffer::lastIndexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* _data = data();
    const uint8_t* v_data = (const uint8_t*)v.c_str();
    int32_t v_length = v.length();

    for (int32_t i = length() - v_length; i >= offset; --i) {
        if (memcmp(_data + i, v_data, v_length) == 0) {
            retVal = i;
            return 0;
        }
    }

    retVal = -1;
    return 0;
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

    return 0;
}

result_t Buffer::equals(object_base* expected, bool& retVal)
{
    obj_ptr<Buffer> buf = Buffer::getInstance(expected);
    if (!buf) {
        retVal = false;
        return 0;
    }

    if (length() != buf->length()) {
        retVal = false;
        return 0;
    }

    retVal = !memcmp(data(), buf->data(), length());
    return 0;
}

result_t Buffer::compare(Buffer_base* buf, int32_t& retVal)
{
    Buffer* cmpdata = Buffer::Cast(buf);
    int32_t pos_length = (int32_t)length();
    int32_t neg_length = (int32_t)cmpdata->length();

    retVal = memcmp(data(), cmpdata->data(), MIN(pos_length, neg_length));
    if (retVal)
        return 0;

    retVal = pos_length - neg_length;
    return 0;
}

result_t Buffer::toString(exlib::string& retVal)
{
    retVal.assign((const char*)data(), length());
    return 0;
}

result_t Buffer::toString(exlib::string codec, int32_t offset, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (offset >= sz) {
        retVal = "";
        return 0;
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
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (end < 0 || offset >= end) {
        retVal = "";
        return 0;
    }

    if (end > sz)
        end = sz;

    if (offset < end) {
        return commonEncode(codec, (const char*)data() + offset, end - offset, retVal);
    } else {
        return commonEncode(codec, (const char*)data(), sz, retVal);
    }
}

result_t Buffer::toArray(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate, (int32_t)length());
    int32_t i;
    const uint8_t* _data = data();

    for (i = 0; i < (int32_t)length(); i++)
        a->Set(context, i, v8::Number::New(isolate->m_isolate, _data[i])).IsJust();

    retVal = a;

    return 0;
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

result_t Buffer::unbind(obj_ptr<object_base>& retVal)
{
    retVal = new Buffer(this);
    return 0;
}

}
