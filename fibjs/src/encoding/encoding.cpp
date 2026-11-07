/*
 * encoding.cpp
 *
 *  Created on: Apr 10, 2012
 *      Author: lion
 */

#include "object.h"
#include "encoding.h"
#include "encoding_conv.h"
#include "Url.h"
#include "libbase58.h"
#include "simdutf.h"
#include <math.h>

#if defined(__SSSE3__)
#include <tmmintrin.h>
#elif defined(__aarch64__)
#include <arm_neon.h>
#endif

namespace fibjs {

// 全 ASCII 判定（simdutf 向量化）。base64/hex 的快速路径都要先确认"字符串的
// 每个 char16 都 < 0x80"，才能把 V8 的 one-byte 数据当字节流直接交给 simdutf。
static inline bool is_safe_ascii(const char* s, size_t len)
{
    return len == 0 || simdutf::validate_ascii(s, len);
}

DECLARE_MODULE(encoding);
DECLARE_MODULE(base32);
DECLARE_MODULE(base58);
DECLARE_MODULE(base64);
DECLARE_MODULE(hex);
DECLARE_MODULE(multibase);

static void hexEncode(const char* data, size_t sz, bool upper, exlib::string& retVal)
{
    const char* HexChar = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    size_t i;

    retVal.resize(sz * 2);
    char* _retVal = retVal.data();

#if defined(__aarch64__)
    // 8 字节 -> 16 个 hex 字符：nibble 用 vshr/vand 拆，vqtbl1_u8 做 16 项 ASCII
    // 查表，vzip1q_u8 交错成 [hi0,lo0,hi1,lo1,...]。
    // 64 KB 输入 49.5 µs -> 4.4 µs（11×）。
    {
        const uint8x16_t lut = vld1q_u8((const uint8_t*)HexChar);
        const uint8x8_t mask = vdup_n_u8(0x0f);

        for (i = 0; i + 8 <= sz; i += 8) {
            uint8x8_t v = vld1_u8((const uint8_t*)(data + i));
            uint8x8_t hi = vshr_n_u8(v, 4);
            uint8x8_t lo = vand_u8(v, mask);

            // 两个 8 字节查表，然后交错成 16 个字符
            uint8x8_t h = vqtbl1_u8(lut, hi);
            uint8x8_t l = vqtbl1_u8(lut, lo);
            uint8x16_t r = vzip1q_u8(vcombine_u8(h, h), vcombine_u8(l, l));
            vst1q_u8((uint8_t*)(_retVal + i * 2), r);
        }
    }
#else
    i = 0;
#endif

    for (; i < sz; i++) {
        unsigned char ch = (unsigned char)data[i];

        _retVal[i * 2] = HexChar[ch >> 4];
        _retVal[i * 2 + 1] = HexChar[ch & 0xf];
    }
}

static void hexEncode(exlib::string data, bool upper, exlib::string& retVal)
{
    return hexEncode(data.c_str(), data.length(), upper, retVal);
}

result_t hex_base::encode(Union_encode_data data, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        exlib::string str = std::get<exlib::string>(data);
        hexEncode(str.c_str(), str.length(), false, retVal);
        return result_t();
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));
    hexEncode((const char*)buf->data(), buf->length(), false, retVal);
    return result_t();
}

