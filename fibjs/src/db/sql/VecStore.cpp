/*
 * VecStore.cpp
 */

#include "VecStore.h"
#include "ifs/os.h"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_set>

#ifdef _WIN32
#include <malloc.h>
#endif

namespace fibjs {

static std::atomic<uint64_t> g_txn(1);

static std::mutex g_store_mutex;
static std::unordered_map<std::string, std::weak_ptr<VecStore>> g_stores;

// Budgets set before their table was opened, keyed like g_stores; guarded by
// g_store_mutex.
static std::unordered_map<std::string, size_t> g_pending_limits;

// Stores that own an automatic budget.  All of them share one RAM bound, so the
// process cannot exceed it no matter how many tables it opens.  Guarded by
// g_store_mutex (which is also the lock order for taking a store's
// m_cache_mutex from the coordinator).
static std::unordered_set<VecStore*> g_budget_stores;

// Vector payload (what the LRU may hold) and the row ids / order keys that are
// loaded with the snapshot and never evicted, so they have to be counted
// against the RAM bound outside the LRU.
struct SnapshotBytes {
    uint64_t vectors = 0;
    uint64_t meta = 0;
};

static SnapshotBytes snapshot_bytes(const VecSnapshot& snap);

//---------------------------------------------------------------------------
// small sqlite helpers
//---------------------------------------------------------------------------

static int exec_sql(sqlite3* db, const char* sql, std::string& err)
{
    char* e = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &e);

    if (rc != SQLITE_OK) {
        if (e)
            err = e;
        else
            err = sqlite3_errmsg(db);
    }

    if (e)
        sqlite3_free(e);

    return rc;
}

static int prepare_sql(sqlite3* db, const char* sql, sqlite3_stmt** st, std::string& err)
{
    int rc = sqlite3_prepare_v2(db, sql, -1, st, nullptr);

    if (rc != SQLITE_OK) {
        err = sqlite3_errmsg(db);
        *st = nullptr;
    }

    return rc;
}

static int step_done(sqlite3* db, sqlite3_stmt* st, std::string& err)
{
    int rc = sqlite3_step(st);

    if (rc == SQLITE_DONE)
        return SQLITE_OK;

    if (rc == SQLITE_ROW)
        return SQLITE_OK;

    err = sqlite3_errmsg(db);
    return rc;
}

static std::string col_text(sqlite3_stmt* st, int i)
{
    const char* p = (const char*)sqlite3_column_text(st, i);

    if (!p)
        return std::string();

    return std::string(p, sqlite3_column_bytes(st, i));
}

static int query_table_count(sqlite3* db, const char* sql, int64_t& out, std::string& err)
{
    sqlite3_stmt* st = nullptr;
    int rc = prepare_sql(db, sql, &st, err);

    out = 0;
    if (rc != SQLITE_OK)
        return rc;

    rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        out = sqlite3_column_int64(st, 0);
        rc = SQLITE_OK;
    } else if (rc == SQLITE_DONE) {
        rc = SQLITE_OK;
    } else {
        err = sqlite3_errmsg(db);
    }

    sqlite3_finalize(st);
    return rc;
}

static bool table_exists(sqlite3* db, const char* name)
{
    sqlite3_stmt* st = nullptr;
    std::string err;
    bool found = false;

    if (prepare_sql(db, "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?1", &st, err) != SQLITE_OK)
        return false;

    sqlite3_bind_text(st, 1, name, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(st) == SQLITE_ROW)
        found = true;

    sqlite3_finalize(st);
    return found;
}

bool VecStore::table_is_vec_index(sqlite3* db, const char* name)
{
    sqlite3_stmt* st = nullptr;
    std::string err;
    bool found = false;

    if (prepare_sql(db,
            "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?1 "
            "AND sql LIKE '%using vec_index%'",
            &st, err)
        != SQLITE_OK)
        return false;

    sqlite3_bind_text(st, 1, name, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(st) == SQLITE_ROW)
        found = true;

    sqlite3_finalize(st);
    return found;
}

static bool table_has_column(sqlite3* db, const char* table, const char* col)
{
    sqlite3_stmt* st = nullptr;
    std::string err;
    bool found = false;
    std::string sql = std::string("PRAGMA table_info(") + table + ")";

    if (prepare_sql(db, sql.c_str(), &st, err) != SQLITE_OK)
        return false;

    while (sqlite3_step(st) == SQLITE_ROW) {
        std::string name = col_text(st, 1);
        if (name == col) {
            found = true;
            break;
        }
    }

    sqlite3_finalize(st);
    return found;
}

static bool has_legacy_tables(sqlite3* db)
{
    int64_t n = 0;
    std::string err;

    if (query_table_count(db,
            "SELECT count(*) FROM sqlite_master WHERE type='table' AND name IN "
            "('vec_index','vec_index_meta','vec_index_pages')",
            n, err)
        != SQLITE_OK)
        return false;

    return n > 0;
}

static void gen_store_id(char* buf)
{
    unsigned char rnd[16];

    sqlite3_randomness(sizeof(rnd), rnd);
    for (int i = 0; i < 16; i++)
        snprintf(buf + i * 2, 3, "%02x", rnd[i]);
    buf[32] = 0;
}

// Largest vector budget the default policy may use: half of physical memory,
// clamped to the 32-bit address-space cap when applicable.  FIBJS_VEC_RAM_LIMIT
// overrides it (ops use it to cap the vector cache of a container); it is read
// on every rebalance so tests can change it at runtime.
static size_t clamp_cache_limit(size_t bytes);

static size_t default_ram_limit()
{
    const char* e = getenv("FIBJS_VEC_RAM_LIMIT");

    if (e && *e) {
        long long v = atoll(e);

        if (v > 0)
            return clamp_cache_limit((size_t)v);
    }

    int64_t total = 0;

    os_base::totalmem(total);

    if (total <= 0)
        return VEC_CACHE_FALLBACK_LIMIT < VEC_CACHE_MAX_LIMIT ? VEC_CACHE_FALLBACK_LIMIT
                                                              : VEC_CACHE_MAX_LIMIT;

    uint64_t lim = (uint64_t)total / VEC_CACHE_RAM_SHARE;

    if (lim > VEC_CACHE_MAX_LIMIT)
        lim = VEC_CACHE_MAX_LIMIT;

    return (size_t)lim;
}

