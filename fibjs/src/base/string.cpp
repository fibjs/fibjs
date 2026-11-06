/*
 * string.cpp
 *
 *  Created on: Apr 25, 2021
 *      Author: lion
 */

#include "object.h"
#include "Isolate.h"
#include "simdutf.h"

namespace fibjs {

exlib::atomic g_ExtStringCount;

class ExtStringW : public v8::String::ExternalStringResource {
public:
    ExtStringW(v8::Isolate* _isolate, exlib::wstring _buffer)
        : m_isolate(_isolate)
        , m_buffer(std::move(_buffer))
    {
        g_ExtStringCount.inc();
        m_isolate->AdjustAmountOfExternalAllocatedMemory(m_buffer.length() * 2);
    }

    ~ExtStringW()
    {
        m_isolate->AdjustAmountOfExternalAllocatedMemory(-(int64_t)m_buffer.length() * 2);
        g_ExtStringCount.dec();
    }

public:
    virtual const uint16_t* data() const
    {
        return (const uint16_t*)m_buffer.c_str();
    }

    virtual size_t length() const
    {
        return m_buffer.length();
    }

    exlib::string str()
    {
        return utf16to8String(m_buffer);
    }

private:
    v8::Isolate* m_isolate;
    exlib::wstring m_buffer;
};

class ExtString : public v8::String::ExternalOneByteStringResource {
public:
    // `latin1 = true` 表示 m_buffer 里装的是**原始字节**（每个字节就是
    // U+0000..U+00FF 的一个 code unit），而不是 UTF-8 文本。
    //
    // 这两种载荷在 V8 眼里毫无区别（都是 one-byte 串），但**解回 exlib::string
    // 时必须区分**：exlib::string 的约定是合法 UTF-8。把 0x80..0xFF 当成 UTF-8
    // 去校验会得到一串 U+FFFD —— `Buffer.alloc(2000,0xff).toString('latin1')`
    // 再经过 path/util 之类的 native 往返就会整段损坏。见 str()。
    ExtString(v8::Isolate* _isolate, exlib::string _buffer, bool latin1 = false)
        : m_isolate(_isolate)
        , m_buffer(std::move(_buffer))
        , m_latin1(latin1)
    {
        g_ExtStringCount.inc();
        m_isolate->AdjustAmountOfExternalAllocatedMemory(m_buffer.length());
    }

    ~ExtString()
    {
        m_isolate->AdjustAmountOfExternalAllocatedMemory(-(int64_t)m_buffer.length());
        g_ExtStringCount.dec();
    }

public:
    virtual const char* data() const
    {
        return m_buffer.c_str();
    }

    virtual size_t length() const
    {
        return m_buffer.length();
    }