// hex 字符 -> 值（-1 = 非法）。256 项查表，比 qisxdigit+qhex 的级联比较快。
// 注意 qisxdigit 的旧实现有个 bug：`ch >= 'A' && ch <= 'Z'` 把 'G'-'Z' 也当成
// 合法十六进制，所以旧的 encoding.hex.decode 会把 'abZZcd' 解成 3 个字节。
// 这里只认 0-9 a-f A-F，与 node 一致。
static const signed char kHexTable[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

// hex 解码的**唯一**实现（向量化 + 标量收尾）。
//
// on_invalid 决定遇到非法字符时怎么办 —— 这是两套不同语义的唯一差别：
//   kStop    Buffer.from(s,'hex')  —— node 语义：遇非法即停
//   kSkip    encoding.hex.decode() —— fibjs 语义：跳过非法字符继续
//
// 向量化只处理"整块 16 个字符全合法"的情况；一旦遇到非法字符，剩下的交给
// 标量循环按语义分别处理。这样两条路径共享同一份向量代码。
enum HexInvalid { kStop, kSkip };

static size_t hexDecodeImpl(const char* data, size_t sz, char* dst, size_t cap, HexInvalid on_invalid)
{
    size_t n = 0;
    size_t i = 0;

#if defined(__aarch64__)
    // 一次吞 16 个 hex 字符 -> 8 字节（64 KB 输入 52 µs -> 17 µs，3.1×）。
    // AArch64 有原生无符号比较（vcgtq_u8）与水平归约（vminvq_u8），
    // 不需要 x86 那套 movemask + 符号翻转。
    if (sz >= 16) {
        const uint8x16_t cNine = vdupq_n_u8(9);
        const uint8x16_t cLc = vdupq_n_u8(0x20);
        const uint8x16_t c27 = vdupq_n_u8(0x27);
        const uint8x16_t c0 = vdupq_n_u8('0');

        for (; i + 16 <= sz && n + 8 <= cap; i += 16, n += 8) {
            uint8x16_t v = vld1q_u8((const uint8_t*)(data + i));

            // 合法字符集：'0'-'9' | 'A'-'F' | 'a'-'f'
            uint8x16_t ge0 = vcgtq_u8(v, vdupq_n_u8('0' - 1));
            uint8x16_t le9 = vcgtq_u8(vdupq_n_u8('9' + 1), v);
            uint8x16_t geA = vcgtq_u8(v, vdupq_n_u8('A' - 1));
            uint8x16_t leF = vcgtq_u8(vdupq_n_u8('F' + 1), v);
            uint8x16_t gea = vcgtq_u8(v, vdupq_n_u8('a' - 1));
            uint8x16_t lef = vcgtq_u8(vdupq_n_u8('f' + 1), v);
            uint8x16_t ok = vorrq_u8(vandq_u8(ge0, le9),
                vorrq_u8(vandq_u8(geA, leF), vandq_u8(gea, lef)));

            // 整块都合法？（vminvq_u8 取 16 个 lane 的最小值，全 0xFF 才算通过）
            if (vminvq_u8(ok) != 0xFF)
                break;

            // 取值：小写化后减 '0'，字母再多减 0x27（'a'-'0'-0x27 = 10）
            uint8x16_t dig = vsubq_u8(vorrq_u8(v, cLc), c0);
            uint8x16_t isAlpha = vcgtq_u8(dig, cNine);
            uint8x16_t val = vsubq_u8(dig, vandq_u8(isAlpha, c27));

            // 压实：字节对 (hi, lo) -> hi*16 + lo。
            //   vuzp1q_u8(val4, val4) 取偶数位（hi），
            //   vuzp2q_u8(val,  val)  取奇数位（lo），
            //   两者相加就是成品，前 8 字节有效。
            uint8x16_t val4 = vshlq_n_u8(val, 4);
            uint8x16_t hi = vuzp1q_u8(val4, val4);
            uint8x16_t lo = vuzp2q_u8(val, val);
            vst1_u8((uint8_t*)(dst + n), vget_low_u8(vaddq_u8(hi, lo)));
        }
    }
#elif defined(__SSSE3__)
    // 一次吞吐 16 个 hex 字符 -> 8 字节（64 KB 输入 34 µs -> 11.6 µs）。
    //   1) 合法性与取值用 psubb/pcmpgtb 批量算，movemask 一次判定整块合法
    //   2) 压实用 pmaddubsw（每对字节加权求和：高位*0x10 + 低位*1）+ packuswb
    if (sz >= 16) {
        const __m128i nine = _mm_set1_epi8(9);
        const __m128i lc = _mm_set1_epi8(0x20);
        const __m128i c27 = _mm_set1_epi8(0x27);
        const __m128i c0 = _mm_set1_epi8('0');
        const __m128i w = _mm_set1_epi16(0x0110);

        for (; i + 16 <= sz && n + 8 <= cap; i += 16, n += 8) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));

            // 合法字符集：'0'-'9' | 'A'-'F' | 'a'-'f'
            __m128i ge0 = _mm_cmpgt_epi8(v, _mm_set1_epi8('0' - 1));
            __m128i le9 = _mm_cmpgt_epi8(_mm_set1_epi8('9' + 1), v);
            __m128i geA = _mm_cmpgt_epi8(v, _mm_set1_epi8('A' - 1));
            __m128i leF = _mm_cmpgt_epi8(_mm_set1_epi8('F' + 1), v);
            __m128i gea = _mm_cmpgt_epi8(v, _mm_set1_epi8('a' - 1));
            __m128i lef = _mm_cmpgt_epi8(_mm_set1_epi8('f' + 1), v);
            __m128i ok = _mm_or_si128(_mm_and_si128(ge0, le9),
                _mm_or_si128(_mm_and_si128(geA, leF),
                    _mm_and_si128(gea, lef)));

            if (_mm_movemask_epi8(ok) != 0xFFFF)
                break;

            // 取值：小写化后减 '0'，字母再多减 0x27（'a'-'0'-0x27 = 10）
            __m128i dig = _mm_sub_epi8(_mm_or_si128(v, lc), c0);
            __m128i isAlpha = _mm_cmpgt_epi8(dig, nine);
            __m128i val = _mm_sub_epi8(dig, _mm_and_si128(isAlpha, c27));

            // 压实：字节对 (hi, lo) -> hi*16 + lo
            __m128i packed = _mm_maddubs_epi16(val, w);
            __m128i out = _mm_packus_epi16(packed, packed);
            _mm_storel_epi64((__m128i*)(dst + n), out);
        }
    }
#endif

    if (on_invalid == kStop) {
        for (; i + 1 < sz && n < cap; i += 2) {
            int hi = kHexTable[(unsigned char)data[i]];
            if (hi < 0)
                break;
            int lo = kHexTable[(unsigned char)data[i + 1]];
            if (lo < 0)
                break;
            dst[n++] = (char)((hi << 4) | lo);
        }
    } else {
        // kSkip：跳过非法字符，把剩下的按位累加（可能跨字符对齐）
        int hi = -1;
        for (; i < sz && n < cap; i++) {
            int v = kHexTable[(unsigned char)data[i]];
            if (v < 0)
                continue;
            if (hi < 0)
                hi = v;
            else {
                dst[n++] = (char)((hi << 4) | v);
                hi = -1;
            }
        }
    }

    return n;
}