// An explicit limit cannot escape the address-space cap on 32-bit builds.
static size_t clamp_cache_limit(size_t bytes)
{
#ifdef VEC_CACHE_32BIT
    if (bytes == 0 || bytes > VEC_CACHE_MAX_LIMIT)
        return VEC_CACHE_MAX_LIMIT;
#endif
    return bytes;
}

//---------------------------------------------------------------------------
// segment buffers
//---------------------------------------------------------------------------

// Vector rows are read by the SIMD kernels, which want 64-byte alignment (a
// cache line per row at dim 16 and below).  Windows has no posix_memalign, so
// keep the pair of calls local instead of using them directly.
static void* alloc_aligned(size_t bytes)
{
#ifdef _WIN32
    return _aligned_malloc(bytes, 64);
#else
    void* p = nullptr;

    if (posix_memalign(&p, 64, bytes) != 0)
        return nullptr;

    return p;
#endif
}

static void free_aligned(void* p)
{
#ifdef _WIN32
    _aligned_free(p);
#else
    free(p);
#endif
}

static std::shared_ptr<VecBuf> alloc_vecs(uint32_t cap, uint32_t stride)
{
    size_t bytes = (size_t)cap * stride * sizeof(float);

    if (bytes == 0)
        bytes = 64;

    auto buf = std::make_shared<VecBuf>();
    void* p = alloc_aligned(bytes);

    if (!p)
        return nullptr;

    buf->data = (float*)p;
    buf->bytes = bytes;
    return buf;
}

static int64_t* alloc_meta(uint32_t cap)
{
    return (int64_t*)malloc((size_t)cap * 2 * sizeof(int64_t));
}

VecBuf::~VecBuf()
{
    if (data)
        free_aligned(data);
}

VecSegment::~VecSegment()
{
    if (meta)
        free(meta);
}

std::shared_ptr<VecSegment> vec_clone_segment(const VecSegment& src)
{
    // Writers must pin (load) the source first; cloning a lazily unloaded
    // segment would copy from a null buffer.
    if (src.count && !src.vecs)
        return nullptr;

    auto s = std::make_shared<VecSegment>();

    s->seq = src.seq;
    s->cap = src.cap;
    s->count = src.count;
    s->stride = src.stride;
    s->db_id = src.db_id;
    s->keep = true; // owned by a write transaction; never evict before flush
    s->vecs = alloc_vecs(src.cap, src.stride);
    s->meta = alloc_meta(src.cap);

    if (!s->vecs || !s->meta)
        return nullptr;

    if (src.count) {
        memcpy(s->vecs->data, src.vecs->data, (size_t)src.count * src.stride * sizeof(float));
        memcpy(s->meta, src.meta, (size_t)src.count * 2 * sizeof(int64_t));
    }

    return s;
}

std::shared_ptr<VecSegment> vec_new_segment(uint32_t seq, uint32_t cap, uint32_t stride)
{
    auto s = std::make_shared<VecSegment>();

    s->seq = seq;
    s->cap = cap;
    s->count = 0;
    s->stride = stride;
    s->db_id = 0;
    s->keep = true; // write segment; never evict before flush
    s->vecs = alloc_vecs(cap, stride);
    s->meta = alloc_meta(cap);

    if (!s->vecs || !s->meta)
        return nullptr;

    return s;
}

int vec_grow_segment(const std::shared_ptr<VecSegment>& seg, uint32_t stride)
{
    uint32_t ncap = seg->cap * 2;

    if (ncap > VEC_SEG_MAX_CAP)
        ncap = VEC_SEG_MAX_CAP;
    if (ncap <= seg->cap)
        return SQLITE_NOMEM;

    auto nv = alloc_vecs(ncap, stride);
    int64_t* nm = alloc_meta(ncap);

    if (!nv || !nm)
        return SQLITE_NOMEM;

    if (seg->count) {
        if (!seg->vecs)
            return SQLITE_NOMEM;

        memcpy(nv->data, seg->vecs->data, (size_t)seg->count * stride * sizeof(float));
        memcpy(nm, seg->meta, (size_t)seg->count * 2 * sizeof(int64_t));
    }

    free(seg->meta);
    seg->vecs = nv;
    seg->meta = nm;
    seg->cap = ncap;
    seg->write_all = true;

    return SQLITE_OK;
}

std::shared_ptr<VecPartition> vec_mutable_part(VecColumnState& c, int32_t pid, uint64_t txn)
{
    if (pid < 0 || (size_t)pid >= c.parts.size())
        return nullptr;

    auto p = c.parts[pid];

    if (!p)
        return nullptr;
    if (p->txn == txn)
        return p;

    auto np = std::make_shared<VecPartition>();
    np->id = p->id;
    np->key = p->key;
    np->count = p->count;
    np->segs = p->segs;
    np->txn = txn;
    c.parts[pid] = np;

    return np;
}

std::shared_ptr<VecSegment> vec_mutable_seg(const std::shared_ptr<VecPartition>& p, size_t si,
    uint64_t txn)
{
    if (si >= p->segs.size())
        return nullptr;

    auto s = p->segs[si];

    if (!s)
        return nullptr;
    if (s->txn == txn)
        return s;

    // The old snapshot still reads this db_id; keep its buffer resident even
    // if the cache is over budget.
    s->keep = true;

    auto ns = vec_clone_segment(*s);

    if (!ns)
        return nullptr;

    ns->txn = txn;
    p->segs[si] = ns;
    return ns;
}

//---------------------------------------------------------------------------
// blob helpers
//---------------------------------------------------------------------------

static int blob_read(sqlite3* db, int64_t id, const char* column, void* dst, int bytes,
    std::string& err)
{
    sqlite3_blob* b = nullptr;
    int rc = sqlite3_blob_open(db, "main", "vec_index_seg", column, id, 0, &b);

    if (rc != SQLITE_OK) {
        err = sqlite3_errmsg(db);
        return rc;
    }

    if (sqlite3_blob_bytes(b) < bytes) {
        err = "vec_index segment blob is truncated";
        sqlite3_blob_close(b);
        return SQLITE_CORRUPT;
    }

    rc = sqlite3_blob_read(b, dst, bytes, 0);
    if (rc != SQLITE_OK)
        err = sqlite3_errmsg(db);

    sqlite3_blob_close(b);
    return rc;
}