    exlib::string str()
    {
        if (!m_latin1)
            return m_buffer;

        // Latin-1 -> UTF-8（0x80..0xFF 展开成两字节）。这是把"原始字节"还原成
        // exlib::string 约定形式的唯一正确做法；估算长度用 simdutf 的向量化实现。
        size_t len = m_buffer.length();
        if (len == 0)
            return exlib::string();

        exlib::string out;
        out.resize(simdutf::utf8_length_from_latin1(m_buffer.c_str(), len));
        simdutf::convert_latin1_to_utf8(m_buffer.c_str(), len, out.data());
        return out;
    }

private:
    v8::Isolate* m_isolate;
    exlib::string m_buffer;
    bool m_latin1;
};

// 判断一段 UTF-8 字节是否全部落在 ASCII 范围（即"可以直接当 one-byte 字符串挂给 V8"）。
//
// 旧实现是 8 字节 SWAR：
//   const uint64_t* w = (const uint64_t*)s;   // ← 未对齐时是 UB
// 实测两个问题：
//   1) 性能：64 KB 全 ASCII 要 2.18 µs，simdutf::validate_ascii 只要 0.47 µs（4.7×）
//      （4096 字节 0.145 vs 0.029 µs，5×；1024 字节 0.044 vs 0.012 µs，3.7×）
//   2) 正确性/可移植性：把 char* 直接 reinterpret_cast 成 uint64_t* 后解引用，
//      在未对齐的指针上是 C++ 未定义行为（UBSan 实测报 "load of misaligned address"）。
//      str.c_str() 的对齐取决于 exlib::string 的内部布局，长字符串走 malloc 时通常
//      8 字节对齐、短字符串用 SSO 缓冲时不一定。
//
// simdutf::validate_ascii 是向量化实现（AVX2/AVX-512/NEON），无对齐要求，
// 语义与旧实现逐位一致（256 个单字节取值 + 10 万随机串，0 失配）。
inline bool is_safe_string(const char* s, size_t len)
{
    if (len == 0)
        return true;

    return simdutf::validate_ascii(s, len);
}

#define SMALL_STRING 1024

v8::Local<v8::String> NewString(v8::Isolate* isolate, exlib::string str)
{
    size_t length = str.length();

    // **必须用 v8::String::kMaxLength，不能用 INT_MAX**：V8 的上限是 2^29-24
    // （64 位：(1<<29)-24 = 536870888），比 INT_MAX 小得多。用 INT_MAX 判断会
    // 把 536870889..INT_MAX 之间当成"合法"，然后 NewExternalOneByte /
    // NewFromOneByte 失败返回空句柄，被 .FromMaybe() 静默吞掉 ——
    // 调用方（如 hex.encode）于是返回 undefined 而不是抛错：
    //   hex.encode(Buffer.alloc(268435450))   // 输出 536870900 字符
    //   期望 RangeError，实际得到 undefined（且 Buffer 的 512 MB 白占）。
    if (length > (size_t)v8::String::kMaxLength) {
        ThrowResult(result_t(CALL_E_OVERFLOW));
        return v8::Local<v8::String>();
    }

    if (length < SMALL_STRING)
        return NewString(isolate, str.c_str(), length);

    v8::Local<v8::String> v;

    // 走外部字符串时把缓冲**移进去**（ExtString/ExtStringW 已改为 move 构造）：
    // 旧写法按值传参会多一次全量 memcpy，40 KB 的 ASCII 这条占 20 µs 里的一半。
    if (is_safe_string(str.c_str(), length))
        v = v8::String::NewExternalOneByte(isolate, new ExtString(isolate, std::move(str))).FromMaybe(v8::Local<v8::String>());
    else
        v = v8::String::NewExternalTwoByte(isolate, new ExtStringW(isolate, utf8to16String(str))).FromMaybe(v8::Local<v8::String>());

    return v;
}

v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, ssize_t length)
{
    if (length == -1)
        length = (ssize_t)qstrlen(data);

    // 同 NewString(exlib::string)：上限是 kMaxLength，不是 INT_MAX
    if (length < 0 || (size_t)length > (size_t)v8::String::kMaxLength) {
        ThrowResult(result_t(CALL_E_OVERFLOW));
        return v8::Local<v8::String>();
    }

    if (length >= SMALL_STRING)
        return NewString(isolate, exlib::string(data, length));

    v8::Local<v8::String> v;

    if (is_safe_string(data, length))
        v = v8::String::NewFromOneByte(isolate, (const uint8_t*)data, v8::NewStringType::kNormal, (uint32_t)length).FromMaybe(v8::Local<v8::String>());
    else
        v = v8::String::NewExternalTwoByte(isolate, new ExtStringW(isolate, utf8to16String(data, length))).FromMaybe(v8::Local<v8::String>());

    return v;
}