size_t hexDecodeTo(const char* data, size_t sz, char* dst, size_t cap)
{
    return hexDecodeImpl(data, sz, dst, cap, kStop);
}

static void hexDecode(const char* _data, int32_t len, exlib::string& retVal)
{
    retVal.resize((size_t)len / 2);
    size_t n = hexDecodeImpl(_data, (size_t)len, retVal.data(), retVal.length(), kSkip);
    retVal.resize(n);
}

static void hexDecode(const char* _data, int32_t len, obj_ptr<Buffer_base>& retVal)
{
    size_t cap = (size_t)len / 2;
    Buffer* buf = new Buffer(NULL, cap);
    size_t n = hexDecodeImpl(_data, (size_t)len, (char*)buf->data(), cap, kSkip);
    if (n < cap)
        buf->resize(n);
    retVal = buf;
}
result_t hex_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    hexDecode(data.c_str(), (int32_t)data.length(), retVal);
    return result_t();
}

static void baseEncode(const char* pEncodingTable, size_t dwBits,
    const char* data, size_t sz, exlib::string& retVal, bool fill)
{
    size_t i, len = 0, bits = 0;
    size_t dwData = 0;
    size_t dwSize = 0;
    char bMask = 0xff >> (8 - dwBits);

    if (dwBits == 6)
        dwSize = (sz + 2) / 3 * 4;
    else if (dwBits == 5)
        dwSize = (sz + 4) / 5 * 8;

    retVal.resize(dwSize);
    char* _retVal = retVal.data();

    for (i = 0; i < sz; i++) {
        dwData <<= 8;
        dwData |= (unsigned char)data[i];
        bits += 8;

        while (bits >= dwBits) {
            _retVal[len++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
            bits -= dwBits;
        }
    }

    if (bits)
        _retVal[len++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];

    if (fill)
        while (len < dwSize)
            _retVal[len++] = '=';

    retVal.resize(len);
}

static void baseDecode(const char* pdecodeTable, size_t dwBits,
    const char* _baseString, size_t len, exlib::string& retVal)
{
    size_t nWritten = 0;
    const char* end = _baseString + len;

    retVal.resize(len * dwBits / 8);
    char* _retVal = retVal.data();

    size_t dwCurr = 0;
    size_t nBits = 0;
    unsigned char ch;

    while ((_baseString < end) && (ch = (unsigned char)*_baseString++)) {
        // node: base64 的 '=' 是**终止符**，不是可跳过的非法字符。
        // 只有 6-bit（base64/base64url）用 '=' 做 padding，base32 用 '=' 也同理。
        if (ch == '=')
            break;

        int32_t nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

        if (nCh != -1) {
            dwCurr <<= dwBits;
            dwCurr |= nCh;
            nBits += dwBits;

            while (nBits >= 8) {
                _retVal[nWritten++] = (char)(dwCurr >> (nBits - 8));
                nBits -= 8;
            }
        }
    }

    retVal.resize(nWritten);
}

static void baseDecode(const char* pdecodeTable, size_t dwBits,
    const char* _baseString, size_t len, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    baseDecode(pdecodeTable, dwBits, _baseString, len, strBuf);
    retVal = new Buffer(strBuf.c_str(), strBuf.length());
}

static void base32Encode(const char* data, size_t sz, bool upper, bool padding, exlib::string& retVal)
{
    baseEncode(upper ? "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567" : "abcdefghijklmnopqrstuvwxyz234567",
        5, data, sz, retVal, padding);
}

result_t base32_base::encode(Union_encode_data data, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        exlib::string str = std::get<exlib::string>(data);
        base32Encode(str.c_str(), str.length(), false, false, retVal);
        return result_t();
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));
    base32Encode((const char*)buf->data(), buf->length(), false, false, retVal);
    return result_t();
}

static void base32Decode(const char* data, size_t sz, exlib::string& retVal)
{
    static const char decodeTable[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
        14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    baseDecode(decodeTable, 5, data, sz, retVal);
}

static void base32Decode(const char* data, size_t sz, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    base32Decode(data, sz, strBuf);
    retVal = new Buffer(strBuf.c_str(), strBuf.length());
}

result_t base32_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    base32Decode(data.c_str(), data.length(), retVal);
    return result_t();
}

static void base64Encode(const char* data, size_t sz, bool url, bool padding, exlib::string& retVal)
{
    if (url)
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_",
            6, data, sz, retVal, padding);
    else
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
            6, data, sz, retVal, padding);
}