static int blob_open_write(sqlite3* db, int64_t id, const char* column, sqlite3_blob** out,
    std::string& err)
{
    int rc = sqlite3_blob_open(db, "main", "vec_index_seg", column, id, 1, out);

    if (rc != SQLITE_OK)
        err = sqlite3_errmsg(db);

    return rc;
}

static int write_ranges(sqlite3* db, VecSegment& seg, std::string& err)
{
    if (seg.dirty.empty())
        return SQLITE_OK;

    std::vector<uint32_t>& d = seg.dirty;
    std::sort(d.begin(), d.end());
    d.erase(std::unique(d.begin(), d.end()), d.end());

    sqlite3_blob* bv = nullptr;
    sqlite3_blob* bm = nullptr;
    int rc = blob_open_write(db, seg.db_id, "vecs", &bv, err);

    if (rc != SQLITE_OK)
        return rc;

    rc = blob_open_write(db, seg.db_id, "meta", &bm, err);
    if (rc != SQLITE_OK) {
        sqlite3_blob_close(bv);
        return rc;
    }

    size_t i = 0;
    while (i < d.size() && rc == SQLITE_OK) {
        size_t j = i;
        while (j + 1 < d.size() && d[j + 1] == d[j] + 1)
            j++;

        uint32_t start = d[i];
        uint32_t n = d[j] - start + 1;
        int vbytes = (int)((size_t)n * seg.stride * sizeof(float));

        rc = sqlite3_blob_write(bv, seg.vecs->data + (size_t)start * seg.stride, vbytes,
            (sqlite3_int64)start * seg.stride * sizeof(float));
        if (rc == SQLITE_OK)
            rc = sqlite3_blob_write(bm, seg.meta + (size_t)start * 2, (int)(n * 16),
                (sqlite3_int64)start * 16);

        i = j + 1;
    }

    if (rc != SQLITE_OK)
        err = sqlite3_errmsg(db);

    sqlite3_blob_close(bv);
    sqlite3_blob_close(bm);
    return rc;
}

static int write_all_slots(sqlite3* db, VecSegment& seg, std::string& err)
{
    if (seg.count == 0)
        return SQLITE_OK;

    sqlite3_blob* bv = nullptr;
    sqlite3_blob* bm = nullptr;
    int rc = blob_open_write(db, seg.db_id, "vecs", &bv, err);

    if (rc != SQLITE_OK)
        return rc;

    rc = blob_open_write(db, seg.db_id, "meta", &bm, err);
    if (rc != SQLITE_OK) {
        sqlite3_blob_close(bv);
        return rc;
    }

    rc = sqlite3_blob_write(bv, seg.vecs->data,
        (int)((size_t)seg.count * seg.stride * sizeof(float)), 0);
    if (rc == SQLITE_OK)
        rc = sqlite3_blob_write(bm, seg.meta, (int)((size_t)seg.count * 16), 0);

    if (rc != SQLITE_OK)
        err = sqlite3_errmsg(db);

    sqlite3_blob_close(bv);
    sqlite3_blob_close(bm);
    return rc;
}

static int flush_segment(sqlite3* db, const std::string& table, const std::string& name,
    int32_t part, const VecColumnDef& col, VecSegment& seg, std::string& err)
{
    uint32_t stride = (uint32_t)vec::stride_of(col.dim);
    sqlite3_stmt* st = nullptr;
    int rc;

    if (seg.db_id == 0) {
        rc = prepare_sql(db,
            "INSERT INTO vec_index_seg(tbl,name,part,seq,count,cap,meta,vecs) "
            "VALUES(?1,?2,?3,?4,?5,?6,zeroblob(?7),zeroblob(?8))",
            &st, err);
        if (rc != SQLITE_OK)
            return rc;

        sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(st, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(st, 3, part);
        sqlite3_bind_int64(st, 4, seg.seq);
        sqlite3_bind_int64(st, 5, seg.count);
        sqlite3_bind_int64(st, 6, seg.cap);
        sqlite3_bind_int64(st, 7, (sqlite3_int64)seg.cap * 16);
        sqlite3_bind_int64(st, 8, (sqlite3_int64)seg.cap * stride * 4);

        rc = step_done(db, st, err);
        sqlite3_finalize(st);
        if (rc != SQLITE_OK)
            return rc;

        seg.db_id = sqlite3_last_insert_rowid(db);
        seg.write_all = true;
    } else if (seg.write_all) {
        rc = prepare_sql(db,
            "UPDATE vec_index_seg SET count=?2,cap=?3,meta=zeroblob(?4),vecs=zeroblob(?5) "
            "WHERE id=?1",
            &st, err);
        if (rc != SQLITE_OK)
            return rc;

        sqlite3_bind_int64(st, 1, seg.db_id);
        sqlite3_bind_int64(st, 2, seg.count);
        sqlite3_bind_int64(st, 3, seg.cap);
        sqlite3_bind_int64(st, 4, (sqlite3_int64)seg.cap * 16);
        sqlite3_bind_int64(st, 5, (sqlite3_int64)seg.cap * stride * 4);

        rc = step_done(db, st, err);
        sqlite3_finalize(st);
        if (rc != SQLITE_OK)
            return rc;
    } else {
        rc = prepare_sql(db, "UPDATE vec_index_seg SET count=?2 WHERE id=?1", &st, err);
        if (rc != SQLITE_OK)
            return rc;

        sqlite3_bind_int64(st, 1, seg.db_id);
        sqlite3_bind_int64(st, 2, seg.count);

        rc = step_done(db, st, err);
        sqlite3_finalize(st);
        if (rc != SQLITE_OK)
            return rc;
    }

    if (seg.write_all)
        rc = write_all_slots(db, seg, err);
    else
        rc = write_ranges(db, seg, err);

    if (rc != SQLITE_OK)
        return rc;

    seg.write_all = false;
    seg.keep = false; // persisted: reloading from the blob is now correct
    seg.dirty.clear();
    return SQLITE_OK;
}

//---------------------------------------------------------------------------
// VecStore
//---------------------------------------------------------------------------

std::string VecStore::cache_key(sqlite3* db, const std::string& table)
{
    const char* fn = sqlite3_db_filename(db, "main");
    std::string key;

    if (fn && *fn) {
        key = "file:";
        key += fn;
    } else {
        char buf[64];
        snprintf(buf, sizeof(buf), "mem:%p", (void*)db);
        key = buf;
    }

    key += '|';
    key += table;
    return key;
}

static bool same_columns(const std::vector<VecColumnDef>& a, const std::vector<VecColumnDef>& b)
{
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].name != b[i].name || a[i].dim != b[i].dim || a[i].is_key != b[i].is_key
            || a[i].is_order != b[i].is_order)
            return false;
    }

    return true;
}

