/*
 * VecStore.h
 *
 * Document-partitioned vector storage for vec_index v2.
 *
 * A column owns partitions ("doc_id" values); a partition owns segments
 * (capacity 64..2048 rows). A segment is the unit of scanning, copy-on-write,
 * persistence and caching. Rows are stored SoA: packed float vectors plus a
 * separate {rowid, order} array.
 */

#pragma once

#include "VecKernel.h"
#include <sqlite/sqlite3.h>

#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace fibjs {

#define VEC_SEG_MIN_CAP 64
#define VEC_SEG_MAX_CAP 2048
#define VEC_INDEX_MAX_DIMENSIONS 4096

// A 32-bit process is limited by its user address space, not by physical
// memory: libuv still reports the host's RAM, so cap the cache well below the
// 2-3GB a 32-bit process can map.
#if (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4) \
    || (defined(_WIN32) && !defined(_WIN64)) \
    || defined(_M_IX86) || defined(__i386__) \
    || defined(_M_ARM) || defined(__arm__) \
    || (defined(__riscv) && __riscv_xlen == 32)
#define VEC_CACHE_32BIT 1
#define VEC_CACHE_MAX_LIMIT (256u << 20)
#else
#define VEC_CACHE_MAX_LIMIT ((size_t)-1 / 2)
#endif

#define VEC_CACHE_MIN_LIMIT (64 << 20)
#define VEC_CACHE_RAM_SHARE 2 // default budget: 1/N of total physical memory
#define VEC_CACHE_FALLBACK_LIMIT (1 << 30)

struct VecColumnDef {
    std::string name;
    uint32_t dim = 0; // 0 for key/order columns
    bool is_key = false;
    bool is_order = false;
};

struct VecRowRef {
    uint32_t part;
    uint16_t seg;
    uint16_t slot;
};

struct VecBuf {
    float* data = nullptr;
    size_t bytes = 0;

    ~VecBuf();
};

// A pinned buffer keeps the cache from evicting it while a scan or a column
// read is using it.  The pin count is raised under the cache lock and lowered
// outside of it, so eviction only has to read the atomic counter.
class VecPin {
public:
    VecPin() = default;
    VecPin(std::shared_ptr<VecBuf> buf, std::atomic<int32_t>* pins)
        : m_buf(std::move(buf))
        , m_pins(pins)
    {
    }
    VecPin(VecPin&& other) noexcept
        : m_buf(std::move(other.m_buf))
        , m_pins(other.m_pins)
    {
        other.m_pins = nullptr;
    }
    VecPin& operator=(VecPin&& other) noexcept
    {
        if (this != &other) {
            release();
            m_buf = std::move(other.m_buf);
            m_pins = other.m_pins;
            other.m_pins = nullptr;
        }
        return *this;
    }
    VecPin(const VecPin&) = delete;
    VecPin& operator=(const VecPin&) = delete;
    ~VecPin() { release(); }

    const float* data() const { return m_buf ? m_buf->data : nullptr; }

private:
    void release()
    {
        if (m_buf) {
            if (m_pins)
                m_pins->fetch_sub(1, std::memory_order_relaxed);
            m_buf.reset();
        }
    }

    std::shared_ptr<VecBuf> m_buf;
    std::atomic<int32_t>* m_pins = nullptr;
};

struct VecSegment {
    uint32_t seq = 0;
    uint32_t cap = 0;
    uint32_t count = 0;
    uint32_t stride = 0; // floats per row (rounded up to 16)
    int64_t db_id = 0;
    std::shared_ptr<VecBuf> vecs;
    std::atomic<int32_t> pins { 0 };
    std::atomic<bool> keep { false }; // an older snapshot still reads this db_id's buffer
    int64_t* meta = nullptr; // [rowid, order] per slot
    std::vector<uint32_t> dirty;
    bool write_all = false;
    uint64_t txn = 0;

    ~VecSegment();
};

struct VecPartition {
    int32_t id = -1;
    std::string key;
    uint32_t count = 0;
    std::vector<std::shared_ptr<VecSegment>> segs;
    uint64_t txn = 0;
};

struct VecColumnState {
    std::unordered_map<std::string, int32_t> keys;
    std::vector<std::shared_ptr<VecPartition>> parts;
    std::unordered_map<int64_t, VecRowRef> rows;
};

struct VecSnapshot {
    int64_t version = 0;
    std::string store_id;
    uint64_t txn = 0;
    std::vector<std::shared_ptr<VecColumnState>> cols;
};

struct VecWriteSet {
    std::vector<std::vector<int32_t>> new_parts;
    std::vector<std::vector<int32_t>> del_parts;

    void init(size_t n)
    {
        new_parts.assign(n, std::vector<int32_t>());
        del_parts.assign(n, std::vector<int32_t>());
    }

    void clear()
    {
        for (auto& v : new_parts)
            v.clear();
        for (auto& v : del_parts)
            v.clear();
    }
};

class VecStore : public std::enable_shared_from_this<VecStore> {
public:
    VecStore() = default;
    ~VecStore();