// 从裸字节构造 **one-byte** 字符串：每个字节就是 U+0000..U+00FF 的一个 code unit。
// 这就是 Node 的 StringBytes::Encode(LATIN1) 走的路
// （node/src/string_bytes.cc: `ExternOneByteString::NewFromCopy`）。
//
// 与 NewString(isolate, exlib::string) 的区别是关键性的：后者把入参当 **UTF-8**，
// 于是 latin1 的 0x80..0xFF 会先被展开成两字节 UTF-8、再被拆回 one-byte。
// 编码路径想避免这两趟往返，就必须走这里。
v8::Local<v8::String> NewStringOneByte(v8::Isolate* isolate, exlib::string str)
{
    size_t length = str.length();

    // 上限是 v8::String::kMaxLength（一个字节 = 一个 code unit），不是 INT_MAX。
    // 超限必须抛错，不能交给 NewExternalOneByte 静默失败
    // （否则 latin1 的 toString 会返回 undefined）。
    if (length > (size_t)v8::String::kMaxLength) {
        ThrowResult(result_t(CALL_E_OVERFLOW));
        return v8::Local<v8::String>();
    }

    if (length == 0)
        return v8::String::Empty(isolate);

    if (length < SMALL_STRING)
        return v8::String::NewFromOneByte(isolate, (const uint8_t*)str.c_str(),
            v8::NewStringType::kNormal, (uint32_t)length)
            .FromMaybe(v8::Local<v8::String>());

    // `true`：这个缓冲装的是**原始字节**（Latin-1），不是 UTF-8。解回
    // exlib::string 时若按 UTF-8 处理，0x80..0xFF 会全部变成 U+FFFD。
    return v8::String::NewExternalOneByte(isolate, new ExtString(isolate, str, true))
        .FromMaybe(v8::Local<v8::String>());
}

// 从 UTF-16 code unit 构造 **two-byte** 字符串，不经过 UTF-8。
// 对应 Node 的 `ExternTwoByteString::NewFromCopy`（ucs2 分支）。
v8::Local<v8::String> NewStringTwoByte(v8::Isolate* isolate, exlib::wstring str)
{
    size_t length = str.length();

    // 同样是 kMaxLength（单位是 code unit，与 utf16 缓冲的字节数无关）
    if (length > (size_t)v8::String::kMaxLength) {
        ThrowResult(result_t(CALL_E_OVERFLOW));
        return v8::Local<v8::String>();
    }

    if (length == 0)
        return v8::String::Empty(isolate);

    if (length < SMALL_STRING)
        return v8::String::NewFromTwoByte(isolate, (const uint16_t*)str.c_str(),
            v8::NewStringType::kNormal, (uint32_t)length)
            .FromMaybe(v8::Local<v8::String>());

    return v8::String::NewExternalTwoByte(isolate, new ExtStringW(isolate, str))
        .FromMaybe(v8::Local<v8::String>());
}

exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::String> str)
{
    exlib::string n;

    if (str->IsExternalOneByte())
        return ((ExtString*)str->GetExternalOneByteStringResource())->str();
    else if (str->IsExternal())
        return ((ExtStringW*)str->GetExternalStringResource())->str();

    int32_t bufUtf8Len = str->Utf8Length(isolate);
    n.resize(bufUtf8Len);
    // **必须带 REPLACE_INVALID_UTF8**。
    //
    // 不加这个 flag 时 V8 的 WriteUtf8 会把孤立代理项写成 WTF-8（U+D800 ->
    // ED A0 80），而那**不是**合法的 UTF-8 序列。exlib::string 的约定是"合法
    // UTF-8"，下游（simdutf 解码、NewString 回挂）都按标准 UTF-8 校验，于是
    // ED A0 80 被判成 3 个独立非法字节：
    //
    //   path.normalize('a\ud800b')  ->  'a\uFFFD\uFFFD\uFFFDb'
    //
    // 一个 code unit **膨胀成三个**，长度和内容都变了；而纯 JS 的 concat /
    // toUpperCase / JSON 往返不受影响，于是同一个字符串"过不过 native"结果不同。
    //
    // 带上 flag 后 V8 直接写 1 个 U+FFFD，与 Buffer.from(s,'utf8') 以及 V8
    // TextEncoder 的口径一致，也不再放大。
    int flags = v8::String::HINT_MANY_WRITES_EXPECTED | v8::String::NO_NULL_TERMINATION
        | v8::String::REPLACE_INVALID_UTF8;

    str->WriteUtf8(isolate, n.data(), bufUtf8Len, NULL, flags);
    return n;
}

exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::Value> v)
{
    exlib::string n;
    v8::Local<v8::String> str;

    if (v->IsDate())
        str = v.As<v8::Date>()->ToISOString();
    else
        str = v->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>());
    if (str.IsEmpty())
        return n;

    return ToString(isolate, str);
}

