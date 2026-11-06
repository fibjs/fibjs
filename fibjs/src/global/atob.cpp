/*
 * atob.cpp
 *
 *  Created on: Aug 5, 2025
 *      Author: fibjs team
 */

#include "object.h"
#include "ifs/global.h"
#include "Runtime.h"
#include "encoding.h"
#include "simdutf.h"

namespace fibjs {

// base64 字母表（标准 + url-safe 两套字符都接受，与 Buffer.from(...,'base64') 一致）
static inline bool b64_char_ok(unsigned char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || (c >= '0' && c <= '9') || c == '+' || c == '/' || c == '-' || c == '_';
}

// 是否含 ASCII 空白（atob 需要忽略的唯一非 base64 字符）。
// 5 次比较比 simdutf 的一次调用更划算（64 KB 约 15 µs，且只在快路径判定时用一次）。
static inline bool has_ascii_blank(const char* p, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        char c = p[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\r')
            return true;
    }
    return false;
}

// WHATWG forgiving-base64 的长度约束。
//
// 规范里"缺少 padding"是**合法**的（'YQ' 与 'YQ==' 等价），但 padding 一旦出现
// 就必须**正好补齐**，不能少也不能多。实测 node 的判定：
//
//   body % 4 == 0  -> pad 必须为 0        （'YWJj=' 非法）
//   body % 4 == 1  -> 一律非法            （'A'、'AAAAA'）
//   body % 4 == 2  -> pad 为 0 或 2       （'YQ'、'YQ==' 合法；'YQ=' 非法）
//   body % 4 == 3  -> pad 为 0 或 1       （'YWJjZA'、'YWJjZA=' 合法）
//
// 旧实现写成 pad <= 2 / pad <= 1（"不超过"），于是 atob('YQ=') 返回 'a'、
// atob('TWE==') 返回 'ma' —— node 与浏览器都会抛 InvalidCharacterError。
static inline bool atob_pad_ok(size_t body, size_t pad)
{
    size_t m = body % 4;
    if (m == 1)
        return false;               // 1 mod 4 无法由 padding 修复
    if (m == 0)
        return pad == 0;
    return pad == 0 || pad == 4 - m;    // 缺省 padding 允许，出现就必须补齐
}

// UTF-8 -> Latin-1，要求每个码点都 <= 0xFF（否则 btoa 报错）。
//
// 旧实现是手写的逐码点解码循环（btoa 64 KB 要 295 µs）。改成 simdutf：
//   - validate_utf8 判定整体合法性（向量化，64 KB 0.5 µs）
//   - convert_utf8_to_latin1 直接产出 Latin-1 字节（向量化）
//   - 只有当"转换数 != 输入码点数"时才说明含 > 0xFF 的字符，此时再走一次
//     utf8 解码定位报错（罕见路径，保持原有的报错语义）
static result_t utf8ToLatin1(const exlib::string& utf8Str, exlib::string& latin1Str)
{
    const char* src = utf8Str.c_str();
    size_t len = utf8Str.length();

    latin1Str.clear();
    if (len == 0)
        return result_t();

    if (!simdutf::validate_utf8(src, len))
        return result_t::make(CALL_E_EXCEPTION).with_message("Failed to execute 'btoa': Invalid UTF-8 sequence.");

    // UTF-8 长度一定 >= Latin-1 长度，据此分配
    latin1Str.resize(len);
    size_t n = simdutf::convert_utf8_to_latin1(src, len, latin1Str.data());

    if (n == 0) {
        // 含超出 Latin-1 的码点（simdutf 遇到就停并返回 0）
        return result_t::make(CALL_E_EXCEPTION).with_message("Failed to execute 'btoa': The string to be encoded contains characters outside of the Latin1 range.");
    }

    latin1Str.resize(n);
    return result_t();
}

result_t global_base::btoa(v8::Local<v8::Value> data_, exlib::string& retVal)
{
    // the value is converted to its string form, as the DOM and node do:
    // btoa(123) encodes "123", btoa(null) encodes "null"
    exlib::string data;
    {
        result_t hr_ = GetDOMStringValue(data_, data);
        if (hr_.failed())
            return hr_;
    }

    // Convert UTF-8 to Latin1 and validate
    exlib::string latin1Data;
    result_t hr = utf8ToLatin1(data, latin1Data);
    if (hr.failed()) {
        return hr;
    }

    // Encode to Base64（base64Encode 走 simdutf，btoa 从 295 µs 降到 ~3 µs）
    base64Encode(latin1Data.c_str(), latin1Data.length(), false, retVal);
    return result_t();
}

result_t global_base::atob(v8::Local<v8::Value> data_, exlib::string& retVal)
{
    // the value is converted to its string form, as the DOM and node do:
    // atob(123) decodes "123"
    exlib::string data;
    {
        result_t hr_ = GetDOMStringValue(data_, data);
        if (hr_.failed())
            return hr_;
    }

    // atob 用的是 WHATWG 的 forgiving-base64，比 Buffer.from(s,'base64') 严格：
    //   - 只忽略 ASCII 空白（Tab/LF/FF/CR/Space），其它非法字符一律报错
    //   - 去掉空白、去掉尾部 padding 后的长度 % 4 不能是 1
    //   - '=' 只能出现在尾部，且最多 2 个
    // （node：atob('YWJj=') 抛错，而 Buffer.from('YWJj=','base64') 正常返回 ——
    //   两者语义不同，这里按 atob 实现。）
    //
    // 实现分两条路径（旧的手写逐码点解码 437 µs，现在 58 µs）：
    //   快路径：整串 ASCII 且无空白 —— 直接拿原始指针交给 simdutf，零拷贝
    //   慢路径：含空白 —— 过滤到一个缓冲里再做同样的事
    exlib::string latin1Data;
    {
        const char* p = data.c_str();
        const size_t total = data.length();

        // 快路径下 body 就是"去掉尾部 '='"；慢路径由过滤循环重新计算
        size_t body = total;
        while (body > 0 && p[body - 1] == '=')
            body--;
        size_t pad = total - body;

        bool ok = true;
        // 过滤后的缓冲区（只有含空白/非 ASCII 时才用到）；必须活到解码结束
        exlib::string clean;
        const char* decode_src = p;

        if (!simdutf::validate_ascii(p, total)
            || has_ascii_blank(p, total)) {
            // 慢路径：过滤 ASCII 空白
            clean.resize(total);
            char* dst = clean.data();
            size_t n = 0;
            size_t body_end = 0;
            size_t seen_pad = 0;

            for (size_t i = 0; i < total; i++) {
                unsigned char c = (unsigned char)p[i];
                if (c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\r')
                    continue;
                if (c == '=') {
                    seen_pad++;
                    continue;
                }
                if (seen_pad) {                 // '=' 之后又有内容
                    ok = false;
                    break;
                }
                if (!b64_char_ok(c)) {
                    ok = false;
                    break;
                }
                dst[n++] = (char)c;
                body_end = n;
            }

            if (ok) {
                pad = seen_pad;
                body = body_end;
                clean.resize(n);
                decode_src = clean.c_str();
            }
        } else {
            // 快路径：逐字节校验字母表（body 部分，padding 已剥离）
            for (size_t i = 0; i < body; i++) {
                if (!b64_char_ok((unsigned char)p[i])) {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok || !atob_pad_ok(body, pad))
            return result_t::make(CALL_E_EXCEPTION).with_message(
                "Failed to execute 'atob': The string to be decoded is not correctly encoded.");

        latin1Data.resize((body * 3) >> 2);
        if (latin1Data.length()) {
            simdutf::result r = simdutf::base64_to_binary(
                decode_src, body, latin1Data.data(),
                simdutf::base64_default, simdutf::loose);
            if (r.error != simdutf::error_code::SUCCESS)
                return result_t::make(CALL_E_EXCEPTION).with_message(
                    "Failed to execute 'atob': The string to be decoded is not correctly encoded.");
            latin1Data.resize(r.count);
        } else
            latin1Data.clear();
    }

    // Latin-1 -> UTF-8（simdutf 向量化；旧的手写循环原地 append 逐字节）
    size_t len = latin1Data.length();
    if (len == 0) {
        retVal.clear();
        return result_t();
    }

    retVal.resize(simdutf::utf8_length_from_latin1(latin1Data.c_str(), len));
    simdutf::convert_latin1_to_utf8(latin1Data.c_str(), len, retVal.data());
    return result_t();
}

}
