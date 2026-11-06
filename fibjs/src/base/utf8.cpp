#include "utf8.h"
#include "simdutf.h"

namespace fibjs {

// ============================================================================
// simdutf 后端（2026-11-06）
//
// 以下 4 个 utf_convert 重载原本是纯标量的逐字符循环。64 KB 负载上：
//   utf8 -> utf16  77 µs（CJK）/ 62 µs（ASCII）
//   utf16 -> utf8 424 µs
// 换成 simdutf（AVX2/AVX-512/NEON）后为 16 / 3.5 / 18 µs，快 4.8-23×。
//
// **非法输入的处理不再沿用旧的宽松策略**（旧代码把 0xC0 0xAF 解成 '/'、把 5 字节
// 序列解成代理对、把超出 0x10FFFF 的码点回绕）。现在按 Unicode 标准用 U+FFFD：
// 每遇到一个非法序列产出一个替换字符，与 V8 的 WriteUtf8(REPLACE_INVALID_UTF8)
// 以及浏览器的 TextDecoder 一致。
//
// dstlen 的语义保持不变：
//   dst == NULL  → 只测量，返回所需的输出单元数
//   dst != NULL  → 最多写 dstlen，放不下的部分截断且不产出半个字符
// ============================================================================

// UTF-8 首字节声明的序列长度（0 = 非法首字节）。只用于错误恢复与截断判定。
static inline int utf8_seq_len(uint8_t c)
{
    if (c < 0x80)
        return 1;
    if (c >= 0xC2 && c <= 0xDF)
        return 2;
    if (c >= 0xE0 && c <= 0xEF)
        return 3;
    if (c >= 0xF0 && c <= 0xF4)
        return 4;
    return 0;   // 0x80-0xC1（续字节或过长编码）、0xF5-0xFF
}

// 非法序列的恢复位置（WHATWG 的 "maximal subpart" 规则）。
//
// 这里返回的不是"整个序列的结束位置"，而是**下一个 U+FFFD 的起点**：
//
//   - 首字节本身非法（0x80-0xC1 / 0xF5-0xFF）      -> 只吃 1 个字节
//   - 首字节合法但续字节坏掉：
//       · 后续还有合法续字节 -> 吃掉"首字节 + 已匹配的续字节"，剩下的
//         字节各自再产出一个 U+FFFD
//       · 一个续字节都没有   -> 只吃首字节
//
// 例（node/WHATWG 与 Unicode 标准一致）：
//    E0 80 AF  -> 3 个 U+FFFD（E0 后面跟的不是 A0-BF，maximal subpart 只有 E0；
//                 80 和 AF 各自独立非法）
//    E4 B8     -> 1 个 U+FFFD（E4 B8 是合法前缀，只是被截断）
//    E4 B8 AD 中夹 80 -> 中、U+FFFD、中
//
// 旧实现把整个声明的序列长度全吞掉（E0 80 AF 只产 1 个 U+FFFD），比 node 少。
static inline size_t utf8_skip_invalid(const char* data, size_t len, size_t i)
{
    uint8_t c = (uint8_t)data[i];
    int n = utf8_seq_len(c);
    if (n == 0)
        return i + 1;               // 首字节非法 -> 一个 U+FFFD

    // 首字节合法：还要检查第二字节的合法范围（避免过长编码/代理项被当成
    // "合法前缀"）。这与 maximal subpart 的定义一致。
    if (i + 1 >= len)
        return i + 1;               // 截断 -> 一个 U+FFFD，没有更多字节可吃

    uint8_t c1 = (uint8_t)data[i + 1];
    bool ok1;
    if (c == 0xE0)
        ok1 = (c1 >= 0xA0 && c1 <= 0xBF);   // 排除过长编码
    else if (c == 0xED)
        ok1 = (c1 >= 0x80 && c1 <= 0x9F);   // 排除代理项
    else if (c == 0xF0)
        ok1 = (c1 >= 0x90 && c1 <= 0xBF);   // 排除过长编码
    else if (c == 0xF4)
        ok1 = (c1 >= 0x80 && c1 <= 0x8F);   // 排除 > U+10FFFF
    else
        ok1 = (c1 >= 0x80 && c1 <= 0xBF);

    if (!ok1)
        return i + 1;               // 第二字节就坏了 -> 只吃首字节

    // 首字节 + 第二字节构成合法前缀，继续往后吃合法续字节
    size_t j = i + 2;
    size_t max = i + (size_t)n;
    while (j < len && j < max && (((uint8_t)data[j]) & 0xC0) == 0x80)
        j++;
    return j;
}

// 截断辅助：在一段**已知合法**的源里，求"转换后不超过 room 个输出单元"的最长前缀。
// 只在调用方给的容量不足时走这条线性路径（罕见）。
// 与 utf_convert(utf8 -> utf16/utf32) 完全对齐的输出单元计数。
//
// **为什么不能直接用 simdutf::utf16_length_from_utf8 / utf32_length_from_utf8**：
// 那两个函数只在输入**全部合法**时才有意义（遇到非法序列时 simdutf 按"1 个
// U+FFFD 吃掉整段"处理），而 utf_convert 现在按 WHATWG 的 maximal subpart 规则
// 产出**多个** U+FFFD。两者不一致会导致调用方按 measure 分配的缓冲装不下实际
// 内容 —— 表现为孤代理项之后的所有字符被写成 U+FFFD
// （Buffer.from('x\ud800yz','utf16be') 曾经得到 0078 fffd fffd fffd）。
//
// 这个函数逐字节走与 utf_convert 相同的判定，保证 measure == convert。
// 求 src 开头**最长合法前缀**的长度。
//
// **必须只用 *_with_errors**：它在遇到第一个错误时就停下，代价是 O(合法前缀)。
// 不能写成"先 validate_utf8 判整段、失败再 with_errors" —— validate_utf8 是
// 整段扫描、**不早退**，于是在"每个字节都非法"的输入上，每一轮都要把剩余全部
// 数据重扫一遍，整体退化成 O(n²)：实测 256 KB 全 0x80 的 Buffer.toString('utf8')
// 要 4.2 秒（node 5 ms），256 KB 的非法码点 utf32 解码同样二次爆炸。
//
// all_valid 由调用方在进入主循环**之前**用一次整段 validate_* 得出：
//   - 全合法（绝大多数真实负载）→ 直接返回 len，主循环一轮处理完
//   - 含非法 → 每次只扫到下一个错误，总代价 O(n)
static inline size_t utf8_valid_prefix(const char* src, size_t len, bool all_valid)
{
    return all_valid ? len : (size_t)simdutf::validate_utf8_with_errors(src, len).count;
}

static inline size_t utf16_valid_prefix(const char16_t* src, size_t len, bool all_valid)
{
    return all_valid ? len : (size_t)simdutf::validate_utf16_with_errors(src, len).count;
}

static inline size_t utf32_valid_prefix(const char32_t* src, size_t len, bool all_valid)
{
    return all_valid ? len : (size_t)simdutf::validate_utf32_with_errors(src, len).count;
}

static size_t utf8_measure_units(const char* src, size_t len, size_t units_per_cp)
{
    size_t n = 0, i = 0;
    const bool all_valid = simdutf::validate_utf8(src, len);
    while (i < len) {
        size_t ok = utf8_valid_prefix(src + i, len - i, all_valid);

        if (ok) {
            // 合法前缀：utf16 有代理对（4 字节序列 -> 2 个单元），utf32 恒 1:1
            n += (units_per_cp == 2)
                ? simdutf::utf16_length_from_utf8(src + i, ok)
                : simdutf::utf32_length_from_utf8(src + i, ok);
            i += ok;
            continue;
        }

        // 一个 U+FFFD（utf16 与 utf32 都是 1 个单元）
        n += 1;
        size_t next = utf8_skip_invalid(src, len, i);
        i = next > i ? next : i + 1;
    }
    return n;
}

static size_t utf8_fit_utf16(const char* src, size_t ok, size_t room)
{
    size_t i = 0, used = 0;
    while (i < ok) {
        int l = utf8_seq_len((uint8_t)src[i]);
        if (l <= 0)
            l = 1;
        size_t u = (l == 4) ? 2 : 1;      // 4 字节序列 → 代理对
        if (used + u > room)
            return i;
        used += u;
        i += (size_t)l;
    }
    return i;
}

static size_t utf8_fit_utf32(const char* src, size_t ok, size_t room)
{
    size_t i = 0, used = 0;
    while (i < ok) {
        int l = utf8_seq_len((uint8_t)src[i]);
        if (l <= 0)
            l = 1;
        if (used + 1 > room)          // 每个码点占 1 个 code unit
            return i;
        used += 1;
        i += (size_t)l;
    }
    return i;
}

static size_t utf16_fit_utf8(const char16_t* src, size_t ok, size_t room)
{
    size_t i = 0, used = 0;
    while (i < ok) {
        char16_t c = src[i];
        size_t u, step;
        if (c >= 0xD800 && c <= 0xDBFF && i + 1 < ok
            && src[i + 1] >= 0xDC00 && src[i + 1] <= 0xDFFF) {
            u = 4;
            step = 2;
        } else if (c < 0x80) {
            u = 1;
            step = 1;
        } else if (c < 0x800) {
            u = 2;
            step = 1;
        } else {
            u = 3;
            step = 1;
        }
        if (used + u > room)
            return i;
        used += u;
        i += step;
    }
    return i;
}

// UTF-32 的单个码点转 UTF-8 后占几个字节。
//
// 必须与 utf_convert(char32_t -> char) 的规则**逐位一致**：
//   - 合法码点（<= U+10FFFF 且不在 D800..DFFF）-> 1/2/3/4 字节
//   - 非法码点（> U+10FFFF、代理项值）        -> 1 个 U+FFFD = **3 字节**
//
// 旧的 measure 只看数值大小（>= 0x10000 就 +4），于是 0x110000 / 0xFFFFFFFF
// 这类非法码点被算成 4 字节，而 convert 只写 3 字节 —— 缓冲里多出一个 NUL
// （Buffer(4 字节 BE '0x00011000').toString('utf32be') === '�\u0000'）。
static inline int utf32_cp_utf8_len(char32_t c)
{
    if (c < 0x80)
        return 1;
    if (c < 0x800)
        return 2;
    if (c >= 0xD800 && c <= 0xDFFF)
        return 3;           // 代理项值 -> U+FFFD
    if (c < 0x10000)
        return 3;
    if (c <= 0x10FFFF)
        return 4;
    return 3;               // > U+10FFFF -> U+FFFD
}

static size_t utf32_fit_utf8(const char32_t* src, size_t ok, size_t room)
{
    size_t i = 0, used = 0;
    while (i < ok) {
        // 与 utf_convert 的输出长度规则一致（非法码点算 1 个 U+FFFD = 3 字节）
        size_t u = (size_t)utf32_cp_utf8_len(src[i]);
        if (used + u > room)
            return i;
        used += u;
        i++;
    }
    return i;
}

ssize_t utf_convert(const char* src, ssize_t srclen, char16_t* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t len = (size_t)srclen;
    const bool measuring = (dst == nullptr);
    const size_t cap = measuring ? (size_t)-1 : (size_t)dstlen;
    size_t n = 0, i = 0;
    const bool all_valid = simdutf::validate_utf8(src, len);

    while (i < len && n < cap) {
        // 1) 取剩余输入里最长的**合法**前缀
        size_t ok = utf8_valid_prefix(src + i, len - i, all_valid);

        if (ok) {
            // 2) 这段一定合法，长度可精确算出
            size_t need = simdutf::utf16_length_from_utf8(src + i, ok);
            if (n + need <= cap) {
                if (!measuring)
                    n += simdutf::convert_utf8_to_utf16(src + i, ok, dst + n);
                else
                    n += need;
                i += ok;
                continue;
            }
            // 容量装不下整段：只转换能放下的前缀后结束
            size_t fit = utf8_fit_utf16(src + i, ok, cap - n);
            if (fit)
                n += simdutf::convert_utf8_to_utf16(src + i, fit, dst + n);
            break;
        }

        // 3) 第一个序列就非法 → 一个 U+FFFD，然后跳过该序列
        if (!measuring) {
            if (n >= cap)
                break;
            dst[n] = 0xFFFD;
        }
        n += 1;
        size_t next = utf8_skip_invalid(src, len, i);
        i = next > i ? next : i + 1;
    }

    return (ssize_t)n;
}

ssize_t utf_convert(const char16_t* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t len = (size_t)srclen;
    const bool measuring = (dst == nullptr);
    const size_t cap = measuring ? (size_t)-1 : (size_t)dstlen;
    size_t n = 0, i = 0;
    static const char kReplacement[] = "\xEF\xBF\xBD";   // U+FFFD
    const bool all_valid = simdutf::validate_utf16(src, len);

    while (i < len && n < cap) {
        size_t ok = utf16_valid_prefix(src + i, len - i, all_valid);

        if (ok) {
            size_t need = simdutf::utf8_length_from_utf16(src + i, ok);
            if (n + need <= cap) {
                if (!measuring)
                    n += simdutf::convert_utf16_to_utf8(src + i, ok, dst + n);
                else
                    n += need;
                i += ok;
                continue;
            }
            size_t fit = utf16_fit_utf8(src + i, ok, cap - n);
            if (fit)
                n += simdutf::convert_utf16_to_utf8(src + i, fit, dst + n);
            break;
        }

        // 孤立代理项 → U+FFFD
        if (!measuring) {
            if (cap - n < 3)
                break;
            memcpy(dst + n, kReplacement, 3);
        }
        n += 3;
        i += 1;
    }

    return (ssize_t)n;
}

ssize_t utf_convert(const char* src, ssize_t srclen, char32_t* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t len = (size_t)srclen;
    const bool measuring = (dst == nullptr);
    const size_t cap = measuring ? (size_t)-1 : (size_t)dstlen;
    size_t n = 0, i = 0;
    const bool all_valid = simdutf::validate_utf8(src, len);

    while (i < len && n < cap) {
        size_t ok = utf8_valid_prefix(src + i, len - i, all_valid);

        if (ok) {
            size_t need = simdutf::utf32_length_from_utf8(src + i, ok);
            if (n + need <= cap) {
                if (!measuring)
                    n += simdutf::convert_utf8_to_utf32(src + i, ok, dst + n);
                else
                    n += need;
                i += ok;
                continue;
            }
            size_t fit = utf8_fit_utf32(src + i, ok, cap - n);
            if (fit)
                n += simdutf::convert_utf8_to_utf32(src + i, fit, dst + n);
            break;
        }

        if (!measuring) {
            if (n >= cap)
                break;
            dst[n] = 0xFFFD;
        }
        n += 1;
        size_t next = utf8_skip_invalid(src, len, i);
        i = next > i ? next : i + 1;
    }

    return (ssize_t)n;
}

ssize_t utf_convert(const char32_t* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t len = (size_t)srclen;
    const bool measuring = (dst == nullptr);
    const size_t cap = measuring ? (size_t)-1 : (size_t)dstlen;
    size_t n = 0, i = 0;
    static const char kReplacement[] = "\xEF\xBF\xBD";
    const bool all_valid = simdutf::validate_utf32(src, len);

    while (i < len && n < cap) {
        size_t ok = utf32_valid_prefix(src + i, len - i, all_valid);

        if (ok) {
            size_t need = simdutf::utf8_length_from_utf32(src + i, ok);
            if (n + need <= cap) {
                if (!measuring)
                    n += simdutf::convert_utf32_to_utf8(src + i, ok, dst + n);
                else
                    n += need;
                i += ok;
                continue;
            }
            size_t fit = utf32_fit_utf8(src + i, ok, cap - n);
            if (fit)
                n += simdutf::convert_utf32_to_utf8(src + i, fit, dst + n);
            break;
        }

        if (!measuring) {
            if (cap - n < 3)
                break;
            memcpy(dst + n, kReplacement, 3);
        }
        n += 3;
        i += 1;
    }

    return (ssize_t)n;
}

// ---------------------------------------------------------------------------
// 字节交换变体（utf_convert_s）
//
// 这 4 个重载服务的是大端编码（ucs2be / utf16be / ucs4be / utf32be，以及在小端
// 主机上的 *le 名称）。旧实现是逐 code unit 的手写循环：64 KB 的 ucs2be 编码要
// 200 µs，utf32 编码要 6306 µs。
//
// simdutf 有 utf16be 的原生支持（convert_utf8_to_utf16be / convert_utf16be_to_utf8，
// 64 KB 只要 12.4 / 5.8 µs）；utf32be 没有，但可以先做 LE 转换再就地 swap32
// （13.1 µs）。两条路径都远快于原实现。
//
// 语义：与 utf_convert 一致 —— 非法序列产出 U+FFFD，dst==null 只测量，
// dstlen 是硬上限且不写半个字符。
// ---------------------------------------------------------------------------

// 16 位字节交换。**不要用 __builtin_bswap16**：那是 GCC/Clang 内建，
// MSVC（fibjs 支持的 Windows 构建）没有，会直接编译失败。
static inline uint16_t utf16_bswap(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}

// 32 位字节交换
static inline uint32_t utf32_bswap(uint32_t v)
{
    return ((v & 0x000000ffu) << 24) | ((v & 0x0000ff00u) << 8)
        | ((v & 0x00ff0000u) >> 8) | ((v & 0xff000000u) >> 24);
}

// 32 位就地字节交换
static inline void utf32_swap(char32_t* p, size_t n)
{
    for (size_t i = 0; i < n; i++)
        p[i] = (char32_t)utf32_bswap((uint32_t)p[i]);
}

// UTF-8 <-> UTF-16，大端
ssize_t utf_convert_s(const char* src, ssize_t srclen, char16_t* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t cap = dst ? (size_t)dstlen : (size_t)-1;

    if (!dst) {
        // 测量：必须与下面的 convert 走同一套非法序列规则（见 utf8_measure_units）
        return (ssize_t)utf8_measure_units(src, (size_t)srclen, 2);
    }

    if (!simdutf::validate_utf8(src, (size_t)srclen)) {
        // 含非法序列：退回通用的 lenient 路径（它按 U+FFFD 处理），
        // 再把结果交换到 BE。
        ssize_t need = (ssize_t)utf8_measure_units(src, (size_t)srclen, 2);
        if ((size_t)need > cap)
            need = (ssize_t)cap;
        if (need <= 0)
            return 0;

        exlib::wstring w;
        w.resize((size_t)need);
        ssize_t n = utf_convert(src, srclen, w.data(), need);
        if (n <= 0)
            return n;
        for (ssize_t i = 0; i < n; i++) {
            uint16_t c = (uint16_t)w[i];
            dst[i] = (char16_t)((c << 8) | (c >> 8));
        }
        return n;
    }

    // 合法：simdutf 直接产出 BE
    size_t units = simdutf::utf16_length_from_utf8(src, (size_t)srclen);
    if (units > cap) {
        // 截断到 cap（可能切在代理对中间：simdutf 不会写半个，用 LE 版逐段对齐）
        units = cap;
        exlib::wstring w;
        w.resize(units + 1);
        ssize_t n = utf_convert(src, srclen, w.data(), (ssize_t)units + 1);
        if (n > (ssize_t)units)
            n = (ssize_t)units;
        for (ssize_t i = 0; i < n; i++) {
            uint16_t c = (uint16_t)w[i];
            dst[i] = (char16_t)((c << 8) | (c >> 8));
        }
        return n;
    }

    return (ssize_t)simdutf::convert_utf8_to_utf16be(src, (size_t)srclen, dst);
}

// UTF-16(BE) -> UTF-8 的**合法输入**快速实现（measure 与 convert 同规则）。
// 调用方必须先确认 validate_utf16be(src, srclen) 为真。
//
// 合法输入下 simdutf 的 utf8_length_from_utf16be 与 convert_utf16be_to_utf8
// 完全一致（代理对按 4 字节计、没有孤代理项这个歧义），所以两边都可以用它。
static ssize_t utf_convert_s_be_impl(const char16_t* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    if (!dst)
        return (ssize_t)simdutf::utf8_length_from_utf16be(src, (size_t)srclen);

    return (ssize_t)simdutf::convert_utf16be_to_utf8(src, (size_t)srclen, dst);
}

// UTF-16(BE) -> UTF-8
ssize_t utf_convert_s(const char16_t* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    // **measure 不能无条件用 simdutf::utf8_length_from_utf16be**。
    //
    // 那个函数把孤代理项算作 **2 字节**（它的公式是 `+1 ASCII、+1 非 ASCII、
    // +1 仅 3 字节区`，而代理项落在"非 ASCII 但不进 3 字节区"这一档），
    // 而下面的 convert 走 utf_convert(LE)，按 Unicode 语义把孤代理项写成
    // **一个 U+FFFD = 3 字节**。两者差 1 字节，后果是：
    //
    //   调用方（utf16to8String_s）按 measure 的 2 分配、resize(2)，
    //   convert 拿到 dstlen=2 时因为"放不下一个 3 字节的替换符"直接 break，
    //   于是一个字节都不写 —— 结果是一串 NUL，**整段数据静默丢失**：
    //     encoding.encode(Buffer.from([0xd8,0x00]),'utf16be')  ->  "\0\0"
    //     Buffer.from([0xd8,0x00,0x00,0x41]).toString('utf16be') -> "\0\0\0..."
    //   （实测全部 2048 个孤代理项都受影响。）
    //
    // 但**不能**为此让 measure 永远多分配一次 —— 合法输入的绝大多数（99.9%）
    // 没有孤代理项，simdutf 的向量化 measure 是免费的。所以先做一次
    // validate_utf16be：合法就走 simdutf（此时它与 convert 的规则完全一致），
    // 不合法才退回"先 bswap 再用 utf_convert 计数"的慢路径。
    if (simdutf::validate_utf16be(src, (size_t)srclen))
        return utf_convert_s_be_impl(src, srclen, dst, dstlen);

    // 含孤代理项：先转成主机序，再走通用路径（与 utf_convert 的 U+FFFD 语义一致）
    exlib::wstring w;
    w.resize((size_t)srclen);
    for (ssize_t i = 0; i < srclen; i++)
        w[i] = (char16_t)utf16_bswap((uint16_t)src[i]);

    return utf_convert(w.data(), srclen, dst, dstlen);
}

// UTF-8 -> UTF-32（主机序的内存布局即 *LE）
ssize_t utf_convert_s(const char* src, ssize_t srclen, char32_t* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    const size_t cap = dst ? (size_t)dstlen : (size_t)-1;

    if (!dst)
        return (ssize_t)utf8_measure_units(src, (size_t)srclen, 1);

    ssize_t n = utf_convert(src, srclen, dst, (ssize_t)cap);
    if (n > 0)
        utf32_swap(dst, (size_t)n);
    return n;
}

// UTF-32(BE) -> UTF-8
ssize_t utf_convert_s(const char32_t* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    if (srclen <= 0)
        return 0;

    if (!dst) {
        // **measure 阶段必须先做字节交换**。
        // utf8_length_from_utf32 把输入当主机序；而这里的 src 是大端字节，
        // 在小端机上 'a' 的 00 00 00 61 会被读成 0x61000000（超出 U+10FFFF），
        // 于是长度算错。所以这里自己交换 + 用与 convert 相同的规则计数。
        int64_t len = 0;
        for (ssize_t i = 0; i < srclen; i++)
            len += utf32_cp_utf8_len((char32_t)utf32_bswap((uint32_t)src[i]));
        return (ssize_t)len;
    }

    // 先交换到主机序再走通用路径
    exlib::wstring32 w;
    w.resize((size_t)srclen);
    for (ssize_t i = 0; i < srclen; i++)
        w[i] = (char32_t)utf32_bswap((uint32_t)src[i]);

    return utf_convert(w.data(), srclen, dst, dstlen);
}

}