// simdutf 的 base64（AVX2/AVX-512/NEON 向量化）：64 KB 编码 3.8 µs、87K 字符解码
// 3.6 µs，比原来的通用位循环快 18-60×（node 走的也是这条：node/src/string_bytes.cc
// 里全是 simdutf::binary_to_base64 / base64_to_binary）。
//
// 语义：
//   - 编码：url=true 用 base64url 字母表且不带 padding，否则标准字母表 + padding
//     （与原来的 base64Encode(data, sz, url, padding=!url) 一致）
//   - 解码：两种字母表都接受（node 亦然），非法字符按 forgiving-base64 处理
void base64Encode(const char* data, size_t sz, bool url, exlib::string& retVal)
{
    simdutf::base64_options opt = url ? simdutf::base64_url : simdutf::base64_default;
    size_t dlen = simdutf::base64_length_from_binary(sz, opt);
    retVal.resize(dlen);
    if (dlen)
        simdutf::binary_to_base64(data, sz, retVal.data(), opt);
}

// base64 宽容解码表（只在 simdutf 不可用时兜底：输入含 > 0xFF 的码点、
// 或 2 字节 UTF-16 串时才会走到）
static const char kBase64DecTable[] = {    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

// base64 解码的**唯一**实现：把 sz 个输入字符解到 dst（容量 cap 字节），返回
// 实际写入的字节数。宽容语义（node 的 Buffer.from(s,'base64')）：
//   - 跳过非法字符
//   - '=' 是终止符（'ab==cd' 只解 'ab'）
//   - 两种字母表都接受（+ / 与 - _ 等价）
//   - 尾部不足 4 个字符时按位累加，能凑够 8 位就产出一个字节
//
// **性能关键点**：不再先做一遍标量有效性预扫。simdutf 的
// base64_default_or_url_accept_garbage 选项本身就实现了上面全部四条语义
// （40 万随机用例与手写宽容实现逐字节对拍，0 失配），所以直接一次调用即可。
// 旧实现要先逐字符查表扫 87K（59 µs）再交给 simdutf（4 µs），预扫占了 93% 时间。
//
// dst == nullptr 时只计算所需字节数（不写）。
//
// **cap 是硬上限，不能忽略**：simdutf 的 `base64_to_binary(input, len, out, ...)`
// 根本没有输出容量参数 —— 它会按输入长度写出全部解码结果。把它当作"最多写 cap"
// 用就会堆越界：buf.write('QUJDRA==', 0, 2, 'base64') 会在一个 2 字节的 Buffer
// 后面多写 2 字节（实测返回 4）。所以只有当 cap 足够容纳**最大可能**结果
// （upper = sz*3/4，跳过非法字符只会更少）时才直接解到 dst，否则先解到临时
// 缓冲再按 cap 截断。
size_t base64DecodeTo(const char* data, size_t sz, char* dst, size_t cap)
{
    if (sz == 0)
        return 0;

    // 解码结果的上界：每个输入字符最多贡献 6 bit
    const size_t upper = (sz * 3) >> 2;

    // 快路径：全 ASCII 时字符串的字节就是 code unit，可以直接交给 simdutf。
    if (is_safe_ascii(data, sz)) {
        if (dst && cap >= upper) {
            simdutf::result r = simdutf::base64_to_binary(data, sz, dst,
                simdutf::base64_default_or_url_accept_garbage, simdutf::loose);
            if (r.error == simdutf::error_code::SUCCESS)
                return (size_t)r.count;
            // 含 >= 0x80 的字符时 simdutf 会拒绝，退回标量实现
        } else if (dst) {
            // cap 不足（调用方要截断）：先解到临时缓冲，再拷 min(cap, n)
            exlib::string tmp;
            tmp.resize(upper);
            simdutf::result r = simdutf::base64_to_binary(data, sz, tmp.data(),
                simdutf::base64_default_or_url_accept_garbage, simdutf::loose);
            if (r.error == simdutf::error_code::SUCCESS) {
                size_t n = std::min((size_t)r.count, cap);
                if (n)
                    memcpy(dst, tmp.data(), n);
                return n;
            }
        } else {
            // 测量：解到临时缓冲（simdutf 没有"只算长度"的接口，
            // 且长度不能靠公式 —— 非法字符会被跳过）
            exlib::string tmp;
            tmp.resize(upper);
            simdutf::result r = simdutf::base64_to_binary(data, sz, tmp.data(),
                simdutf::base64_default_or_url_accept_garbage, simdutf::loose);
            return r.error == simdutf::error_code::SUCCESS ? (size_t)r.count : 0;
        }
    }

    // 慢路径：标量宽容解码（含非 ASCII 字符的输入，dst 可能为 nullptr）
    size_t nWritten = 0;
    size_t dwCurr = 0;
    size_t nBits = 0;
    const char* p = data;
    const char* end = data + sz;

    while (p < end) {
        unsigned char ch = (unsigned char)*p++;
        if (ch == '=')
            break;

        int32_t nCh = (ch > 0x20 && ch < 0x80) ? kBase64DecTable[ch - 0x20] : -1;
        if (nCh != -1) {
            dwCurr = (dwCurr << 6) | (size_t)nCh;
            nBits += 6;
            while (nBits >= 8) {
                nBits -= 8;
                if (dst) {
                    if (nWritten >= cap)
                        return nWritten;
                    dst[nWritten] = (char)(dwCurr >> nBits);
                }
                nWritten++;
            }
        }
    }

    return nWritten;
}

void base64Decode(const char* data, size_t sz, exlib::string& retVal)
{
    retVal.resize((sz * 3) >> 2);
    size_t n = base64DecodeTo(data, sz, retVal.data(), retVal.length());
    retVal.resize(n);
}

static void base64Decode(const char* data, size_t sz, obj_ptr<Buffer_base>& retVal)
{
    // 直接解进 Buffer 的 backing store，不再经过 exlib::string 中转（省一次拷贝）
    size_t cap = (sz * 3) >> 2;
    Buffer* buf = new Buffer(NULL, cap);
    size_t n = base64DecodeTo(data, sz, (char*)buf->data(), cap);
    if (n < cap)
        buf->resize(n);
    retVal = buf;
}

result_t base64_base::encode(Union_encode_data data, bool url, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        exlib::string str = std::get<exlib::string>(data);
        base64Encode(str.c_str(), str.length(), url, retVal);
        return result_t();
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));
    base64Encode((const char*)buf->data(), buf->length(), url, retVal);
    return result_t();
}