    static std::shared_ptr<VecStore> open(sqlite3* db, const std::string& table,
        const std::vector<VecColumnDef>& cols, bool create, std::string& err);
    static std::shared_ptr<VecStore> lookup(sqlite3* db, const std::string& table);
    static std::string cache_key(sqlite3* db, const std::string& table);

    int read_state(sqlite3* db, int64_t& version, std::string& store_id, std::string& err);

    // Segment vectors are loaded on demand and held in a byte-budget LRU; the
    // returned pin must stay alive while the buffer is being read.  A
    // non-cacheable load keeps the buffer out of the LRU (stream-only), so a
    // scan larger than the budget does not evict the hot working set.
    int ensure_vecs(sqlite3* db, const std::shared_ptr<VecSegment>& seg, VecPin& pin,
        std::string& err, bool cacheable = true);

    // Drops unpinned buffers until the cache fits the budget.  Callers use it
    // between statements so a scan never has to evict a buffer it still reads.
    void trim_cache();
    void set_cache_limit(size_t bytes); // 0 disables the limit
    size_t cache_limit() const;

    // The store only exists once the virtual table has been opened on this
    // connection, but a caller may want to configure or read the budget before
    // the first query.  set_pending_limit() remembers the value per (database,
    // table) and open() applies it, so nothing is cached under the default
    // budget beforehand.
    static size_t clamp_limit(size_t bytes);
    static void set_pending_limit(sqlite3* db, const std::string& table, size_t bytes);
    static bool pending_limit(sqlite3* db, const std::string& table, size_t& bytes);

    // True when the table exists and was created by this module; the store is
    // not needed for that (it only exists once the vtab is opened).
    static bool table_is_vec_index(sqlite3* db, const char* table);

    struct CacheStats {
        int64_t bytes = 0;
        int64_t limit = 0;
        int64_t segments = 0;
        int64_t hits = 0;
        int64_t misses = 0;
        int64_t evictions = 0;
        int64_t loads = 0;
    };
    CacheStats cache_stats() const;

    std::shared_ptr<const VecSnapshot> ensure(sqlite3* db, int64_t version,
        const std::string& store_id, std::string& err);

    std::shared_ptr<VecSnapshot> begin_write(const std::shared_ptr<const VecSnapshot>& base,
        std::string& err);
    void publish(const std::shared_ptr<VecSnapshot>& snap);

    int persist(sqlite3* db, VecSnapshot& snap, VecWriteSet& ws,
        int64_t& new_version, std::string& err);

private:
    int load(sqlite3* db, int64_t version, const std::string& store_id,
        std::shared_ptr<VecSnapshot>& out, std::string& err);
    int create_tables(sqlite3* db, std::string& err);
    int create_state(sqlite3* db, std::string& err);

    struct CacheEntry {
        std::weak_ptr<VecSegment> seg;
        size_t bytes = 0;
        std::list<const VecSegment*>::iterator lru_it;
    };

    void register_locked(const std::shared_ptr<VecSegment>& seg);
    void evict_locked();

    // Default budget for an automatic store.  vectors_bytes is what the LRU may
    // hold, meta_bytes is what the snapshot already holds outside the LRU (row
    // ids and order keys cannot be evicted), so only the remainder of the RAM
    // bound is available for vectors.  Every automatic store in the process
    // shares that bound (see rebalance()), so several tables cannot each claim
    // half of memory; an explicit set_cache_limit() opts out of the split but
    // still counts against the bound.
    void apply_auto_limit(uint64_t vectors_bytes, uint64_t meta_bytes);
    static void rebalance();

    std::string m_table;
    std::vector<VecColumnDef> m_cols;
    std::string m_store_id;

    std::mutex m_mutex;

    mutable std::mutex m_cache_mutex;
    std::list<const VecSegment*> m_lru; // front = most recently used
    std::unordered_map<const VecSegment*, CacheEntry> m_cache;
    size_t m_cache_bytes = 0;
    size_t m_cache_limit = VEC_CACHE_MIN_LIMIT;
    bool m_cache_limit_set = false; // explicit limit; auto-sizing is disabled
    int64_t m_cache_hits = 0;
    int64_t m_cache_misses = 0;
    int64_t m_cache_evictions = 0;
    int64_t m_cache_loads = 0;

    // Auto-budget bookkeeping, guarded by the budget registry lock.
    uint64_t m_auto_want = 0; // vector bytes this store would like to cache
    uint64_t m_auto_meta = 0; // resident metadata counted against the RAM bound
    bool m_budget_registered = false;

    std::shared_ptr<const VecSnapshot> m_cur;
    std::shared_ptr<const VecSnapshot> m_prev;
};

// helpers shared with the vtab
std::shared_ptr<VecSegment> vec_clone_segment(const VecSegment& src);
std::shared_ptr<VecSegment> vec_new_segment(uint32_t seq, uint32_t cap, uint32_t stride);
std::shared_ptr<VecPartition> vec_mutable_part(VecColumnState& c, int32_t pid, uint64_t txn);
std::shared_ptr<VecSegment> vec_mutable_seg(const std::shared_ptr<VecPartition>& p, size_t si,
    uint64_t txn);
int vec_grow_segment(const std::shared_ptr<VecSegment>& seg, uint32_t stride);

} // namespace fibjs