// an object converts to a string only through a toString() of its own: the
// generic Object.prototype / Array.prototype rendering ("[object Object]",
// "1,2") is a tag rather than a conversion, and a native object carries none
// (its generic toString reports an error, see object_base::toString)
static bool has_own_toString(Isolate* isolate, v8::Local<v8::Value> v)
{
    v8::Local<v8::Object> o = v.As<v8::Object>();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> key = isolate->NewString("toString");

    v8::Local<v8::Value> fn;
    if (!o->Get(context, key).ToLocal(&fn) || !fn->IsFunction())
        return false;

    return o->HasOwnProperty(context, key).FromMaybe(false);
}

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, exlib::string& n, bool bStrict)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    v8::Local<v8::String> str;

    if (v->IsString())
        str = v.As<v8::String>();
    else if (v->IsStringObject())
        str = v.As<v8::StringObject>()->ValueOf();
    else {
        // the first pass takes real strings only. The second pass renders a
        // value through a toString() of its own: a JS object that brings one
        // (`{toString: () => "x"}`) and a native object whose class implements
        // one (a URL renders its href, a Buffer its bytes, an element its
        // markup) convert through it, which is what a string argument means in
        // JavaScript.
        //
        // everything else is not a string: primitives (1 is not "1", null is
        // not "null"), objects that only inherit the generic Object.prototype /
        // Array.prototype rendering ("[object Object]", "1,2"), and native
        // objects whose class implements no toString -- the generic
        // object_base::toString reports an error instead of turning into a
        // plausible looking "[object Name]", so those have no string form
        if (bStrict || !v->IsObject())
            return result_t::type_mismatch();

        // a read that fails inside the object -- a getter that throws -- is the
        // JavaScript error of that getter, not a type error of the argument
        v8::TryCatch trycatch(isolate->m_isolate);

        if (v->IsDate())
            str = v.As<v8::Date>()->ToISOString();
        else if (IsJSBuffer(v) || has_own_toString(isolate, v))
            // a buffer renders its bytes as utf8, which is what handing a
            // buffer to a string parameter has always meant here
            str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());
        else if (IsNativeObject(v)) {
            str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());

            if (trycatch.HasCaught() || str.IsEmpty())
                return result_t::type_mismatch();
        } else {
            if (trycatch.HasCaught()) {
                trycatch.ReThrow();

                return result_t(CALL_E_JAVASCRIPT);
            }

            return result_t::type_mismatch();
        }
    }

    if (str.IsEmpty())
        return result_t(CALL_E_JAVASCRIPT);

    n = ToString(isolate->m_isolate, str);
    return result_t();
}

result_t GetDOMStringValue(v8::Local<v8::Value> v, exlib::string& retVal)
{
    Isolate* isolate = v->IsObject() ? Isolate::current(v.As<v8::Object>()) : Isolate::current();

    // the value's own string form, like String() in JavaScript. A conversion
    // that throws is either a symbol (the engine error stands) or a native
    // object whose class implements no toString() -- the generic one reports
    // an error (object_base::toString); the DOM renders the object tag for the
    // latter, which is what Object.prototype.toString gives: String(file) is
    // "[object File]"
    v8::TryCatch trycatch(isolate->m_isolate);
    v8::Local<v8::String> str = v->ToString(isolate->context()).FromMaybe(v8::Local<v8::String>());

    if (trycatch.HasCaught()) {
        object_base* o = object_base::getInstance(v);

        if (!o) {
            // the conversion of a value that is not a native object threw (a
            // symbol, or a toString() of its own): the engine error stands
            trycatch.ReThrow();

            return result_t(CALL_E_JAVASCRIPT);
        }

        retVal = "[object ";
        retVal.append(o->Classinfo().name());
        retVal.append("]");
        return result_t();
    }

    if (str.IsEmpty())
        return result_t(CALL_E_JAVASCRIPT);

    retVal = ToString(isolate->m_isolate, str);
    return result_t();
}

} // namespace fibjs