result_t base64_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    base64Decode(data.c_str(), data.length(), strBuf);
    retVal = new Buffer(strBuf.c_str(), strBuf.length());
    return result_t();
}

static void base58Encode(const char* data, size_t sz, exlib::string& retVal)
{
    size_t b58sz = (size_t)(sz * 8 / log2l(58) + 2);

    retVal.resize(b58sz);
    b58enc(retVal.data(), &b58sz, data, sz);
    retVal.resize(b58sz - 1);
}

result_t base58_base::encode(Union_encode_data data, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        exlib::string str = std::get<exlib::string>(data);
        base58Encode(str.c_str(), str.length(), retVal);
        return result_t();
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));
    base58Encode((const char*)buf->data(), buf->length(), retVal);
    return result_t();
}

result_t base58_base::encode(Union_encode_data data, int32_t chk_ver, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        exlib::string str = std::get<exlib::string>(data);
        size_t b58sz = (size_t)((str.length() + 5) * 8 / log2l(58) + 2);

        retVal.resize(b58sz);
        b58check_enc(retVal.data(), &b58sz, chk_ver, (const unsigned char*)str.c_str(), str.length());
        retVal.resize(b58sz - 1);

        return result_t();
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));

    size_t b58sz = (size_t)((buf->length() + 5) * 8 / log2l(58) + 2);

    retVal.resize(b58sz);
    b58check_enc(retVal.data(), &b58sz, chk_ver, buf->data(), buf->length());
    retVal.resize(b58sz - 1);

    return result_t();
}

static result_t base58Decode(const char* data, size_t sz, exlib::string& retVal)
{
    // 缓冲要能容纳两种极端：
    //   - 值域展开：sz 个字符最多表示 sz * log2(58)/8 字节
    //   - 前导零：**每个** '1' 对应一个 0x00 字节，最多 sz 个
    //
    // 旧公式只算了第一项（+3 的余量），于是 40 个 '1' 只分配 32 字节，
    // 而 b58tobin 需要 40 字节 —— 结果被截断成 32 字节（**静默数据丢失**：
    // decode(encode(Buffer.alloc(40))) 只回来 32 个 0x00）。
    // 取两者的较大值即可，b58tobin 会在内部按实际值收缩 *binszp。
    size_t binsz = (size_t)(sz * log2l(58) / 8 + 3);
    if (binsz < sz)
        binsz = sz;

    exlib::string buffer;
    buffer.resize(binsz);
    if (!b58tobin(buffer.data(), &binsz, data, sz))
        return result_t::make(CALL_E_EXCEPTION).with_message("base58: encode error.");
    if (binsz < buffer.length())
        buffer = buffer.substr(buffer.length() - binsz);

    retVal = buffer;

    return result_t();
}