int VecStore::create_tables(sqlite3* db, std::string& err)
{
    return exec_sql(db,
        "CREATE TABLE IF NOT EXISTS vec_index_state("
        " tbl TEXT PRIMARY KEY, version INTEGER NOT NULL, store_id TEXT NOT NULL);"
        "CREATE TABLE IF NOT EXISTS vec_index_part("
        " tbl TEXT NOT NULL, name TEXT NOT NULL, part INTEGER NOT NULL,"
        " key TEXT NOT NULL, count INTEGER NOT NULL,"
        " PRIMARY KEY(tbl,name,part), UNIQUE(tbl,name,key));"
        "CREATE TABLE IF NOT EXISTS vec_index_seg("
        " id INTEGER PRIMARY KEY,"
        " tbl TEXT NOT NULL, name TEXT NOT NULL, part INTEGER NOT NULL, seq INTEGER NOT NULL,"
        " count INTEGER NOT NULL, cap INTEGER NOT NULL,"
        " meta BLOB NOT NULL, vecs BLOB NOT NULL);"
        "CREATE UNIQUE INDEX IF NOT EXISTS vec_index_seg_key ON vec_index_seg(tbl,name,part,seq);"
        "",
        err);
}

int VecStore::create_state(sqlite3* db, std::string& err)
{
    char sid[33];

    gen_store_id(sid);
    m_store_id = sid;

    sqlite3_stmt* st = nullptr;
    int rc = prepare_sql(db,
        "INSERT OR REPLACE INTO vec_index_state(tbl,version,store_id) VALUES(?1,0,?2)", &st, err);
    if (rc != SQLITE_OK)
        return rc;

    sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, m_store_id.c_str(), -1, SQLITE_TRANSIENT);

    rc = step_done(db, st, err);
    sqlite3_finalize(st);
    return rc;
}

static int drop_legacy(sqlite3* db, std::string& err)
{
    if (has_legacy_tables(db)) {
        int rc = exec_sql(db,
            "DROP TABLE IF EXISTS vec_index;"
            "DROP TABLE IF EXISTS vec_index_meta;"
            "DROP TABLE IF EXISTS vec_index_pages;",
            err);
        if (rc != SQLITE_OK)
            return rc;
    }

    if (table_exists(db, "vec_index_state") && !table_has_column(db, "vec_index_state", "store_id")) {
        int rc = exec_sql(db, "DROP TABLE IF EXISTS vec_index_state", err);
        if (rc != SQLITE_OK)
            return rc;
    }

    return SQLITE_OK;
}

static int check_legacy(sqlite3* db, std::string& err)
{
    if (has_legacy_tables(db)) {
        err = "vec_index storage format changed; drop and rebuild";
        return SQLITE_ERROR;
    }

    if (table_exists(db, "vec_index_state") && !table_has_column(db, "vec_index_state", "store_id")) {
        err = "vec_index storage format changed; drop and rebuild";
        return SQLITE_ERROR;
    }

    return SQLITE_OK;
}

std::shared_ptr<VecStore> VecStore::open(sqlite3* db, const std::string& table,
    const std::vector<VecColumnDef>& cols, bool create, std::string& err)
{
    std::string key = cache_key(db, table);

    {
        std::lock_guard<std::mutex> lock(g_store_mutex);
        auto it = g_stores.find(key);

        if (it != g_stores.end()) {
            auto s = it->second.lock();

            if (s) {
                if (!same_columns(s->m_cols, cols)) {
                    err = "vec_index column definition changed; drop and rebuild";
                    return nullptr;
                }

                if (create) {
                    int rc = drop_legacy(db, err);

                    if (rc == SQLITE_OK)
                        rc = s->create_tables(db, err);
                    if (rc == SQLITE_OK) {
                        // wipe rows of this table
                        sqlite3_stmt* st = nullptr;
                        std::string tmp;

                        if (prepare_sql(db, "DELETE FROM vec_index_part WHERE tbl=?1", &st, tmp)
                            == SQLITE_OK) {
                            sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);
                            sqlite3_step(st);
                            sqlite3_finalize(st);
                        }
                        if (prepare_sql(db, "DELETE FROM vec_index_seg WHERE tbl=?1", &st, tmp)
                            == SQLITE_OK) {
                            sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);
                            sqlite3_step(st);
                            sqlite3_finalize(st);
                        }
                        rc = s->create_state(db, err);
                    }

                    if (rc != SQLITE_OK)
                        return nullptr;

                    std::lock_guard<std::mutex> slock(s->m_mutex);
                    s->m_cur.reset();
                    s->m_prev.reset();
                } else {
                    int64_t ver = 0;
                    std::string sid;
                    int rc = s->read_state(db, ver, sid, err);

                    if (rc == SQLITE_NOTFOUND) {
                        err = "vec_index is not initialized; drop and rebuild";
                        return nullptr;
                    }
                    if (rc != SQLITE_OK)
                        return nullptr;

                    std::lock_guard<std::mutex> slock(s->m_mutex);
                    if (s->m_store_id != sid) {
                        s->m_cur.reset();
                        s->m_prev.reset();
                        s->m_store_id = sid;
                    }
                }

                return s;
            }
        }
    }

    auto s = std::make_shared<VecStore>();
    s->m_table = table;
    s->m_cols = cols;
    s->m_store_id.clear();

    int rc;

    if (create) {
        rc = drop_legacy(db, err);
        if (rc == SQLITE_OK)
            rc = s->create_tables(db, err);
        if (rc == SQLITE_OK) {
            sqlite3_stmt* st = nullptr;
            std::string tmp;

            if (prepare_sql(db, "DELETE FROM vec_index_part WHERE tbl=?1", &st, tmp) == SQLITE_OK) {
                sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(st);
                sqlite3_finalize(st);
            }
            if (prepare_sql(db, "DELETE FROM vec_index_seg WHERE tbl=?1", &st, tmp) == SQLITE_OK) {
                sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(st);
                sqlite3_finalize(st);
            }
            rc = s->create_state(db, err);
        }
    } else {
        rc = check_legacy(db, err);
        if (rc == SQLITE_OK) {
            int64_t ver = 0;
            std::string sid;
            rc = s->read_state(db, ver, sid, err);
            if (rc == SQLITE_NOTFOUND) {
                err = "vec_index is not initialized; drop and rebuild";
                rc = SQLITE_ERROR;
            } else if (rc == SQLITE_OK) {
                s->m_store_id = sid;
            }
        }
    }

    if (rc != SQLITE_OK)
        return nullptr;

    size_t pending = 0;
    bool has_pending = false;

    {
        std::lock_guard<std::mutex> lock(g_store_mutex);
        g_stores[key] = s;

        // A budget configured before the table was opened (see
        // set_pending_limit) is applied below, once the registry lock is
        // released: set_cache_limit rebalances every store.
        auto pit = g_pending_limits.find(key);

        if (pit != g_pending_limits.end()) {
            pending = pit->second;
            has_pending = true;
            g_pending_limits.erase(pit);
        }
    }

    // Takes effect before anything is cached.
    if (has_pending)
        s->set_cache_limit(pending);

    return s;
}

