/*
 * VecKernel.h
 *
 * Scalar/SIMD kernel for the vec_index v2 storage.
 * All callers (index scans and the SQL functions) must go through this
 * header so distances stay bit-identical within a build.
 *
 * Dispatch (chosen once at build/run time, never mixed inside a build):
 *   - aarch64: NEON, four accumulators per row
 *   - x86-64:  SSE2 baseline, AVX2+FMA via runtime dispatch
 *   - other:   scalar
 * dot(), ip_distance() and ip_distance4() always take the same path, so the
 * index scan and vec_distance_ip() agree bit for bit on the same machine.
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <vector>

#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define VEC_KERNEL_NEON 1
#endif

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#if defined(_MSC_VER)
// clang-cl targets the MSVC CRT, which provides neither __cpu_model nor
// __cpu_indicator_init; cpu_has_avx2() probes CPUID/XGETBV instead.
#include <intrin.h>
#endif
#define VEC_KERNEL_X64 1
// MSVC has no per-function target attributes; it keeps the SSE2 path.
#if defined(__clang__) || defined(__GNUC__)
#define VEC_KERNEL_X64_DISPATCH 1
#endif
#endif

// 32-bit x86 without SSE2 evaluates float expressions in x87 80-bit
// registers, so the same inline function can round differently at different
// call sites.  Storing each partial sum as a volatile float forces the value
// back to float32 and keeps the index and the SQL functions bit-identical.
#if (defined(__i386__) || defined(_M_IX86)) \
    && !defined(__SSE2__) \
    && (!defined(_M_IX86_FP) || _M_IX86_FP < 2)
#define VEC_KERNEL_X87 1
#endif

namespace fibjs {
namespace vec {

// Row stride in floats; rows stay 64-byte aligned when the base is.
inline size_t stride_of(size_t dim)
{
    return (dim + 15) & ~(size_t)15;
}

inline float dot_scalar(const float* a, const float* b, size_t n)
{
#ifdef VEC_KERNEL_X87
    volatile float s0 = 0, s1 = 0, s2 = 0, s3 = 0;
#else
    float s0 = 0, s1 = 0, s2 = 0, s3 = 0;
#endif
    size_t i = 0;

    for (; i + 4 <= n; i += 4) {
        s0 += a[i] * b[i];
        s1 += a[i + 1] * b[i + 1];
        s2 += a[i + 2] * b[i + 2];
        s3 += a[i + 3] * b[i + 3];
    }

#ifdef VEC_KERNEL_X87
    volatile float s = (s0 + s1) + (s2 + s3);
#else
    float s = (s0 + s1) + (s2 + s3);
#endif

    for (; i < n; i++)
        s += a[i] * b[i];

    return s;
}

#ifdef VEC_KERNEL_NEON
inline float dot_neon(const float* a, const float* b, size_t n)
{
    float32x4_t s0 = vdupq_n_f32(0);
    float32x4_t s1 = vdupq_n_f32(0);
    float32x4_t s2 = vdupq_n_f32(0);
    float32x4_t s3 = vdupq_n_f32(0);
    size_t i = 0;

    for (; i + 16 <= n; i += 16) {
        s0 = vfmaq_f32(s0, vld1q_f32(a + i), vld1q_f32(b + i));
        s1 = vfmaq_f32(s1, vld1q_f32(a + i + 4), vld1q_f32(b + i + 4));
        s2 = vfmaq_f32(s2, vld1q_f32(a + i + 8), vld1q_f32(b + i + 8));
        s3 = vfmaq_f32(s3, vld1q_f32(a + i + 12), vld1q_f32(b + i + 12));
    }

    s0 = vaddq_f32(vaddq_f32(s0, s1), vaddq_f32(s2, s3));

    float r = vaddvq_f32(s0);
    for (; i < n; i++)
        r += a[i] * b[i];

    return r;
}

// Four rows against one query in a single pass.
//
// The query block is loaded once and reused by four vectors, which halves the
// load count per row (A: 32 query + 32 vector loads per row, this: 8 + 32).
// Each row keeps the four accumulators of dot_neon fed in the same order and
// is reduced with the same tree, so out[i] is bit-identical to
// 1.0f - dot(q, v_i, n).  Parity with vec_distance_ip depends on that.
inline void dot4_neon(const float* q, const float* v0, const float* v1,
    const float* v2, const float* v3, size_t n, float out[4])
{
    float32x4_t a0 = vdupq_n_f32(0), a1 = a0, a2 = a0, a3 = a0;
    float32x4_t b0 = a0, b1 = a0, b2 = a0, b3 = a0;
    float32x4_t c0 = a0, c1 = a0, c2 = a0, c3 = a0;
    float32x4_t d0 = a0, d1 = a0, d2 = a0, d3 = a0;
    size_t i = 0;

    for (; i + 16 <= n; i += 16) {
        float32x4_t q0 = vld1q_f32(q + i);
        float32x4_t q1 = vld1q_f32(q + i + 4);
        float32x4_t q2 = vld1q_f32(q + i + 8);
        float32x4_t q3 = vld1q_f32(q + i + 12);

        a0 = vfmaq_f32(a0, q0, vld1q_f32(v0 + i));
        a1 = vfmaq_f32(a1, q1, vld1q_f32(v0 + i + 4));
        a2 = vfmaq_f32(a2, q2, vld1q_f32(v0 + i + 8));
        a3 = vfmaq_f32(a3, q3, vld1q_f32(v0 + i + 12));

        b0 = vfmaq_f32(b0, q0, vld1q_f32(v1 + i));
        b1 = vfmaq_f32(b1, q1, vld1q_f32(v1 + i + 4));
        b2 = vfmaq_f32(b2, q2, vld1q_f32(v1 + i + 8));
        b3 = vfmaq_f32(b3, q3, vld1q_f32(v1 + i + 12));

        c0 = vfmaq_f32(c0, q0, vld1q_f32(v2 + i));
        c1 = vfmaq_f32(c1, q1, vld1q_f32(v2 + i + 4));
        c2 = vfmaq_f32(c2, q2, vld1q_f32(v2 + i + 8));
        c3 = vfmaq_f32(c3, q3, vld1q_f32(v2 + i + 12));

        d0 = vfmaq_f32(d0, q0, vld1q_f32(v3 + i));
        d1 = vfmaq_f32(d1, q1, vld1q_f32(v3 + i + 4));
        d2 = vfmaq_f32(d2, q2, vld1q_f32(v3 + i + 8));
        d3 = vfmaq_f32(d3, q3, vld1q_f32(v3 + i + 12));
    }

    a0 = vaddq_f32(vaddq_f32(a0, a1), vaddq_f32(a2, a3));
    b0 = vaddq_f32(vaddq_f32(b0, b1), vaddq_f32(b2, b3));
    c0 = vaddq_f32(vaddq_f32(c0, c1), vaddq_f32(c2, c3));
    d0 = vaddq_f32(vaddq_f32(d0, d1), vaddq_f32(d2, d3));

    out[0] = vaddvq_f32(a0);
    out[1] = vaddvq_f32(b0);
    out[2] = vaddvq_f32(c0);
    out[3] = vaddvq_f32(d0);

    for (; i < n; i++) {
        float qi = q[i];
        out[0] += qi * v0[i];
        out[1] += qi * v1[i];
        out[2] += qi * v2[i];
        out[3] += qi * v3[i];
    }
}
#endif

#ifdef VEC_KERNEL_X64
// x86-64: one row, two accumulators (8 floats per iteration).  dot4_sse2()
// keeps the same per-row shape, so both agree bit for bit on the same build.
inline float hsum128(__m128 x)
{
    __m128 hi = _mm_movehl_ps(x, x);
    __m128 s = _mm_add_ps(x, hi); // [x0+x2, x1+x3, ...]
    s = _mm_add_ss(s, _mm_shuffle_ps(s, s, 0x55));
    return _mm_cvtss_f32(s);
}

inline float dot_sse2(const float* a, const float* b, size_t n)
{
    __m128 s0 = _mm_setzero_ps();
    __m128 s1 = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 8 <= n; i += 8) {
        s0 = _mm_add_ps(s0, _mm_mul_ps(_mm_loadu_ps(a + i), _mm_loadu_ps(b + i)));
        s1 = _mm_add_ps(s1, _mm_mul_ps(_mm_loadu_ps(a + i + 4), _mm_loadu_ps(b + i + 4)));
    }

    float r = hsum128(_mm_add_ps(s0, s1));
    for (; i < n; i++)
        r += a[i] * b[i];

    return r;
}

inline void dot4_sse2(const float* q, const float* v0, const float* v1,
    const float* v2, const float* v3, size_t n, float out[4])
{
    __m128 a0 = _mm_setzero_ps(), a1 = a0;
    __m128 b0 = a0, b1 = a0;
    __m128 c0 = a0, c1 = a0;
    __m128 d0 = a0, d1 = a0;
    size_t i = 0;

    for (; i + 8 <= n; i += 8) {
        __m128 q0 = _mm_loadu_ps(q + i);
        __m128 q1 = _mm_loadu_ps(q + i + 4);

        a0 = _mm_add_ps(a0, _mm_mul_ps(q0, _mm_loadu_ps(v0 + i)));
        a1 = _mm_add_ps(a1, _mm_mul_ps(q1, _mm_loadu_ps(v0 + i + 4)));

        b0 = _mm_add_ps(b0, _mm_mul_ps(q0, _mm_loadu_ps(v1 + i)));
        b1 = _mm_add_ps(b1, _mm_mul_ps(q1, _mm_loadu_ps(v1 + i + 4)));

        c0 = _mm_add_ps(c0, _mm_mul_ps(q0, _mm_loadu_ps(v2 + i)));
        c1 = _mm_add_ps(c1, _mm_mul_ps(q1, _mm_loadu_ps(v2 + i + 4)));

        d0 = _mm_add_ps(d0, _mm_mul_ps(q0, _mm_loadu_ps(v3 + i)));
        d1 = _mm_add_ps(d1, _mm_mul_ps(q1, _mm_loadu_ps(v3 + i + 4)));
    }

    out[0] = hsum128(_mm_add_ps(a0, a1));
    out[1] = hsum128(_mm_add_ps(b0, b1));
    out[2] = hsum128(_mm_add_ps(c0, c1));
    out[3] = hsum128(_mm_add_ps(d0, d1));

    for (; i < n; i++) {
        float qi = q[i];
        out[0] += qi * v0[i];
        out[1] += qi * v1[i];
        out[2] += qi * v2[i];
        out[3] += qi * v3[i];
    }
}
#endif

#if defined(VEC_KERNEL_X64_DISPATCH)
#define VEC_KERNEL_AVX2 __attribute__((target("avx2,fma")))

VEC_KERNEL_AVX2 inline float hsum256(__m256 x)
{
    __m128 lo = _mm256_castps256_ps128(x);
    __m128 hi = _mm256_extractf128_ps(x, 1);
    return hsum128(_mm_add_ps(lo, hi));
}

// AVX2: one row, two accumulators (16 floats per iteration).
VEC_KERNEL_AVX2 inline float dot_avx2(const float* a, const float* b, size_t n)
{
    __m256 s0 = _mm256_setzero_ps();
    __m256 s1 = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 16 <= n; i += 16) {
        s0 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i), _mm256_loadu_ps(b + i), s0);
        s1 = _mm256_fmadd_ps(_mm256_loadu_ps(a + i + 8), _mm256_loadu_ps(b + i + 8), s1);
    }

    float r = hsum256(_mm256_add_ps(s0, s1));
    for (; i < n; i++)
        r += a[i] * b[i];

    return r;
}

// Variant B for x86: four rows, eight accumulators total.  The query block is
// loaded once per iteration and each row keeps the accumulator shape and the
// reduction tree of dot_avx2(), hence identical bits per row.
VEC_KERNEL_AVX2 inline void dot4_avx2(const float* q, const float* v0, const float* v1,
    const float* v2, const float* v3, size_t n, float out[4])
{
    __m256 a0 = _mm256_setzero_ps(), a1 = a0;
    __m256 b0 = a0, b1 = a0;
    __m256 c0 = a0, c1 = a0;
    __m256 d0 = a0, d1 = a0;
    size_t i = 0;

    for (; i + 16 <= n; i += 16) {
        __m256 q0 = _mm256_loadu_ps(q + i);
        __m256 q1 = _mm256_loadu_ps(q + i + 8);

        a0 = _mm256_fmadd_ps(q0, _mm256_loadu_ps(v0 + i), a0);
        a1 = _mm256_fmadd_ps(q1, _mm256_loadu_ps(v0 + i + 8), a1);

        b0 = _mm256_fmadd_ps(q0, _mm256_loadu_ps(v1 + i), b0);
        b1 = _mm256_fmadd_ps(q1, _mm256_loadu_ps(v1 + i + 8), b1);

        c0 = _mm256_fmadd_ps(q0, _mm256_loadu_ps(v2 + i), c0);
        c1 = _mm256_fmadd_ps(q1, _mm256_loadu_ps(v2 + i + 8), c1);

        d0 = _mm256_fmadd_ps(q0, _mm256_loadu_ps(v3 + i), d0);
        d1 = _mm256_fmadd_ps(q1, _mm256_loadu_ps(v3 + i + 8), d1);
    }

    out[0] = hsum256(_mm256_add_ps(a0, a1));
    out[1] = hsum256(_mm256_add_ps(b0, b1));
    out[2] = hsum256(_mm256_add_ps(c0, c1));
    out[3] = hsum256(_mm256_add_ps(d0, d1));

    for (; i < n; i++) {
        float qi = q[i];
        out[0] += qi * v0[i];
        out[1] += qi * v1[i];
        out[2] += qi * v2[i];
        out[3] += qi * v3[i];
    }
}
#endif

// Resolved once; both dot() and ip_distance4() branch on the same value.
inline bool cpu_has_avx2()
{
#if defined(VEC_KERNEL_X64_DISPATCH)
    static const bool has = []() {
#if defined(_MSC_VER)
        // clang-cl links against the MSVC CRT, which has no
        // __cpu_model/__cpu_indicator_init for __builtin_cpu_supports(),
        // so probe the CPU directly.
        int regs[4];

        __cpuid(regs, 0);
        if (regs[0] < 7)
            return false;

        __cpuid(regs, 1);
        if (!(regs[2] & (1 << 27)) // OSXSAVE
            || !(regs[2] & (1 << 28)) // AVX
            || !(regs[2] & (1 << 12))) // FMA
            return false;

        // XMM and YMM state must be enabled by the OS to use AVX.
        if ((_xgetbv(0) & 0x06) != 0x06)
            return false;

        __cpuidex(regs, 7, 0);
        return (regs[1] & (1 << 5)) != 0; // AVX2
#else
        __builtin_cpu_init();
        return __builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma");
#endif
    }();
    return has;
#else
    return false;
#endif
}

inline float dot(const float* a, const float* b, size_t n)
{
#ifdef VEC_KERNEL_NEON
    return dot_neon(a, b, n);
#elif defined(VEC_KERNEL_X64_DISPATCH)
    if (cpu_has_avx2())
        return dot_avx2(a, b, n);
    return dot_sse2(a, b, n);
#elif defined(VEC_KERNEL_X64)
    return dot_sse2(a, b, n);
#else
    return dot_scalar(a, b, n);
#endif
}

inline float ip_distance(const float* q, const float* v, size_t n)
{
    return 1.0f - dot(q, v, n);
}

// Distance of four vectors against one query; out[i] == ip_distance(q, v_i, n).
inline void ip_distance4(const float* q, const float* v0, const float* v1,
    const float* v2, const float* v3, size_t n, float out[4])
{
#ifdef VEC_KERNEL_NEON
    dot4_neon(q, v0, v1, v2, v3, n, out);
#elif defined(VEC_KERNEL_X64_DISPATCH)
    if (cpu_has_avx2())
        dot4_avx2(q, v0, v1, v2, v3, n, out);
    else
        dot4_sse2(q, v0, v1, v2, v3, n, out);
#elif defined(VEC_KERNEL_X64)
    dot4_sse2(q, v0, v1, v2, v3, n, out);
#else
    out[0] = 1.0f - dot_scalar(q, v0, n);
    out[1] = 1.0f - dot_scalar(q, v1, n);
    out[2] = 1.0f - dot_scalar(q, v2, n);
    out[3] = 1.0f - dot_scalar(q, v3, n);
    return;
#endif

    out[0] = 1.0f - out[0];
    out[1] = 1.0f - out[1];
    out[2] = 1.0f - out[2];
    out[3] = 1.0f - out[3];
}

inline void normalize(float* v, size_t n)
{
    float s = dot(v, v, n);
    float inv = 1.0f / (sqrtf(s) + 1e-30f);

    for (size_t i = 0; i < n; i++)
        v[i] *= inv;
}

struct Cand {
    float dist;
    int64_t order;
    int64_t rowid;
};

// Strict "better than" order: distance, then order key, then rowid.
inline bool cand_less(const Cand& a, const Cand& b)
{
    if (a.dist != b.dist)
        return a.dist < b.dist;
    if (a.order != b.order)
        return a.order < b.order;
    return a.rowid < b.rowid;
}

class TopK {
public:
    explicit TopK(size_t k)
        : m_k(k)
    {
        m_heap.reserve(k);
    }

    size_t size() const { return m_heap.size(); }
    bool full() const { return m_heap.size() == m_k; }
    size_t capacity() const { return m_k; }

    inline void push(float dist, int64_t order, int64_t rowid)
    {
        Cand c { dist, order, rowid };

        if (m_heap.size() < m_k) {
            m_heap.push_back(c);
            std::push_heap(m_heap.begin(), m_heap.end(), cand_less);
        } else if (cand_less(c, m_heap.front())) {
            std::pop_heap(m_heap.begin(), m_heap.end(), cand_less);
            m_heap.back() = c;
            std::push_heap(m_heap.begin(), m_heap.end(), cand_less);
        }
    }

    void into(std::vector<Cand>& out) const
    {
        out = m_heap;
        std::sort(out.begin(), out.end(), cand_less);
    }

    void merge(const TopK& other)
    {
        for (auto& c : other.m_heap)
            push(c.dist, c.order, c.rowid);
    }

private:
    size_t m_k;
    std::vector<Cand> m_heap;
};

} // namespace vec
} // namespace fibjs