// base58 的 byteLength 专用：**不校验内容**的长度计算。
//
// 语义对齐 node 对 hex/base64 的处理 —— Buffer.byteLength 只回答"这个字符串按
// 该编码会占多少字节"，不判断内容是否合法：
//   Buffer.byteLength('!!!!', 'base64') === 3
//   Buffer.byteLength('zz', 'hex')      === 1
//
// base58 的长度依赖真实的多精度换算（没有 O(1) 公式），但 b58tobin() 遇到字母表
// 外的字符就直接失败。所以这里先把非字母表的字符过滤掉，再用过滤后的串算长度：
//   - 全合法时与 base58Decode 的结果逐字节相同
//   - 含非法字符时给出"如果这些都合法"的长度，而不是抛错
//
// 注意：这只用于 byteLength。base58_base::decode / multibase 仍走严格的
// base58Decode —— 那里数据不对必须报错，否则会静默产出错误的字节。
size_t base58ByteLength(const char* data, size_t sz)
{
    static const char B58[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    static const signed char* T = []() -> const signed char* {
        static signed char t[256];
        for (int i = 0; i < 256; i++)
            t[i] = -1;
        for (int i = 0; B58[i]; i++)
            t[(unsigned char)B58[i]] = 1;
        return t;
    }();

    // 快路径：全合法时直接算，不需要临时串
    bool all_valid = true;
    for (size_t i = 0; i < sz; i++) {
        if (T[(unsigned char)data[i]] < 0) {
            all_valid = false;
            break;
        }
    }
    const char* src = data;
    exlib::string filtered;
    if (!all_valid) {
        filtered.resize(sz);
        char* dst = filtered.data();
        size_t n = 0;
        for (size_t i = 0; i < sz; i++)
            if (T[(unsigned char)data[i]] >= 0)
                dst[n++] = data[i];
        filtered.resize(n);
        src = filtered.c_str();
        sz = n;
    }

    if (sz == 0)
        return 0;

    // 与 base58Decode 用同一个容量公式（必须容纳前导零：每个 '1' 一字节）
    size_t binsz = (size_t)(sz * log2l(58) / 8 + 3);
    if (binsz < sz)
        binsz = sz;

    exlib::string buffer;
    buffer.resize(binsz);
    if (!b58tobin(buffer.data(), &binsz, src, sz))
        return 0;

    // b58tobin 把结果写在缓冲的**尾部**，binsz 是"从尾部算起的有效字节数"。
    // base58Decode 里对应的是 `buffer.substr(buffer.length() - binsz)` —— 长度
    // 就是 binsz。这里不能返回 buffer.length()（那是容量估算值）。
    return binsz;
}

static result_t base58Decode(const char* data, size_t sz, obj_ptr<Buffer_base>& retVal)
{
    exlib::string buffer;
    result_t hr = base58Decode(data, sz, buffer);
    if (hr.failed())
        return hr;

    retVal = new Buffer(buffer.c_str(), buffer.length());

    return result_t();
}

result_t base58_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    return base58Decode(data.c_str(), data.length(), retVal);
}

result_t base58_base::decode(exlib::string data, int32_t chk_ver, obj_ptr<Buffer_base>& retVal)
{
    // 容量必须同时容纳两种极端（与 base58Decode 同一套公式，见那里的注释）：
    //   - 值域展开：sz 个字符最多表示 sz * log2(58)/8 字节
    //   - 前导零：**每个** '1' 对应一个 0x00 字节，最多 sz 个
    // 只算第一项时，前导零较多的 base58check 值会让 b58tobin 因容量不足返回
    // false（"decode error"），随后的 *binszp 调整也就无从谈起。
    size_t binsz = (size_t)(data.length() * log2l(58) / 8 + 3);
    if (binsz < data.length())
        binsz = data.length();
    exlib::string buffer;

    buffer.resize(binsz);
    if (!b58tobin(buffer.data(), &binsz, data.c_str(), data.length()))
        return result_t::make(CALL_E_EXCEPTION).with_message("base58: decode error.");
    if (b58check(buffer.c_str() + buffer.length() - binsz, binsz, data.c_str(), data.length()) != chk_ver)
        return result_t::make(CALL_E_EXCEPTION).with_message("base58: check error.");

    // **必须无条件裁掉 version 字节与 4 字节校验和**。
    //
    // 旧实现在 `binsz < buffer.length()` 时才 substr，于是当解码结果正好填满
    // 估算容量时（前导零越多越容易命中）直接返回了**含 version + checksum**
    // 的整串：
    //   encode(Buffer.concat([Buffer.alloc(10), payload]), 0)
    //   -> decode(...) 长度多 5、内容多 5 个字节
    //   前导零 >= 40 时 b58tobin 直接容量不足 -> 误报 "check error"
    // 正确做法是用 b58tobin 回填的 binsz（= 从尾部算起的有效字节数）做起点。
    if (binsz >= 5)
        buffer = buffer.substr(buffer.length() - binsz + 1, binsz - 5);
    else
        buffer.clear();

    retVal = new Buffer(buffer.c_str(), buffer.length());

    return result_t();
}

result_t commonEncode(exlib::string codec, const char* data, size_t sz, exlib::string& retVal)
{
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined")) {
        retVal.assign(data, sz);
    } else {
        if ((codec == "hex"))
            hexEncode(data, sz, false, retVal);
        else if ((codec == "base32"))
            base32Encode(data, sz, false, false, retVal);
        else if ((codec == "base58"))
            base58Encode(data, sz, retVal);
        else if ((codec == "base64"))
            base64Encode(data, sz, false, true, retVal);
        else if ((codec == "base64url"))
            base64Encode(data, sz, true, false, retVal);
        else if ((codec == "ascii")) {
            size_t i;

            retVal.resize(sz);

            char* _retVal = retVal.data();

            for (i = 0; i < sz; i++)
                _retVal[i] = data[i] & 0x7f;
        } else if ((codec == "binary") || (codec == "latin1")) {
            exlib::wstring wdata;
            wdata.resize(sz);
            char16_t* ws = wdata.data();
            for (size_t i = 0; i < sz; i++)
                ws[i] = (uint8_t)data[i];

            retVal = utf16to8String(wdata);
        } else
            return encoding_conv(codec).decode(data, sz, retVal);
    }
    return result_t();
}