std::shared_ptr<VecStore> VecStore::lookup(sqlite3* db, const std::string& table)
{
    std::string key = cache_key(db, table);
    std::lock_guard<std::mutex> lock(g_store_mutex);
    auto it = g_stores.find(key);

    if (it == g_stores.end())
        return nullptr;

    return it->second.lock();
}

int VecStore::ensure_vecs(sqlite3* db, const std::shared_ptr<VecSegment>& seg, VecPin& pin,
    std::string& err, bool cacheable)
{
    // Fast path: a cached buffer only needs a pin.
    {
        std::lock_guard<std::mutex> lock(m_cache_mutex);

        if (seg->vecs) {
            m_cache_hits++;
            register_locked(seg);
            seg->pins.fetch_add(1, std::memory_order_relaxed);
            evict_locked();

            pin = VecPin(seg->vecs, &seg->pins);
            return SQLITE_OK;
        }

        m_cache_misses++;
    }

    // Load outside the lock so parallel workers do not serialize their blob
    // reads on the cache.
    auto buf = alloc_vecs(seg->cap, seg->stride);

    if (!buf) {
        err = "vec_index: out of memory";
        return SQLITE_NOMEM;
    }

    if (seg->count && seg->db_id) {
        int rc = blob_read(db, seg->db_id, "vecs", buf->data,
            (int)((size_t)seg->count * seg->stride * sizeof(float)), err);

        if (rc != SQLITE_OK)
            return rc;
    }

    std::lock_guard<std::mutex> lock(m_cache_mutex);

    m_cache_loads++;

    if (!seg->vecs && cacheable) {
        seg->vecs = buf;
        register_locked(seg);
    }
    // else another worker cached the same segment while we were reading the
    // blob, or this is a stream-only load; the pin below owns `buf` on its own.

    seg->pins.fetch_add(1, std::memory_order_relaxed);
    evict_locked();

    pin = VecPin(seg->vecs ? seg->vecs : buf, &seg->pins);
    return SQLITE_OK;
}

void VecStore::register_locked(const std::shared_ptr<VecSegment>& seg)
{
    auto it = m_cache.find(seg.get());

    if (it != m_cache.end()) {
        if (it->second.bytes != seg->vecs->bytes) {
            size_t old = it->second.bytes;

            it->second.bytes = seg->vecs->bytes;
            m_cache_bytes = m_cache_bytes - old + it->second.bytes;
        }

        // Recency only matters while a budget is enforced; skipping the list
        // update keeps full scans from churning the LRU.
        if (m_cache_limit != 0 && m_cache_bytes > m_cache_limit) {
            m_lru.erase(it->second.lru_it);
            m_lru.push_front(seg.get());
            it->second.lru_it = m_lru.begin();
        }
        return;
    }

    m_lru.push_front(seg.get());

    CacheEntry e;
    e.seg = seg;
    e.bytes = seg->vecs->bytes;
    e.lru_it = m_lru.begin();
    m_cache.emplace(seg.get(), e);
    m_cache_bytes += e.bytes;
}

void VecStore::evict_locked()
{
    if (m_cache_limit == 0 || m_cache_bytes <= m_cache_limit)
        return;

    std::vector<const VecSegment*> drop;

    for (auto pit = m_lru.rbegin(); pit != m_lru.rend() && m_cache_bytes > m_cache_limit; ++pit) {
        auto it = m_cache.find(*pit);

        if (it == m_cache.end())
            continue;

        auto seg = it->second.seg.lock();

        if (!seg) {
            m_cache_bytes -= it->second.bytes;
            drop.push_back(*pit);
            continue;
        }

        if (seg->pins.load(std::memory_order_relaxed) > 0
            || seg->keep.load(std::memory_order_relaxed))
            continue;

        seg->vecs.reset();
        m_cache_bytes -= it->second.bytes;
        m_cache_evictions++;
        drop.push_back(*pit);
    }

    for (auto key : drop) {
        auto it = m_cache.find(key);

        if (it != m_cache.end()) {
            m_lru.erase(it->second.lru_it);
            m_cache.erase(it);
        }
    }
}

void VecStore::trim_cache()
{
    std::lock_guard<std::mutex> lock(m_cache_mutex);

    evict_locked();
}

void VecStore::set_cache_limit(size_t bytes)
{
    {
        std::lock_guard<std::mutex> lock(m_cache_mutex);

        m_cache_limit = clamp_cache_limit(bytes);
        m_cache_limit_set = true;
        evict_locked();
    }

    // Explicit budgets are honoured verbatim but still count against the
    // process-wide bound, so the automatic stores get what is left.
    {
        std::lock_guard<std::mutex> lock(g_store_mutex);

        g_budget_stores.insert(this);
        m_budget_registered = true;
    }

    rebalance();
}