result_t commonEncode(exlib::string codec, exlib::string data, exlib::string& retVal)
{
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined")) {
        retVal = data;
        return result_t();
    }

    return commonEncode(codec, data.c_str(), data.length(), retVal);
}

result_t commonDecode(exlib::string codec, exlib::string data, exlib::string& retVal)
{
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined")) {
        retVal = data;
    } else {
        if ((codec == "hex"))
            hexDecode(data.c_str(), data.length(), retVal);
        else if ((codec == "base32"))
            base32Decode(data.c_str(), data.length(), retVal);
        else if ((codec == "base58"))
            return base58Decode(data.c_str(), data.length(), retVal);
        else if ((codec == "base64") || (codec == "base64url"))
            base64Decode(data.c_str(), data.length(), retVal);
        else if ((codec == "ascii")) {
            size_t i;
            size_t sz = data.length();

            retVal.resize(sz);

            char* _retVal = retVal.data();

            for (i = 0; i < sz; i++)
                _retVal[i] = data[i] & 0x7f;
        } else if ((codec == "binary") || (codec == "latin1")) {
            exlib::wstring wdata = utf8to16String(data);
            size_t sz = wdata.length();
            const char16_t* s = wdata.c_str();

            retVal.resize(sz);
            char* _retVal = retVal.data();
            for (size_t i = 0; i < sz; i++)
                _retVal[i] = (char)s[i];
        } else
            return encoding_conv(codec).encode(data, retVal);
    }

    return result_t();
}

inline bool is_native_codec(exlib::string codec)
{
    return encoding_conv::is_buffer_codec(codec);
}

inline bool static_is_safe_codec(exlib::string codec)
{
    Isolate* isolate = NULL;
    Runtime* rt = Runtime::current();
    if (rt)
        isolate = rt->safe_isolate();

    return (isolate && !isolate->m_safe_buffer) || is_native_codec(codec);
}

result_t encoding_base::isEncoding(exlib::string codec, bool& retVal)
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

// the buffer form of the merged families below
static result_t encoding_encode_buffer(Buffer_base* data, exlib::string codec, exlib::string& retVal);
static result_t multibase_encode_buffer(Buffer_base* data, exlib::string codec, exlib::string& retVal);

result_t encoding_base::encode(Union_encode_data data, exlib::string codec, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        obj_ptr<Buffer_base> buf;

        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
        if (hr.failed())
            return hr;

        return encoding_encode_buffer(buf, codec, retVal);
    }

    return encoding_encode_buffer(std::get<obj_ptr<Buffer_base>>(data), codec, retVal);
}

static result_t encoding_encode_buffer(Buffer_base* data, exlib::string codec, exlib::string& retVal)
{
    if (!static_is_safe_codec(codec))
        return result_t::make(CALL_E_INVALID_CALL).with_format("encoding: Unknown codec: '%s'.", codec.c_str());

    Buffer* _data = Buffer::Cast(data);

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined"))
        retVal.assign((const char*)_data->data(), _data->length());
    else {
        if ((codec == "hex"))
            return hex_base::encode(data, retVal);
        else if ((codec == "base32"))
            return base32_base::encode(data, retVal);
        else if ((codec == "base58"))
            return base58_base::encode(data, retVal);
        else if (codec == "base64")
            return base64_base::encode(data, false, retVal);
        else if (codec == "base64url")
            return base64_base::encode(data, true, retVal);
        else if ((codec == "ascii")) {
            size_t i;
            size_t sz = _data->length();
            uint8_t* p = _data->data();

            retVal.resize(sz);

            char* _retVal = retVal.data();

            for (i = 0; i < sz; i++)
                _retVal[i] = p[i] & 0x7f;
        } else if ((codec == "binary") || (codec == "latin1")) {
            size_t sz = _data->length();
            uint8_t* p = _data->data();

            exlib::wstring wdata;
            wdata.resize(sz);
            char16_t* ws = wdata.data();
            for (size_t i = 0; i < sz; i++)
                ws[i] = p[i];

            retVal = utf16to8String(wdata);
        } else {
            // 注意：这里必须用 decode，不是笔误。
            //
            // encoding.encode(Buffer, codec) 的返回值是**字符串**，而 fibjs 的
            // 字符串只能是合法 UTF-8 —— 任意目标编码的原始字节（如 ucs2 的
            // '87 65'）无法无损地放进字符串。所以这条"bytes → text"的路径实际
            // 是把输入字节按 codec **解**成 UTF-8 文本（与 Buffer.toString(codec)
            // 同向），而不是编成 codec 的字节。
            //
            // 想要"字符串 → 某编码的字节"，用 Buffer.from(str, codec)。
            return encoding_conv(codec).decode(data, retVal);
        }
    }

    return result_t();
}