// Default budget: the whole index when it fits, otherwise what is left of the
// RAM bound after the always-resident metadata that all automatic stores
// already hold.  It only ever grows for a given index size, so deleting rows
// does not evict a warm cache.
void VecStore::apply_auto_limit(uint64_t vectors_bytes, uint64_t meta_bytes)
{
    {
        std::lock_guard<std::mutex> lock(g_store_mutex);

        if (m_cache_limit_set) {
            // An explicit limit wins; rebalancing may still need to know about
            // this store, but its own budget never changes.
            return;
        }

        if (m_budget_registered && m_auto_want == vectors_bytes && m_auto_meta == meta_bytes)
            return;

        m_auto_want = vectors_bytes;
        m_auto_meta = meta_bytes;
        g_budget_stores.insert(this);
        m_budget_registered = true;
    }

    rebalance();
}

// Splits the process-wide vector budget between every automatic store:
//   * explicit budgets are honoured and subtracted first,
//   * resident metadata is subtracted (it cannot be evicted),
//   * what remains is shared proportionally to what each store asked for.
// Called when a store's index size changes, so it is not on the per-query path.
void VecStore::rebalance()
{
    struct Share {
        VecStore* store;
        uint64_t want; // read under the registry lock below
    };

    std::vector<Share> shares;
    uint64_t fixed_sum = 0, meta_sum = 0, want_sum = 0, head, floor;
    size_t ram = default_ram_limit();
    size_t n_auto = 0;

    {
        std::lock_guard<std::mutex> lock(g_store_mutex);

        shares.reserve(g_budget_stores.size());

        for (auto* s : g_budget_stores) {
            std::lock_guard<std::mutex> l(s->m_cache_mutex);

            if (s->m_cache_limit_set)
                fixed_sum += s->m_cache_limit;
            else {
                meta_sum += s->m_auto_meta;
                want_sum += s->m_auto_want;
                n_auto++;
                shares.push_back({ s, s->m_auto_want });
            }
        }
    }

    uint64_t used = fixed_sum + meta_sum;

    head = ram > used ? (uint64_t)ram - used : 0;

    // Smallest budget worth handing out: enough to keep a tiny index reportable
    // as "at least this big", but never more than an equal share of what is
    // left, so the floors themselves cannot break the bound.  A budget of 0
    // means "unlimited" here, so an exhausted bound still has to be > 0.
    floor = n_auto ? head / n_auto : head;

    if (floor > VEC_CACHE_MIN_LIMIT)
        floor = VEC_CACHE_MIN_LIMIT;
    if (floor == 0)
        floor = 1;

    for (auto& sh : shares) {
        VecStore* s = sh.store;
        std::lock_guard<std::mutex> l(s->m_cache_mutex);

        if (s->m_cache_limit_set)
            continue;

        uint64_t want = sh.want;
        uint64_t lim;

        if (want_sum <= head)
            lim = want; // nobody is competing: the whole index may be cached
        else
            // Share the remainder in proportion to the request.
            lim = want_sum ? (uint64_t)((long double)want * head / want_sum) : head;

        if (lim > want)
            lim = want;
        if (lim < floor)
            lim = floor;

        s->m_cache_limit = clamp_cache_limit((size_t)lim);
        s->evict_locked();
    }
}

VecStore::~VecStore()
{
    bool was_registered = false;

    {
        std::lock_guard<std::mutex> lock(g_store_mutex);

        if (m_budget_registered) {
            g_budget_stores.erase(this);
            m_budget_registered = false;
            was_registered = true;
        }
    }

    // The others may now take the freed share.
    if (was_registered)
        rebalance();
}

size_t VecStore::cache_limit() const
{
    std::lock_guard<std::mutex> lock(m_cache_mutex);
    return m_cache_limit;
}

size_t VecStore::clamp_limit(size_t bytes)
{
    return clamp_cache_limit(bytes);
}

void VecStore::set_pending_limit(sqlite3* db, const std::string& table, size_t bytes)
{
    std::string key = cache_key(db, table);
    std::lock_guard<std::mutex> lock(g_store_mutex);

    g_pending_limits[key] = clamp_cache_limit(bytes);
}

bool VecStore::pending_limit(sqlite3* db, const std::string& table, size_t& bytes)
{
    std::string key = cache_key(db, table);
    std::lock_guard<std::mutex> lock(g_store_mutex);
    auto it = g_pending_limits.find(key);

    if (it == g_pending_limits.end())
        return false;

    bytes = it->second;
    return true;
}

VecStore::CacheStats VecStore::cache_stats() const
{
    std::lock_guard<std::mutex> lock(m_cache_mutex);

    CacheStats s;

    s.bytes = (int64_t)m_cache_bytes;
    s.limit = (int64_t)m_cache_limit;
    s.segments = (int64_t)m_cache.size();
    s.hits = m_cache_hits;
    s.misses = m_cache_misses;
    s.evictions = m_cache_evictions;
    s.loads = m_cache_loads;
    return s;
}

int VecStore::read_state(sqlite3* db, int64_t& version, std::string& store_id, std::string& err)
{
    sqlite3_stmt* st = nullptr;
    int rc = prepare_sql(db, "SELECT version,store_id FROM vec_index_state WHERE tbl=?1", &st, err);

    if (rc != SQLITE_OK)
        return rc;

    sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        version = sqlite3_column_int64(st, 0);
        store_id = col_text(st, 1);
        rc = SQLITE_OK;
    } else if (rc == SQLITE_DONE) {
        rc = SQLITE_NOTFOUND;
    } else {
        err = sqlite3_errmsg(db);
    }

    sqlite3_finalize(st);
    return rc;
}