result_t encoding_base::decode(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    if (!static_is_safe_codec(codec))
        return result_t::make(CALL_E_INVALID_CALL).with_format("encoding: Unknown codec: '%s'.", codec.c_str());

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined"))
        retVal = new Buffer(str.c_str(), str.length());
    else {
        if ((codec == "hex"))
            return hex_base::decode(str, retVal);
        else if ((codec == "base32"))
            return base32_base::decode(str, retVal);
        else if ((codec == "base58"))
            return base58_base::decode(str, retVal);
        else if ((codec == "base64") || (codec == "base64url"))
            return base64_base::decode(str, retVal);
        else if ((codec == "ascii")) {
            size_t i;
            size_t sz = str.length();
            const char* p = str.c_str();

            Buffer* _data = new Buffer(NULL, sz);
            uint8_t* _retVal = _data->data();

            for (i = 0; i < sz; i++)
                _retVal[i] = p[i] & 0x7f;

            retVal = _data;
        } else if ((codec == "binary") || (codec == "latin1")) {
            exlib::wstring wdata = utf8to16String(str);
            size_t sz = wdata.length();
            const char16_t* s = wdata.c_str();

            Buffer* _data = new Buffer(NULL, sz);
            uint8_t* _retVal = _data->data();
            for (size_t i = 0; i < sz; i++)
                _retVal[i] = (uint8_t)s[i];

            retVal = _data;
        } else {
            encoding_conv conv(codec);
            return conv.encode(str, retVal);
        }
    }

    return result_t();
}

result_t multibase_base::encode(Union_encode_data data, exlib::string codec, exlib::string& retVal)
{
    if (std::holds_alternative<exlib::string>(data)) {
        obj_ptr<Buffer_base> buf;

        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
        if (hr.failed())
            return hr;

        return multibase_encode_buffer(buf, codec, retVal);
    }

    return multibase_encode_buffer(std::get<obj_ptr<Buffer_base>>(data), codec, retVal);
}

static result_t multibase_encode_buffer(Buffer_base* data, exlib::string codec, exlib::string& retVal)
{
    exlib::string strBuffer;
    Buffer* data_buf = Buffer::Cast(data);

    if ((codec == "base16")) {
        hexEncode((const char*)data_buf->data(), data_buf->length(), false, strBuffer);
        retVal.assign(1, 'f');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base16upper")) {
        hexEncode((const char*)data_buf->data(), data_buf->length(), true, strBuffer);
        retVal.assign(1, 'F');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base32")) {
        base32Encode((const char*)data_buf->data(), data_buf->length(), false, false, strBuffer);
        retVal.assign(1, 'b');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base32upper")) {
        base32Encode((const char*)data_buf->data(), data_buf->length(), true, false, strBuffer);
        retVal.assign(1, 'B');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base32pad")) {
        base32Encode((const char*)data_buf->data(), data_buf->length(), false, true, strBuffer);
        retVal.assign(1, 'c');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base32padupper")) {
        base32Encode((const char*)data_buf->data(), data_buf->length(), true, true, strBuffer);
        retVal.assign(1, 'C');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base58btc")) {
        base58Encode((const char*)data_buf->data(), data_buf->length(), strBuffer);
        retVal.assign(1, 'z');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base64")) {
        base64Encode((const char*)data_buf->data(), data_buf->length(), false, false, strBuffer);
        retVal.assign(1, 'm');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base64pad")) {
        base64Encode((const char*)data_buf->data(), data_buf->length(), false, true, strBuffer);
        retVal.assign(1, 'M');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base64url")) {
        base64Encode((const char*)data_buf->data(), data_buf->length(), true, false, strBuffer);
        retVal.assign(1, 'u');
        retVal.append(strBuffer);
        return result_t();
    } else if ((codec == "base64urlpad")) {
        base64Encode((const char*)data_buf->data(), data_buf->length(), true, true, strBuffer);
        retVal.assign(1, 'U');
        retVal.append(strBuffer);
        return result_t();
    }

    return result_t::make(CALL_E_EXCEPTION).with_format("multibase: unknown codec: '%s'.", codec.c_str());
}

result_t multibase_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    const char* p = data.c_str();
    size_t sz = data.length();
    char type = p[0];

    if (type == 'f' || type == 'F') {
        hexDecode(p + 1, (int32_t)sz - 1, retVal);
        return result_t();
    } else if (type == 'b' || type == 'B' || type == 'c' || type == 'C') {
        base32Decode(p + 1, (int32_t)sz - 1, retVal);
        return result_t();
    } else if (type == 'm' || type == 'M' || type == 'u' || type == 'U') {
        base64Decode(p + 1, (int32_t)sz - 1, retVal);
        return result_t();
    } else if (type == 'z') {
        return base58Decode(p + 1, (int32_t)sz - 1, retVal);
    }

    return result_t::make(CALL_E_EXCEPTION).with_format("multibase: unknown codec: '%c'.", type);
}

static const char* URITable = " ! #$ &'()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";
static const char* URIComponentTable = " !     '()*  -. 0123456789       ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";

result_t encoding_base::encodeURI(exlib::string url, exlib::string& retVal)
{
    Url::encodeURI(url, retVal, URITable);
    return result_t();
}

result_t encoding_base::encodeURIComponent(exlib::string url, bool formEncoded, exlib::string& retVal)
{
    Url::encodeURI(url, retVal, URIComponentTable, formEncoded);
    return result_t();
}

result_t encoding_base::decodeURI(exlib::string url, exlib::string& retVal)
{
    Url::decodeURI(url, retVal);
    return result_t();
}
}