int VecStore::load(sqlite3* db, int64_t version, const std::string& store_id,
    std::shared_ptr<VecSnapshot>& out, std::string& err)
{
    int64_t v = version;
    std::string sid = store_id;

    for (int attempt = 0; attempt < 3; attempt++) {
        auto snap = std::make_shared<VecSnapshot>();

        snap->version = v;
        snap->store_id = sid;
        snap->txn = 0;
        snap->cols.resize(m_cols.size());

        for (size_t c = 0; c < m_cols.size(); c++) {
            const VecColumnDef& col = m_cols[c];

            if (col.dim == 0)
                continue;

            uint32_t stride = (uint32_t)vec::stride_of(col.dim);
            auto cs = std::make_shared<VecColumnState>();

            // partitions
            {
                sqlite3_stmt* st = nullptr;
                int rc = prepare_sql(db,
                    "SELECT part,key,count FROM vec_index_part WHERE tbl=?1 AND name=?2 "
                    "ORDER BY part",
                    &st, err);
                if (rc != SQLITE_OK)
                    return rc;

                sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(st, 2, col.name.c_str(), -1, SQLITE_TRANSIENT);

                while ((rc = sqlite3_step(st)) == SQLITE_ROW) {
                    int32_t pid = sqlite3_column_int(st, 0);

                    if (pid < 0 || pid > 100000000) {
                        sqlite3_finalize(st);
                        err = "vec_index partition id out of range";
                        return SQLITE_CORRUPT;
                    }

                    if ((size_t)pid >= cs->parts.size())
                        cs->parts.resize((size_t)pid + 1);

                    auto p = std::make_shared<VecPartition>();
                    p->id = pid;
                    p->key = col_text(st, 1);
                    p->count = (uint32_t)sqlite3_column_int64(st, 2);
                    p->txn = 0;
                    cs->parts[pid] = p;

                    if (!cs->keys.insert(std::make_pair(p->key, pid)).second) {
                        sqlite3_finalize(st);
                        err = "vec_index duplicate partition key";
                        return SQLITE_CORRUPT;
                    }
                }

                if (rc != SQLITE_DONE) {
                    err = sqlite3_errmsg(db);
                    sqlite3_finalize(st);
                    return rc;
                }

                sqlite3_finalize(st);
            }

            // reserve up-front: the row map is the dominant load cost
            {
                uint64_t total_rows = 0;

                for (auto& p : cs->parts)
                    if (p)
                        total_rows += p->count;

                cs->rows.reserve((size_t)total_rows);
                cs->keys.reserve(cs->parts.size());
            }

            // segments
            {
                sqlite3_stmt* st = nullptr;
                int rc = prepare_sql(db,
                    "SELECT id,part,seq,count,cap,meta FROM vec_index_seg WHERE tbl=?1 AND name=?2 "
                    "ORDER BY part,seq",
                    &st, err);
                if (rc != SQLITE_OK)
                    return rc;

                sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(st, 2, col.name.c_str(), -1, SQLITE_TRANSIENT);

                while ((rc = sqlite3_step(st)) == SQLITE_ROW) {
                    int64_t db_id = sqlite3_column_int64(st, 0);
                    int32_t pid = sqlite3_column_int(st, 1);
                    uint32_t seq = (uint32_t)sqlite3_column_int64(st, 2);
                    uint32_t count = (uint32_t)sqlite3_column_int64(st, 3);
                    uint32_t cap = (uint32_t)sqlite3_column_int64(st, 4);

                    if (pid < 0 || (size_t)pid >= cs->parts.size() || !cs->parts[pid]
                        || cap == 0 || cap > VEC_SEG_MAX_CAP || count > cap) {
                        sqlite3_finalize(st);
                        err = "vec_index segment metadata is invalid";
                        return SQLITE_CORRUPT;
                    }

                    auto seg = std::make_shared<VecSegment>();
                    seg->seq = seq;
                    seg->cap = cap;
                    seg->count = count;
                    seg->stride = stride;
                    seg->db_id = db_id;
                    seg->txn = 0;
                    seg->vecs = nullptr;
                    seg->meta = alloc_meta(cap);

                    if (!seg->meta) {
                        sqlite3_finalize(st);
                        err = "vec_index out of memory";
                        return SQLITE_NOMEM;
                    }

                    if (count) {
                        const void* mb = sqlite3_column_blob(st, 5);
                        int mbytes = sqlite3_column_bytes(st, 5);

                        if (!mb || mbytes < (int)((size_t)count * 16)) {
                            sqlite3_finalize(st);
                            err = "vec_index segment meta is truncated";
                            return SQLITE_CORRUPT;
                        }

                        memcpy(seg->meta, mb, (size_t)count * 16);
                    }

                    auto& part = cs->parts[pid];
                    uint16_t seg_index = (uint16_t)part->segs.size();

                    if (part->segs.size() >= 65535) {
                        sqlite3_finalize(st);
                        err = "vec_index too many segments";
                        return SQLITE_CORRUPT;
                    }

                    part->segs.push_back(seg);

                    for (uint32_t slot = 0; slot < count; slot++) {
                        int64_t rowid = seg->meta[(size_t)slot * 2];
                        VecRowRef ref { (uint32_t)pid, seg_index, (uint16_t)slot };

                        if (!cs->rows.insert(std::make_pair(rowid, ref)).second) {
                            sqlite3_finalize(st);
                            err = "vec_index duplicate rowid";
                            return SQLITE_CORRUPT;
                        }
                    }
                }

                if (rc != SQLITE_DONE) {
                    err = sqlite3_errmsg(db);
                    sqlite3_finalize(st);
                    return rc;
                }

                sqlite3_finalize(st);
            }

            for (auto& p : cs->parts) {
                if (!p)
                    continue;

                uint64_t sum = 0;
                for (auto& s : p->segs)
                    sum += s->count;

                if (sum != p->count) {
                    err = "vec_index partition count mismatch";
                    return SQLITE_CORRUPT;
                }
            }

            snap->cols[c] = cs;
        }

        int64_t v2 = 0;
        std::string sid2;
        int rc = read_state(db, v2, sid2, err);

        if (rc != SQLITE_OK)
            return rc;

        if (v2 == v && sid2 == sid) {
            auto bytes = snapshot_bytes(*snap);
            apply_auto_limit(bytes.vectors, bytes.meta);
            out = snap;
            return SQLITE_OK;
        }

        v = v2;
        sid = sid2;
    }

    err = "vec_index is being modified by another connection";
    return SQLITE_BUSY;
}

std::shared_ptr<const VecSnapshot> VecStore::ensure(sqlite3* db, int64_t version,
    const std::string& store_id, std::string& err)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_store_id != store_id) {
            m_cur.reset();
            m_prev.reset();
            m_store_id = store_id;
        }

        if (m_cur && m_cur->version == version)
            return m_cur;

        if (m_prev && m_prev->version == version)
            return m_prev;
    }

    std::shared_ptr<VecSnapshot> snap;
    int rc = load(db, version, store_id, snap, err);

    if (rc != SQLITE_OK)
        return nullptr;

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_cur && m_cur->version == snap->version)
        return m_cur;

    if (!m_cur || snap->version >= m_cur->version) {
        m_prev = m_cur;
        m_cur = snap;
        m_store_id = snap->store_id;
    } else {
        m_prev = snap;
    }

    return snap;
}

std::shared_ptr<VecSnapshot> VecStore::begin_write(const std::shared_ptr<const VecSnapshot>& base,
    std::string& err)
{
    if (!base) {
        err = "vec_index snapshot is not loaded";
        return nullptr;
    }

    auto ws = std::make_shared<VecSnapshot>();

    ws->version = base->version;
    ws->store_id = base->store_id;
    ws->txn = ++g_txn;
    ws->cols.resize(base->cols.size());

    for (size_t i = 0; i < base->cols.size(); i++) {
        if (!base->cols[i])
            continue;

        auto d = std::make_shared<VecColumnState>();
        d->keys = base->cols[i]->keys;
        d->parts = base->cols[i]->parts;
        d->rows = base->cols[i]->rows;
        ws->cols[i] = d;
    }

    return ws;
}

void VecStore::publish(const std::shared_ptr<VecSnapshot>& snap)
{
    if (!snap)
        return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_cur && snap->version < m_cur->version)
            return;

        m_prev = m_cur;
        m_cur = snap;
        m_store_id = snap->store_id;
    }

    // The first load may have seen an empty index (create + insert); publish
    // is where the budget learns about the rows the transaction added.
    auto bytes = snapshot_bytes(*snap);
    apply_auto_limit(bytes.vectors, bytes.meta);
}

int VecStore::persist(sqlite3* db, VecSnapshot& snap, VecWriteSet& ws, int64_t& new_version,
    std::string& err)
{
    bool wrote = false;

    for (size_t c = 0; c < m_cols.size(); c++) {
        const VecColumnDef& col = m_cols[c];

        if (col.dim == 0 || !snap.cols[c])
            continue;

        const std::string& name = col.name;
        auto& cs = *snap.cols[c];

        for (int32_t pid : ws.del_parts[c]) {
            sqlite3_stmt* st = nullptr;
            int rc = prepare_sql(db, "DELETE FROM vec_index_seg WHERE tbl=?1 AND name=?2 AND part=?3",
                &st, err);
            if (rc != SQLITE_OK)
                return rc;

            sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 2, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(st, 3, pid);
            rc = step_done(db, st, err);
            sqlite3_finalize(st);
            if (rc != SQLITE_OK)
                return rc;

            rc = prepare_sql(db, "DELETE FROM vec_index_part WHERE tbl=?1 AND name=?2 AND part=?3",
                &st, err);
            if (rc != SQLITE_OK)
                return rc;

            sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 2, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(st, 3, pid);
            rc = step_done(db, st, err);
            sqlite3_finalize(st);
            if (rc != SQLITE_OK)
                return rc;

            wrote = true;
        }

        std::unordered_set<int32_t> newset(ws.new_parts[c].begin(), ws.new_parts[c].end());

        for (auto& pp : cs.parts) {
            if (!pp || pp->txn != snap.txn)
                continue;

            sqlite3_stmt* st = nullptr;
            int rc;

            if (newset.count(pp->id)) {
                rc = prepare_sql(db,
                    "INSERT OR REPLACE INTO vec_index_part(tbl,name,part,key,count) "
                    "VALUES(?1,?2,?3,?4,?5)",
                    &st, err);
                if (rc != SQLITE_OK)
                    return rc;

                sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(st, 2, name.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(st, 3, pp->id);
                sqlite3_bind_text(st, 4, pp->key.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int64(st, 5, pp->count);
            } else {
                rc = prepare_sql(db,
                    "UPDATE vec_index_part SET count=?4 WHERE tbl=?1 AND name=?2 AND part=?3", &st,
                    err);
                if (rc != SQLITE_OK)
                    return rc;

                sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(st, 2, name.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(st, 3, pp->id);
                sqlite3_bind_int64(st, 4, pp->count);
            }

            rc = step_done(db, st, err);
            sqlite3_finalize(st);
            if (rc != SQLITE_OK)
                return rc;

            wrote = true;

            for (auto& sp : pp->segs) {
                if (!sp || sp->txn != snap.txn)
                    continue;

                rc = flush_segment(db, m_table, name, pp->id, col, *sp, err);
                if (rc != SQLITE_OK)
                    return rc;
            }
        }
    }

    if (!wrote) {
        new_version = snap.version;
        return SQLITE_OK;
    }

    {
        sqlite3_stmt* st = nullptr;
        int rc = prepare_sql(db, "UPDATE vec_index_state SET version=version+1 WHERE tbl=?1", &st,
            err);
        if (rc != SQLITE_OK)
            return rc;

        sqlite3_bind_text(st, 1, m_table.c_str(), -1, SQLITE_TRANSIENT);
        rc = step_done(db, st, err);
        sqlite3_finalize(st);
        if (rc != SQLITE_OK)
            return rc;
    }

    int64_t ver = 0;
    std::string sid;
    int rc = read_state(db, ver, sid, err);

    if (rc != SQLITE_OK)
        return rc;

    new_version = ver;
    return SQLITE_OK;
}

static SnapshotBytes snapshot_bytes(const VecSnapshot& snap)
{
    SnapshotBytes b;

    for (auto& c : snap.cols)
        if (c)
            for (auto& p : c->parts)
                if (p)
                    for (auto& g : p->segs)
                        if (g) {
                            b.vectors += (uint64_t)g->cap * g->stride * sizeof(float);
                            b.meta += (uint64_t)g->cap * 2 * sizeof(int64_t);
                        }

    return b;
}

} // namespace fibjs
