/*
 * SQLite_vec.cpp
 *
 * vec_index v2: document-partitioned exact vector search.
 */

#include "object.h"
#include "ifs/os.h"
#include "SQLite.h"
#include "VecKernel.h"
#include "VecStore.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <unordered_set>

#include <nlohmann/json.hpp>

namespace fibjs {

#define SQLITE_VEC_VERSION "v2.0.0"
#define VEC_SEARCH_FUNCTION SQLITE_INDEX_CONSTRAINT_FUNCTION

// idxNum bits
#define IDX_SEARCH 0x01
#define IDX_KEY_EQ 0x02
#define IDX_KEY_IN 0x04
#define IDX_LIMIT 0x08
#define IDX_LOOKUP 0x10
#define IDX_COL_SHIFT 8

//---------------------------------------------------------------------------
// small helpers
//---------------------------------------------------------------------------

static std::string trim_copy(const std::string& s)
{
    size_t b = 0, e = s.size();

    while (b < e && isspace((unsigned char)s[b]))
        b++;
    while (e > b && isspace((unsigned char)s[e - 1]))
        e--;

    return s.substr(b, e - b);
}

static std::vector<std::string> split_ws(const std::string& s)
{
    std::vector<std::string> out;
    size_t i = 0;

    while (i < s.size()) {
        while (i < s.size() && isspace((unsigned char)s[i]))
            i++;
        size_t b = i;
        while (i < s.size() && !isspace((unsigned char)s[i]))
            i++;
        if (i > b)
            out.push_back(s.substr(b, i - b));
    }

    return out;
}

static std::string lower_ascii(const std::string& s)
{
    std::string r = s;
    for (auto& c : r)
        c = (char)tolower((unsigned char)c);
    return r;
}

static bool parse_constructor(int argc, const char* const* argv, std::vector<VecColumnDef>& cols,
    std::string& err)
{
    std::unordered_set<std::string> names;
    int vec_count = 0, key_count = 0, order_count = 0;

    for (int i = 3; i < argc; i++) {
        std::string arg = trim_copy(argv[i]);

        if (arg.empty()) {
            err = "vec_index: empty column definition";
            return false;
        }

        VecColumnDef c;
        size_t lp = arg.find('(');

        if (lp != std::string::npos) {
            size_t rp = arg.find(')', lp);

            if (rp == std::string::npos) {
                err = "vec_index: invalid column definition '" + arg + "'";
                return false;
            }

            c.name = trim_copy(arg.substr(0, lp));
            c.dim = (uint32_t)atoi(arg.substr(lp + 1, rp - lp - 1).c_str());

            if (c.dim == 0 || c.dim > VEC_INDEX_MAX_DIMENSIONS) {
                err = "vec_index: dimension must be between 1 and "
                    + std::to_string(VEC_INDEX_MAX_DIMENSIONS);
                return false;
            }

            vec_count++;
        } else {
            std::vector<std::string> toks = split_ws(arg);

            if (toks.size() != 2
                || (lower_ascii(toks[1]) != "key" && lower_ascii(toks[1]) != "order")) {
                err = "vec_index: invalid column definition '" + arg + "'";
                return false;
            }

            c.name = toks[0];

            if (lower_ascii(toks[1]) == "key") {
                c.is_key = true;
                key_count++;
            } else {
                c.is_order = true;
                order_count++;
            }
        }

        if (c.name.empty()) {
            err = "vec_index: empty column name";
            return false;
        }

        if (!names.insert(lower_ascii(c.name)).second) {
            err = "vec_index: duplicate column '" + c.name + "'";
            return false;
        }

        cols.push_back(c);
    }

    if (vec_count == 0) {
        err = "vec_index: at least one vector column is required";
        return false;
    }

    if (key_count > 1 || order_count > 1) {
        err = "vec_index: at most one key column and one order column are allowed";
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// vector parsing
//---------------------------------------------------------------------------

static bool json_vector(const char* txt, int bytes, size_t dim, std::vector<float>& out,
    std::string& err)
{
    try {
        nlohmann::json j = nlohmann::json::parse(std::string(txt, bytes));

        out = j.get<std::vector<float>>();
    } catch (const std::exception&) {
        err = "vec_index: invalid vector";
        return false;
    }

    if (out.empty()) {
        err = "vec_index: vector must not be empty";
        return false;
    }

    if (dim > 0) {
        if (out.size() > dim) {
            err = "vec_index: vector is longer than the column dimension";
            return false;
        }

        out.resize(dim, 0.0f);
    }

    return true;
}

// parse a stored/query vector value; returns false with err set on failure.
static bool parse_vector_value(sqlite3_value* v, size_t dim, std::vector<float>& out,
    std::string& err)
{
    int type = sqlite3_value_type(v);

    if (type == SQLITE_BLOB) {
        const float* p = (const float*)sqlite3_value_blob(v);
        int bytes = sqlite3_value_bytes(v);

        if (!p || bytes == 0 || bytes % 4 != 0) {
            err = "vec_index: invalid vector";
            return false;
        }

        size_t n = (size_t)bytes / 4;

        if (dim > 0 && n > dim) {
            err = "vec_index: vector is longer than the column dimension";
            return false;
        }

        out.assign(dim ? dim : n, 0.0f);
        memcpy(out.data(), p, n * sizeof(float));

        for (size_t i = 0; i < n; i++) {
            if (!std::isfinite(out[i])) {
                err = "vec_index: vector contains NaN or Infinity";
                return false;
            }
        }
    } else if (type == SQLITE_TEXT) {
        const char* txt = (const char*)sqlite3_value_text(v);
        int bytes = sqlite3_value_bytes(v);

        if (!txt || !json_vector(txt, bytes, dim, out, err))
            return false;
    } else {
        err = "vec_index: vector value must be a blob or a JSON array";
        return false;
    }

    return true;
}

static void normalize_dim(std::vector<float>& v, size_t dim)
{
    if (v.size() < dim)
        v.resize(dim, 0.0f);
    vec::normalize(v.data(), dim);
}

// parse the vec_search argument: blob or JSON text with an optional ':k' suffix.
static bool parse_query_vector(sqlite3_value* v, size_t dim, std::vector<float>& out, int64_t& k,
    std::string& err)
{
    int type = sqlite3_value_type(v);

    k = 0;

    if (type == SQLITE_NULL) {
        err = "vec_index: query vector is required";
        return false;
    }

    if (type == SQLITE_TEXT) {
        const char* txt = (const char*)sqlite3_value_text(v);
        int bytes = sqlite3_value_bytes(v);

        if (!txt) {
            err = "vec_index: query vector is required";
            return false;
        }

        std::string text(txt, bytes);
        size_t pos = text.find(':');

        if (pos != std::string::npos) {
            k = atoll(text.c_str() + pos + 1);
            text.resize(pos);
        }

        if (!json_vector(text.c_str(), (int)text.size(), dim, out, err))
            return false;

        normalize_dim(out, dim);
    } else if (type == SQLITE_BLOB) {
        const float* p = (const float*)sqlite3_value_blob(v);
        int bytes = sqlite3_value_bytes(v);

        if (!p || bytes == 0 || bytes % 4 != 0) {
            err = "vec_index: invalid query vector";
            return false;
        }

        size_t n = (size_t)bytes / 4;

        if (n > dim) {
            err = "vec_index: query vector is longer than the column dimension";
            return false;
        }

        out.assign(dim, 0.0f);
        memcpy(out.data(), p, n * sizeof(float));

        for (size_t i = 0; i < n; i++) {
            if (!std::isfinite(out[i])) {
                err = "vec_index: query vector contains NaN or Infinity";
                return false;
            }
        }
    } else {
        err = "vec_index: query vector must be a blob or a JSON array";
        return false;
    }

    // Blob queries are taken as-is: callers pass a vector normalized with
    // vec_normalize() so distances stay bit-identical with vec_distance_ip().
    // JSON queries are normalized here for convenience.
    return true;
}

//---------------------------------------------------------------------------
// VecIndex
//---------------------------------------------------------------------------

class VecIndex : public sqlite3_vtab {
public:
    VecIndex()
    {
        memset((sqlite3_vtab*)this, 0, sizeof(sqlite3_vtab));
    }

    void set_err(const std::string& e)
    {
        if (zErrMsg)
            sqlite3_free(zErrMsg);
        zErrMsg = sqlite3_mprintf("%s", e.c_str());
    }

    int acquire(std::shared_ptr<const VecSnapshot>& out, bool force);
    int ensure_working();

    bool row_exists(int64_t rowid)
    {
        int j = vec_cols.empty() ? -1 : vec_cols[0];

        if (j < 0 || !working->cols[j])
            return false;

        return working->cols[j]->rows.find(rowid) != working->cols[j]->rows.end();
    }

    int append_row(int j, int64_t rowid, const std::vector<float>& vec, const std::string& key,
        int64_t order, std::string& err);
    int remove_row(int j, int64_t rowid, std::string& err);

    int apply_insert(int64_t rowid, const std::vector<std::vector<float>>& vecs,
        const std::string& key, int64_t order, std::string& err);
    int apply_delete(int64_t rowid, std::string& err);
    int apply_update(int64_t old_rowid, int64_t new_rowid,
        const std::vector<std::vector<float>>& vecs, const std::vector<bool>& has_vec,
        const std::string& key, int64_t order, std::string& err);

public:
    sqlite3* db = nullptr;
    std::string table;
    std::vector<VecColumnDef> cols;
    int key_col = -1;
    int order_col = -1;
    std::vector<int> vec_cols;

    std::shared_ptr<VecStore> store;
    std::shared_ptr<const VecSnapshot> snap;
    std::shared_ptr<VecSnapshot> working;
    VecWriteSet wset;
    bool dirty = false;
    bool synced = false;
    unsigned int last_dv = 0;
    bool dv_valid = false;
};

int VecIndex::acquire(std::shared_ptr<const VecSnapshot>& out, bool force)
{
    if (working) {
        out = working;
        return SQLITE_OK;
    }

    unsigned int dv = 0;
    bool has_dv = sqlite3_file_control(db, "main", SQLITE_FCNTL_DATA_VERSION, &dv) == SQLITE_OK;

    if (!force && snap && has_dv && dv_valid && dv == last_dv) {
        out = snap;
        return SQLITE_OK;
    }

    int64_t ver = 0;
    std::string sid, err;
    int rc = store->read_state(db, ver, sid, err);

    if (rc != SQLITE_OK) {
        if (rc == SQLITE_NOTFOUND)
            err = "vec_index is not initialized; drop and rebuild";
        set_err(err);
        return SQLITE_ERROR;
    }

    auto s = store->ensure(db, ver, sid, err);

    if (!s) {
        set_err(err);
        return SQLITE_ERROR;
    }

    snap = s;
    last_dv = dv;
    dv_valid = has_dv;
    out = s;
    return SQLITE_OK;
}

int VecIndex::ensure_working()
{
    if (working)
        return SQLITE_OK;

    std::shared_ptr<const VecSnapshot> base;
    int rc = acquire(base, true);

    if (rc != SQLITE_OK)
        return rc;

    std::string err;
    working = store->begin_write(base, err);

    if (!working) {
        set_err(err);
        return SQLITE_ERROR;
    }

    wset.init(cols.size());
    dirty = false;
    synced = false;
    return SQLITE_OK;
}

int VecIndex::append_row(int j, int64_t rowid, const std::vector<float>& vec,
    const std::string& key, int64_t order, std::string& err)
{
    uint32_t dim = cols[j].dim;
    uint32_t stride = (uint32_t)vec::stride_of(dim);
    auto& cs = *working->cols[j];

    std::shared_ptr<VecPartition> part;
    auto it = cs.keys.find(key);

    if (it != cs.keys.end()) {
        part = vec_mutable_part(cs, it->second, working->txn);
        if (!part) {
            err = "vec_index: partition lookup failed";
            return SQLITE_ERROR;
        }
    } else {
        int32_t pid = (int32_t)cs.parts.size();
        part = std::make_shared<VecPartition>();
        part->id = pid;
        part->key = key;
        part->txn = working->txn;
        cs.parts.push_back(part);
        cs.keys[key] = pid;
        wset.new_parts[j].push_back(pid);
    }

    std::shared_ptr<VecSegment> seg;
    VecPin pin;

    if (part->segs.empty() || part->segs.back()->count == VEC_SEG_MAX_CAP) {
        uint32_t seq = part->segs.empty() ? 0 : part->segs.back()->seq + 1;
        seg = vec_new_segment(seq, VEC_SEG_MIN_CAP, stride);
        if (!seg) {
            err = "vec_index: out of memory";
            return SQLITE_NOMEM;
        }
        seg->txn = working->txn;
        part->segs.push_back(seg);
    } else {
        size_t si = part->segs.size() - 1;
        int rc0 = store->ensure_vecs(db, part->segs[si], pin, err);

        if (rc0 != SQLITE_OK)
            return rc0;

        seg = vec_mutable_seg(part, si, working->txn);
        if (!seg) {
            err = "vec_index: out of memory";
            return SQLITE_NOMEM;
        }
        if (seg->count == seg->cap) {
            int rc = vec_grow_segment(seg, stride);
            if (rc != SQLITE_OK) {
                err = "vec_index: out of memory";
                return SQLITE_NOMEM;
            }
        }
    }

    uint32_t slot = seg->count++;
    memcpy(seg->vecs->data + (size_t)slot * seg->stride, vec.data(),
        (size_t)dim * sizeof(float));
    seg->meta[(size_t)slot * 2] = rowid;
    seg->meta[(size_t)slot * 2 + 1] = order;
    seg->dirty.push_back(slot);

    VecRowRef ref;
    ref.part = (uint32_t)part->id;
    ref.seg = (uint16_t)(part->segs.size() - 1);
    ref.slot = (uint16_t)slot;
    cs.rows[rowid] = ref;

    part->count++;
    return SQLITE_OK;
}

int VecIndex::remove_row(int j, int64_t rowid, std::string& err)
{
    uint32_t dim = cols[j].dim;
    auto& cs = *working->cols[j];
    auto it = cs.rows.find(rowid);

    if (it == cs.rows.end())
        return SQLITE_OK;

    VecRowRef ref = it->second;
    auto part = vec_mutable_part(cs, ref.part, working->txn);
    VecPin pin;

    if (part && ref.seg < part->segs.size() && part->segs[ref.seg]) {
        int rc0 = store->ensure_vecs(db, part->segs[ref.seg], pin, err);

        if (rc0 != SQLITE_OK)
            return rc0;
    }

    auto seg = part ? vec_mutable_seg(part, ref.seg, working->txn) : nullptr;

    if (!part || !seg) {
        err = "vec_index: inconsistent row reference";
        return SQLITE_CORRUPT;
    }

    uint32_t last = seg->count - 1;

    if (ref.slot != last) {
        memcpy(seg->vecs->data + (size_t)ref.slot * seg->stride,
            seg->vecs->data + (size_t)last * seg->stride, (size_t)dim * sizeof(float));

        int64_t moved = seg->meta[(size_t)last * 2];
        seg->meta[(size_t)ref.slot * 2] = moved;
        seg->meta[(size_t)ref.slot * 2 + 1] = seg->meta[(size_t)last * 2 + 1];

        auto mit = cs.rows.find(moved);
        if (mit != cs.rows.end()) {
            mit->second.seg = ref.seg;
            mit->second.slot = ref.slot;
        }

        seg->dirty.push_back(ref.slot);
    }

    seg->count--;
    part->count--;
    cs.rows.erase(it);

    if (part->count == 0) {
        cs.keys.erase(part->key);
        cs.parts[part->id] = nullptr;
        wset.del_parts[j].push_back(part->id);
    }

    return SQLITE_OK;
}

int VecIndex::apply_insert(int64_t rowid, const std::vector<std::vector<float>>& vecs,
    const std::string& key, int64_t order, std::string& err)
{
    for (size_t k = 0; k < vec_cols.size(); k++) {
        int rc = append_row(vec_cols[k], rowid, vecs[k], key, order, err);

        if (rc != SQLITE_OK)
            return rc;
    }

    dirty = true;
    return SQLITE_OK;
}

int VecIndex::apply_delete(int64_t rowid, std::string& err)
{
    bool removed = false;

    for (size_t k = 0; k < vec_cols.size(); k++) {
        int j = vec_cols[k];

        if (working->cols[j]->rows.find(rowid) != working->cols[j]->rows.end())
            removed = true;

        int rc = remove_row(j, rowid, err);

        if (rc != SQLITE_OK)
            return rc;
    }

    if (removed)
        dirty = true;

    return SQLITE_OK;
}

int VecIndex::apply_update(int64_t old_rowid, int64_t new_rowid,
    const std::vector<std::vector<float>>& vecs, const std::vector<bool>& has_vec,
    const std::string& key, int64_t order, std::string& err)
{
    for (size_t k = 0; k < vec_cols.size(); k++) {
        int j = vec_cols[k];
        uint32_t dim = cols[j].dim;
        auto& cs = *working->cols[j];
        auto it = cs.rows.find(old_rowid);

        if (it == cs.rows.end()) {
            if (has_vec[k]) {
                int rc = append_row(j, new_rowid, vecs[k], key, order, err);
                if (rc != SQLITE_OK)
                    return rc;
            }
            continue;
        }

        VecRowRef ref = it->second;
        auto part = cs.parts[ref.part];
        auto seg = part ? part->segs[ref.seg] : nullptr;

        if (!part || !seg) {
            err = "vec_index: inconsistent row reference";
            return SQLITE_CORRUPT;
        }

        bool only_order = (part->key == key && old_rowid == new_rowid && !has_vec[k]);

        if (only_order) {
            VecPin pin;
            int rc0 = store->ensure_vecs(db, seg, pin, err);

            if (rc0 != SQLITE_OK)
                return rc0;

            auto mseg = vec_mutable_seg(part, ref.seg, working->txn);

            if (!mseg) {
                err = "vec_index: out of memory";
                return SQLITE_NOMEM;
            }

            if (mseg->meta[(size_t)ref.slot * 2 + 1] != order) {
                mseg->meta[(size_t)ref.slot * 2 + 1] = order;
                mseg->dirty.push_back(ref.slot);
            }
            continue;
        }

        // effective vector before the row moves
        std::vector<float> vec;

        if (has_vec[k]) {
            vec = vecs[k];
        } else {
            // The segment may not have been scanned yet (vectors are loaded
            // lazily), so pin the buffer before reading it.
            VecPin pin;
            int rc0 = store->ensure_vecs(db, seg, pin, err);

            if (rc0 != SQLITE_OK)
                return rc0;

            vec.assign(dim, 0.0f);
            memcpy(vec.data(), pin.data() + (size_t)ref.slot * seg->stride,
                (size_t)dim * sizeof(float));
        }

        int rc = remove_row(j, old_rowid, err);
        if (rc != SQLITE_OK)
            return rc;

        rc = append_row(j, new_rowid, vec, key, order, err);
        if (rc != SQLITE_OK)
            return rc;
    }

    dirty = true;
    return SQLITE_OK;
}

//---------------------------------------------------------------------------
// cursor
//---------------------------------------------------------------------------

enum VecCursorMode {
    VM_SEARCH,
    VM_LOOKUP,
    VM_SCAN
};

class VecCursor : public sqlite3_vtab_cursor {
public:
    explicit VecCursor(VecIndex* p)
    {
        memset((sqlite3_vtab_cursor*)this, 0, sizeof(sqlite3_vtab_cursor));
        pVtab = (sqlite3_vtab*)p;
        idx = p;
    }

    bool advance();

public:
    VecIndex* idx = nullptr;
    VecCursorMode mode = VM_SCAN;
    int col = -1;
    int scan_col = -1;
    std::vector<vec::Cand> results;
    size_t i = 0;
    std::shared_ptr<const VecSnapshot> snap;
    std::vector<int32_t> parts;
    size_t pi = 0;
    size_t si = 0;
    uint32_t slot = 0;
    int64_t cur_rowid = 0;
    bool cur_valid = false;
};

bool VecCursor::advance()
{
    if (scan_col < 0 || !snap) {
        cur_valid = false;
        return false;
    }

    auto cs = snap->cols[scan_col];

    if (!cs) {
        cur_valid = false;
        return false;
    }

    while (pi < parts.size()) {
        int32_t pid = parts[pi];

        if (pid < 0 || (size_t)pid >= cs->parts.size() || !cs->parts[pid]) {
            pi++;
            si = 0;
            slot = 0;
            continue;
        }

        auto& p = cs->parts[pid];

        if (si >= p->segs.size()) {
            pi++;
            si = 0;
            slot = 0;
            continue;
        }

        auto& s = p->segs[si];

        if (!s || slot >= s->count) {
            si++;
            slot = 0;
            continue;
        }

        cur_rowid = s->meta[(size_t)slot * 2];
        cur_valid = true;
        return true;
    }

    cur_valid = false;
    return false;
}

//---------------------------------------------------------------------------
// vtab glue
//---------------------------------------------------------------------------

static int vec_init(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr, bool isCreate)
{
    sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);

    std::vector<VecColumnDef> cols;
    std::string err;

    if (!parse_constructor(argc, argv, cols, err)) {
        *pzErr = sqlite3_mprintf("%s", err.c_str());
        return SQLITE_ERROR;
    }

    sqlite3_str* str = sqlite3_str_new(nullptr);
    sqlite3_str_appendall(str, "CREATE TABLE x(distance hidden");

    for (auto& c : cols)
        sqlite3_str_appendf(str, ", \"%w\"", c.name.c_str());

    sqlite3_str_appendall(str, ")");
    const char* query = sqlite3_str_finish(str);
    int rc = sqlite3_declare_vtab(db, query);
    sqlite3_free((void*)query);

    if (rc != SQLITE_OK)
        return rc;

    VecIndex* p = new VecIndex();

    p->db = db;
    p->table = argv[2];
    p->cols = cols;

    for (size_t i = 0; i < cols.size(); i++) {
        if (cols[i].is_key)
            p->key_col = (int)i;
        else if (cols[i].is_order)
            p->order_col = (int)i;
        else
            p->vec_cols.push_back((int)i);
    }

    p->store = VecStore::open(db, p->table, cols, isCreate, err);

    if (!p->store) {
        delete p;
        *pzErr = sqlite3_mprintf("%s", err.c_str());
        return SQLITE_ERROR;
    }

    *ppVtab = p;
    return SQLITE_OK;
}

static int vecIndexCreate(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr)
{
    return vec_init(db, pAux, argc, argv, ppVtab, pzErr, true);
}

static int vecIndexConnect(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr)
{
    return vec_init(db, pAux, argc, argv, ppVtab, pzErr, false);
}

static bool can_consume_order(VecIndex* p, sqlite3_index_info* info)
{
    if (info->nOrderBy == 0)
        return true;

    int i = 0;

    if (info->aOrderBy[i].iColumn != 0 || info->aOrderBy[i].desc)
        return false;
    i++;

    if (i < info->nOrderBy) {
        if (p->order_col < 0 || info->aOrderBy[i].iColumn != 1 + p->order_col
            || info->aOrderBy[i].desc)
            return false;
        i++;
    }

    if (i < info->nOrderBy) {
        if (info->aOrderBy[i].iColumn != -1 || info->aOrderBy[i].desc)
            return false;
        i++;
    }

    return i == info->nOrderBy;
}

static int vecIndexBestIndex(sqlite3_vtab* tab, sqlite3_index_info* pIdxInfo)
{
    VecIndex* p = (VecIndex*)tab;
    int search_i = -1, search_col = -1;
    int key_i = -1, key_in = 0;
    int limit_i = -1;
    int rowid_i = -1;
    bool rowid_post = false;

    for (int i = 0; i < pIdxInfo->nConstraint; i++) {
        const auto& c = pIdxInfo->aConstraint[i];

        if (c.op == SQLITE_INDEX_CONSTRAINT_OFFSET) {
            p->set_err("vec_index: OFFSET is not supported");
            return SQLITE_ERROR;
        }

        if (!c.usable) {
            if (c.iColumn >= 0) {
                p->set_err("vec_index: unsupported constraint on a vec_index column");
                return SQLITE_ERROR;
            }
            continue;
        }

        if (c.op == SQLITE_INDEX_CONSTRAINT_LIMIT) {
            limit_i = i;
            continue;
        }

        if (c.iColumn == 0) {
            p->set_err("vec_index: distance is not filterable");
            return SQLITE_ERROR;
        }

        if (c.iColumn < 0) {
            bool is_in = sqlite3_vtab_in(pIdxInfo, i, -1) != 0;

            if (c.op == SQLITE_INDEX_CONSTRAINT_EQ && !is_in) {
                if (rowid_i >= 0) {
                    p->set_err("vec_index: duplicate rowid constraint");
                    return SQLITE_ERROR;
                }
                rowid_i = i;
                continue;
            }

            // Other rowid filters are left to SQLite (post-filter); a full
            // or partition scan returns the complete row set.
            rowid_post = true;
            continue;
        }

        int j = c.iColumn - 1;

        if (j < 0 || j >= (int)p->cols.size()) {
            p->set_err("vec_index: unknown column");
            return SQLITE_ERROR;
        }

        if (c.op >= SQLITE_INDEX_CONSTRAINT_FUNCTION) {
            if (c.op != VEC_SEARCH_FUNCTION || p->cols[j].dim == 0) {
                p->set_err("vec_index: unsupported function constraint");
                return SQLITE_ERROR;
            }
            if (search_i >= 0) {
                p->set_err("vec_index: only one vec_search is allowed");
                return SQLITE_ERROR;
            }
            search_i = i;
            search_col = j;
            continue;
        }

        if (j == p->key_col && c.op == SQLITE_INDEX_CONSTRAINT_EQ) {
            if (key_i >= 0) {
                p->set_err("vec_index: only one partition key constraint is allowed");
                return SQLITE_ERROR;
            }
            key_i = i;
            key_in = sqlite3_vtab_in(pIdxInfo, i, -1) ? 1 : 0;
            continue;
        }

        p->set_err("vec_index: unsupported constraint on '" + p->cols[j].name + "'");
        return SQLITE_ERROR;
    }

    if (search_i < 0 && key_i < 0 && rowid_i < 0) {
        pIdxInfo->idxNum = 0;
        pIdxInfo->estimatedCost = 1e12;
        pIdxInfo->estimatedRows = 1000000;
        return SQLITE_OK;
    }

    if (rowid_post && search_i >= 0) {
        // A rowid post-filter on a top-k result would silently under-return.
        p->set_err("vec_index: rowid filters cannot be combined with vec_search");
        return SQLITE_ERROR;
    }

    if (rowid_i >= 0) {
        if (search_i >= 0 || key_i >= 0) {
            p->set_err("vec_index: rowid lookups cannot be combined with other constraints");
            return SQLITE_ERROR;
        }

        pIdxInfo->aConstraintUsage[rowid_i].argvIndex = 1;
        pIdxInfo->aConstraintUsage[rowid_i].omit = 1;
        pIdxInfo->idxNum = IDX_LOOKUP | ((p->vec_cols[0] + 1) << IDX_COL_SHIFT);
        pIdxInfo->estimatedCost = 1.0;
        pIdxInfo->estimatedRows = 1;
        pIdxInfo->idxFlags = SQLITE_INDEX_SCAN_UNIQUE;
        return SQLITE_OK;
    }

    int argv = 1;
    int bits = 0;

    if (search_i >= 0) {
        pIdxInfo->aConstraintUsage[search_i].argvIndex = argv++;
        pIdxInfo->aConstraintUsage[search_i].omit = 1;
        bits |= IDX_SEARCH;
    }

    if (key_i >= 0) {
        pIdxInfo->aConstraintUsage[key_i].argvIndex = argv++;
        pIdxInfo->aConstraintUsage[key_i].omit = 1;

        if (key_in) {
            sqlite3_vtab_in(pIdxInfo, key_i, 1);
            bits |= IDX_KEY_IN;
        } else {
            bits |= IDX_KEY_EQ;
        }
    }

    if (search_i >= 0 && limit_i >= 0) {
        pIdxInfo->aConstraintUsage[limit_i].argvIndex = argv++;
        pIdxInfo->aConstraintUsage[limit_i].omit = 1;
        bits |= IDX_LIMIT;
    }

    if (search_i >= 0) {
        bits |= (search_col + 1) << IDX_COL_SHIFT;

        if (can_consume_order(p, pIdxInfo))
            pIdxInfo->orderByConsumed = 1;

        pIdxInfo->estimatedCost = 300.0;
        pIdxInfo->estimatedRows = 10;
        pIdxInfo->idxNum = bits;
        return SQLITE_OK;
    }

    pIdxInfo->idxNum = bits;
    pIdxInfo->estimatedCost = 1e6;
    pIdxInfo->estimatedRows = 10000;
    return SQLITE_OK;
}

static int vecIndexDisconnect(sqlite3_vtab* pVtab)
{
    delete (VecIndex*)pVtab;
    return SQLITE_OK;
}

static int vecIndexDestroy(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;

    char* sql = sqlite3_mprintf(
        "DELETE FROM vec_index_seg WHERE tbl=%Q;"
        "DELETE FROM vec_index_part WHERE tbl=%Q;"
        "DELETE FROM vec_index_state WHERE tbl=%Q;"
        "DROP TABLE IF EXISTS vec_index;"
        "DROP TABLE IF EXISTS vec_index_meta;"
        "DROP TABLE IF EXISTS vec_index_pages;",
        p->table.c_str(), p->table.c_str(), p->table.c_str());
    sqlite3_exec(p->db, sql, nullptr, nullptr, nullptr);
    sqlite3_free(sql);

    delete p;
    return SQLITE_OK;
}

static int vecIndexOpen(sqlite3_vtab* pVtab, sqlite3_vtab_cursor** ppCursor)
{
    *ppCursor = new VecCursor((VecIndex*)pVtab);
    return SQLITE_OK;
}

static int vecIndexClose(sqlite3_vtab_cursor* cur)
{
    VecCursor* c = (VecCursor*)cur;

    if (c->idx && c->idx->store)
        c->idx->store->trim_cache();

    delete c;
    return SQLITE_OK;
}

// Parallel scan tuning (measured on an 8-CPU M-series laptop, dim 128/768):
//
//   scanned   inline   split
//     3.2MB   134us    134us  (gate not reached)
//     6.5MB   270us    270us  (break even)
//      13MB   595us    478us  (1.25x)
//      26MB   872us    585us  (1.50x)
//
// A query scans count(rows) * stride * 4 bytes, so the gate is a byte count
// and not a row count: the same 6.5MB is ~13k rows at dim 128 but only ~2k
// rows at dim 768.  Below the gate the dispatch latency (~20-40us per task on
// a loaded machine) costs more than the scan itself.
static const size_t VEC_PARALLEL_BYTES = 8u << 20; // gate; ~13MB -> 4 chunks
static const size_t VEC_PARALLEL_CHUNK = VEC_PARALLEL_BYTES / 2;

struct VecScanChunk {
    size_t begin = 0, end = 0; // half open range of segment indices
    size_t pin_base = 0; // first pin in the round's pin vector
    vec::TopK* topk = nullptr;
    exlib::Event ev;
};

// Scans one loaded segment buffer.
static void vec_scan_segment(const float* vecs, const int64_t* meta, uint32_t count,
    const float* q, uint32_t dim, uint32_t stride, vec::TopK& topk)
{
    uint32_t s = 0;

    for (; s + 4 <= count; s += 4) {
        const float* v = vecs + (size_t)s * stride;
        const int64_t* m = meta + (size_t)s * 2;
        float d[4];

        vec::ip_distance4(q, v, v + stride, v + 2 * stride, v + 3 * stride, dim, d);

        topk.push(d[0], m[1], m[0]);
        topk.push(d[1], m[3], m[2]);
        topk.push(d[2], m[5], m[4]);
        topk.push(d[3], m[7], m[6]);
    }

    for (; s < count; s++)
        topk.push(vec::ip_distance(q, vecs + (size_t)s * stride, dim),
            meta[(size_t)s * 2 + 1], meta[(size_t)s * 2]);
}

// Workers only read pinned memory: the connection is opened with
// SQLITE_OPEN_NOMUTEX, so blob I/O must stay on the thread that owns it.
static void vec_scan_pinned(const std::vector<std::shared_ptr<VecSegment>>& segs,
    const VecScanChunk& c, const std::vector<VecPin>& pins, const float* q,
    uint32_t dim, uint32_t stride)
{
    for (size_t i = c.begin; i < c.end; i++)
        vec_scan_segment(pins[c.pin_base + (i - c.begin)].data(), segs[i]->meta,
            segs[i]->count, q, dim, stride, *c.topk);
}

static int vec_scan_inline(VecStore* store, sqlite3* db,
    const std::vector<std::shared_ptr<VecSegment>>& segs, const float* q,
    uint32_t dim, uint32_t stride, bool cacheable, vec::TopK& topk, std::string& err)
{
    for (auto& sp : segs) {
        VecPin pin;
        int rc = store->ensure_vecs(db, sp, pin, err, cacheable);

        if (rc != SQLITE_OK)
            return rc;

        vec_scan_segment(pin.data(), sp->meta, sp->count, q, dim, stride, topk);
    }

    return SQLITE_OK;
}

struct VecRange {
    size_t begin = 0, end = 0;
};

// Pins every segment of ranges [r0, r0+n) on the calling thread.  A round is
// bounded by the number of workers and VEC_PARALLEL_CHUNK, so peak memory is
// O(workers * chunk) instead of O(table), while the buffers are released as
// soon as the round has been scanned.
static int pin_ranges(VecStore* store, sqlite3* db,
    const std::vector<std::shared_ptr<VecSegment>>& segs,
    const std::vector<VecRange>& ranges, size_t r0, size_t n,
    std::vector<VecPin>& pins, bool cacheable, std::string& err)
{
    pins.clear();

    for (size_t j = 0; j < n; j++)
        for (size_t i = ranges[r0 + j].begin; i < ranges[r0 + j].end; i++) {
            VecPin pin;
            int rc = store->ensure_vecs(db, segs[i], pin, err, cacheable);

            if (rc != SQLITE_OK)
                return rc;

            pins.push_back(std::move(pin));
        }

    return SQLITE_OK;
}

static int vec_scan(VecStore* store, sqlite3* db,
    const std::vector<std::shared_ptr<VecSegment>>& segs, const float* q,
    uint32_t dim, uint32_t stride, size_t k, bool cacheable,
    std::vector<vec::Cand>& out, std::string& err)
{
    size_t total = 0;

    for (auto& s : segs)
        total += s->count;

    // Splitting pays once the scanned bytes dwarf the dispatch cost; see the
    // table above VEC_PARALLEL_BYTES.
    size_t bytes = total * (size_t)stride * sizeof(float);

    if (bytes < VEC_PARALLEL_BYTES || segs.size() < 2) {
        vec::TopK topk(k);

        int rc = vec_scan_inline(store, db, segs, q, dim, stride, cacheable, topk, err);

        if (rc != SQLITE_OK)
            return rc;

        topk.into(out);
        return SQLITE_OK;
    }

    int32_t cpus = 1;
    os_base::cpuNumbers(cpus);

    if (cpus < 1)
        cpus = 1;

    // ~VEC_PARALLEL_CHUNK bytes per task.
    std::vector<VecRange> ranges;
    size_t begin = 0;
    size_t acc = 0;

    for (size_t i = 0; i < segs.size(); i++) {
        acc += (size_t)segs[i]->count * stride * sizeof(float);

        if (acc >= VEC_PARALLEL_CHUNK || i + 1 == segs.size()) {
            ranges.push_back({ begin, i + 1 });
            begin = i + 1;
            acc = 0;
        }
    }

    size_t workers = std::min<size_t>((size_t)cpus, ranges.size());
    std::vector<VecPin> pins, next_pins;
    vec::TopK merged(k);
    size_t r = 0;

    int rc = pin_ranges(store, db, segs, ranges, 0, std::min(workers, ranges.size()), pins,
        cacheable, err);

    if (rc != SQLITE_OK)
        return rc;

    while (r < ranges.size()) {
        size_t n = std::min(workers, ranges.size() - r);
        std::vector<VecScanChunk> plan(n);
        size_t base = 0;

        for (size_t j = 0; j < n; j++) {
            plan[j].begin = ranges[r + j].begin;
            plan[j].end = ranges[r + j].end;
            plan[j].pin_base = base;
            plan[j].topk = new vec::TopK(k);
            base += plan[j].end - plan[j].begin;
        }

        // One async(longsync) task per chunk: acPool gives each task an OS
        // thread, the caller takes the first chunk itself and waits for the
        // rest.
        for (size_t j = 1; j < n; j++)
            async([&plan, j, &segs, &pins, q, dim, stride]() {
                vec_scan_pinned(segs, plan[j], pins, q, dim, stride);
                plan[j].ev.set();
            }, result_t::longsync());

        vec_scan_pinned(segs, plan[0], pins, q, dim, stride);

        // Load the next round on this thread while the workers finish the
        // current one; the pins double the resident window but keep blob I/O
        // on the connection's owner thread.
        size_t nr = r + n;
        int prc = SQLITE_OK;

        if (nr < ranges.size())
            prc = pin_ranges(store, db, segs, ranges, nr,
                std::min(workers, ranges.size() - nr), next_pins, cacheable, err);

        for (size_t j = 1; j < n; j++)
            plan[j].ev.wait();

        for (size_t j = 0; j < n; j++) {
            merged.merge(*plan[j].topk);
            delete plan[j].topk;
        }

        pins.clear();
        pins.swap(next_pins);
        next_pins.clear();

        if (prc != SQLITE_OK)
            return prc;

        r = nr;
    }

    merged.into(out);
    return SQLITE_OK;
}

static int resolve_parts(VecIndex* p, const std::shared_ptr<const VecColumnState>& cs,
    sqlite3_value* keyval, int key_in, std::vector<int32_t>& parts, int64_t& total)
{
    std::unordered_set<int32_t> seen;

    total = 0;
    parts.clear();

    auto add = [&](int32_t pid) {
        if (pid < 0 || (size_t)pid >= cs->parts.size() || !cs->parts[pid])
            return;
        if (!seen.insert(pid).second)
            return;
        parts.push_back(pid);
        total += cs->parts[pid]->count;
    };

    if (!keyval) {
        for (size_t i = 0; i < cs->parts.size(); i++)
            if (cs->parts[i])
                add((int32_t)i);
        return SQLITE_OK;
    }

    if (key_in) {
        sqlite3_value* val = nullptr;
        int rc = sqlite3_vtab_in_first(keyval, &val);

        while (rc == SQLITE_OK && val) {
            const char* t = (const char*)sqlite3_value_text(val);

            if (t) {
                auto it = cs->keys.find(std::string(t, sqlite3_value_bytes(val)));
                if (it != cs->keys.end())
                    add(it->second);
            }

            rc = sqlite3_vtab_in_next(keyval, &val);
        }

        if (rc != SQLITE_OK && rc != SQLITE_DONE)
            return rc;

        return SQLITE_OK;
    }

    const char* t = (const char*)sqlite3_value_text(keyval);

    if (t) {
        auto it = cs->keys.find(std::string(t, sqlite3_value_bytes(keyval)));
        if (it != cs->keys.end())
            add(it->second);
    }

    return SQLITE_OK;
}

static int vecIndexFilter(sqlite3_vtab_cursor* pVtabCursor, int idxNum, const char* idxStr,
    int argc, sqlite3_value** argv)
{
    VecCursor* cur = (VecCursor*)pVtabCursor;
    VecIndex* p = (VecIndex*)cur->pVtab;
    std::string err;

    std::shared_ptr<const VecSnapshot> snap;
    int rc = p->acquire(snap, false);

    if (rc != SQLITE_OK)
        return rc;

    cur->snap = snap;
    cur->i = 0;
    cur->pi = 0;
    cur->si = 0;
    cur->slot = 0;
    cur->cur_valid = false;
    cur->results.clear();
    cur->parts.clear();
    cur->scan_col = p->vec_cols.empty() ? -1 : p->vec_cols[0];
    cur->col = ((idxNum >> IDX_COL_SHIFT) & 0xff) - 1;

    int argi = 0;
    int bits = idxNum & 0xff;

    if (bits & IDX_LOOKUP) {
        cur->mode = VM_LOOKUP;

        if (cur->scan_col < 0)
            return SQLITE_OK;

        int64_t rowid = sqlite3_value_int64(argv[argi]);
        auto cs = snap->cols[cur->scan_col];

        if (cs && cs->rows.find(rowid) != cs->rows.end()) {
            cur->cur_rowid = rowid;
            cur->cur_valid = true;
        }

        return SQLITE_OK;
    }

    if (bits & IDX_SEARCH) {
        cur->mode = VM_SEARCH;

        if (cur->col < 0 || cur->col >= (int)p->cols.size() || p->cols[cur->col].dim == 0) {
            p->set_err("vec_index: invalid search column");
            return SQLITE_ERROR;
        }

        uint32_t dim = p->cols[cur->col].dim;
        std::vector<float> q;
        int64_t k = 0;

        if (!parse_query_vector(argv[argi++], dim, q, k, err)) {
            p->set_err(err);
            return SQLITE_ERROR;
        }

        sqlite3_value* keyval = nullptr;
        int key_in = 0;

        if (bits & (IDX_KEY_EQ | IDX_KEY_IN)) {
            keyval = argv[argi++];
            key_in = (bits & IDX_KEY_IN) ? 1 : 0;
        }

        if (bits & IDX_LIMIT) {
            int64_t l = sqlite3_value_int64(argv[argi++]);
            if (l > 0)
                k = l;
        }

        if (k <= 0)
            k = 100;

        auto cs = snap->cols[cur->col];

        if (!cs)
            return SQLITE_OK;

        std::vector<int32_t> parts;
        int64_t total = 0;

        rc = resolve_parts(p, cs, keyval, key_in, parts, total);
        if (rc != SQLITE_OK) {
            p->set_err("vec_index: failed to resolve the partition filter");
            return rc;
        }

        if (total == 0)
            return SQLITE_OK;

        if (k > total)
            k = total;

        uint32_t stride = (uint32_t)vec::stride_of(dim);
        std::vector<std::shared_ptr<VecSegment>> segs;
        uint64_t scan_bytes = 0;

        for (int32_t pid : parts) {
            auto& part = cs->parts[pid];

            for (auto& seg : part->segs) {
                segs.push_back(seg);
                // Cache accounting uses the allocated capacity, so compare the
                // scan budget against capacity bytes too (count bytes can be
                // 25-40% lower because of segment padding).
                scan_bytes += (uint64_t)seg->cap * stride * sizeof(float);
            }
        }

        // Enforce the byte budget between statements; buffers pinned by the
        // scan itself are skipped.  A scan larger than the budget streams
        // without entering the cache, so it cannot evict the hot set.
        p->store->trim_cache();

        size_t cache_limit = p->store->cache_limit();
        bool cacheable = cache_limit == 0 || scan_bytes <= cache_limit;

        rc = vec_scan(p->store.get(), p->db, segs, q.data(), dim, stride, (size_t)k,
            cacheable, cur->results, err);

        if (rc != SQLITE_OK) {
            p->set_err(err);
            return rc;
        }

        return SQLITE_OK;
    }

    // partition scan / full scan
    cur->mode = VM_SCAN;

    if (cur->scan_col < 0)
        return SQLITE_OK;

    auto cs = snap->cols[cur->scan_col];

    if (!cs)
        return SQLITE_OK;

    sqlite3_value* keyval = nullptr;
    int key_in = 0;

    if (bits & (IDX_KEY_EQ | IDX_KEY_IN)) {
        keyval = argv[argi++];
        key_in = (bits & IDX_KEY_IN) ? 1 : 0;
    }

    int64_t total = 0;
    rc = resolve_parts(p, cs, keyval, key_in, cur->parts, total);

    if (rc != SQLITE_OK) {
        p->set_err("vec_index: failed to resolve the partition filter");
        return rc;
    }

    cur->cur_valid = cur->advance();
    return SQLITE_OK;
}

static int vecIndexNext(sqlite3_vtab_cursor* pVtabCursor)
{
    VecCursor* cur = (VecCursor*)pVtabCursor;

    switch (cur->mode) {
    case VM_SEARCH:
        cur->i++;
        break;
    case VM_LOOKUP:
        cur->cur_valid = false;
        break;
    case VM_SCAN:
        cur->slot++;
        cur->cur_valid = cur->advance();
        break;
    }

    return SQLITE_OK;
}

static int vecIndexEof(sqlite3_vtab_cursor* pVtabCursor)
{
    VecCursor* cur = (VecCursor*)pVtabCursor;

    switch (cur->mode) {
    case VM_SEARCH:
        return cur->i >= cur->results.size();
    case VM_LOOKUP:
    case VM_SCAN:
        return !cur->cur_valid;
    }

    return 1;
}

static int vecIndexColumn(sqlite3_vtab_cursor* pVtabCursor, sqlite3_context* ctx, int i)
{
    VecCursor* cur = (VecCursor*)pVtabCursor;
    VecIndex* p = (VecIndex*)cur->pVtab;

    if (i == 0) {
        if (cur->mode == VM_SEARCH && cur->i < cur->results.size())
            sqlite3_result_double(ctx, cur->results[cur->i].dist);
        return SQLITE_OK;
    }

    int j = i - 1;

    if (j < 0 || j >= (int)p->cols.size() || !cur->snap)
        return SQLITE_OK;

    int64_t rowid = (cur->mode == VM_SEARCH) ? cur->results[cur->i].rowid : cur->cur_rowid;
    const VecColumnDef& col = p->cols[j];

    // Key/order are row attributes, not per-column storage: resolve them
    // through the scan column (the first vector column).
    int state_col = (col.dim > 0) ? j : cur->scan_col;

    if (state_col < 0)
        return SQLITE_OK;

    auto cs = cur->snap->cols[state_col];

    if (!cs)
        return SQLITE_OK;

    auto it = cs->rows.find(rowid);

    if (it == cs->rows.end())
        return SQLITE_OK;

    VecRowRef ref = it->second;

    if (ref.part >= cs->parts.size() || !cs->parts[ref.part])
        return SQLITE_OK;

    auto part = cs->parts[ref.part];

    if (ref.seg >= part->segs.size())
        return SQLITE_OK;

    auto seg = part->segs[ref.seg];

    if (!seg || ref.slot >= seg->count)
        return SQLITE_OK;

    if (col.dim > 0) {
        std::string err;
        VecPin pin;

        if (p->store->ensure_vecs(p->db, seg, pin, err) != SQLITE_OK) {
            p->set_err(err);
            return SQLITE_ERROR;
        }

        sqlite3_result_blob(ctx, pin.data() + (size_t)ref.slot * seg->stride, (int)col.dim * 4,
            SQLITE_TRANSIENT);
    } else if (col.is_key) {
        sqlite3_result_text(ctx, part->key.c_str(), -1, SQLITE_TRANSIENT);
    } else if (col.is_order) {
        sqlite3_result_int64(ctx, seg->meta[(size_t)ref.slot * 2 + 1]);
    }

    return SQLITE_OK;
}

static int vecIndexRowid(sqlite3_vtab_cursor* pVtabCursor, sqlite_int64* pRowid)
{
    VecCursor* cur = (VecCursor*)pVtabCursor;

    if (cur->mode == VM_SEARCH) {
        if (cur->i < cur->results.size())
            *pRowid = cur->results[cur->i].rowid;
    } else {
        *pRowid = cur->cur_rowid;
    }

    return SQLITE_OK;
}

static int vecIndexBegin(sqlite3_vtab* tab)
{
    return SQLITE_OK;
}

static int vecIndexSync(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;

    if (!p->working || !p->dirty || p->synced)
        return SQLITE_OK;

    int64_t nv = 0;
    std::string err;
    int rc = p->store->persist(p->db, *p->working, p->wset, nv, err);

    if (rc != SQLITE_OK) {
        p->set_err(err);
        return rc;
    }

    p->working->version = nv;
    p->synced = true;
    return SQLITE_OK;
}

static int vecIndexCommit(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;

    if (p->working && p->dirty) {
        p->store->publish(p->working);
        p->snap = p->working;
    }

    p->working.reset();
    p->wset.clear();
    p->dirty = false;
    p->synced = false;
    p->dv_valid = false;
    return SQLITE_OK;
}

static int vecIndexRollback(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;

    p->working.reset();
    p->wset.clear();
    p->dirty = false;
    p->synced = false;
    p->snap.reset();
    p->dv_valid = false;
    return SQLITE_OK;
}

static int vecIndexUpdate(sqlite3_vtab* pVtab, int argc, sqlite3_value** argv,
    sqlite_int64* pRowid)
{
    VecIndex* p = (VecIndex*)pVtab;
    std::string err;

    int rc = p->ensure_working();

    if (rc != SQLITE_OK)
        return rc;

    if (argc == 1 && sqlite3_value_type(argv[0]) != SQLITE_NULL) {
        if (sqlite3_value_type(argv[0]) != SQLITE_INTEGER) {
            p->set_err("vec_index: rowid must be an integer");
            return SQLITE_ERROR;
        }

        int64_t rowid = sqlite3_value_int64(argv[0]);
        rc = p->apply_delete(rowid, err);
        if (rc != SQLITE_OK) {
            p->set_err(err);
            return rc;
        }
        return SQLITE_OK;
    }

    bool is_insert = (sqlite3_value_type(argv[0]) == SQLITE_NULL);

    if (argc < 2 || sqlite3_value_type(argv[1]) != SQLITE_INTEGER) {
        p->set_err("vec_index: rowid must be an integer");
        return SQLITE_ERROR;
    }

    int64_t old_rowid = is_insert ? 0 : sqlite3_value_int64(argv[0]);
    int64_t new_rowid = sqlite3_value_int64(argv[1]);

    std::string key;
    if (p->key_col >= 0) {
        sqlite3_value* v = argv[3 + p->key_col];
        const char* t = (const char*)sqlite3_value_text(v);

        if (sqlite3_value_type(v) == SQLITE_NULL || !t) {
            p->set_err("vec_index: partition key is required");
            return SQLITE_ERROR;
        }

        key.assign(t, sqlite3_value_bytes(v));
    }

    int64_t order = 0;
    if (p->order_col >= 0) {
        sqlite3_value* v = argv[3 + p->order_col];
        if (sqlite3_value_type(v) != SQLITE_NULL)
            order = sqlite3_value_int64(v);
    }

    if (!is_insert && old_rowid != new_rowid && p->row_exists(new_rowid)) {
        p->set_err("vec_index: duplicate rowid");
        return SQLITE_ERROR;
    }

    if (is_insert && p->row_exists(new_rowid)) {
        int mode = sqlite3_vtab_on_conflict(p->db);

        if (mode == SQLITE_REPLACE) {
            rc = p->apply_delete(new_rowid, err);
            if (rc != SQLITE_OK) {
                p->set_err(err);
                return rc;
            }
        } else if (mode == SQLITE_IGNORE) {
            *pRowid = new_rowid;
            return SQLITE_OK;
        } else {
            p->set_err("vec_index: duplicate rowid");
            return SQLITE_ERROR;
        }
    }

    std::vector<std::vector<float>> vecs(p->vec_cols.size());
    std::vector<bool> has_vec(p->vec_cols.size(), false);

    for (size_t k = 0; k < p->vec_cols.size(); k++) {
        int j = p->vec_cols[k];
        sqlite3_value* v = argv[3 + j];

        if (sqlite3_value_type(v) == SQLITE_NULL) {
            if (is_insert) {
                p->set_err("vec_index: vector value is required");
                return SQLITE_ERROR;
            }
            continue;
        }

        // UPDATE passes the stored blob back for untouched columns.
        if (!is_insert && sqlite3_value_type(v) == SQLITE_BLOB
            && sqlite3_value_bytes(v) == (int)(p->cols[j].dim * 4)) {
            auto cs = p->working->cols[j];

            if (cs) {
                auto it = cs->rows.find(old_rowid);

                if (it != cs->rows.end()) {
                    VecRowRef ref = it->second;
                    auto part = cs->parts[ref.part];
                    auto seg = part ? part->segs[ref.seg] : nullptr;

                    if (seg && ref.slot < seg->count) {
                        // UPDATE of another column passes the stored blob
                        // back; the segment is loaded lazily and may still
                        // have vecs == nullptr.
                        VecPin pin;

                        if (p->store->ensure_vecs(p->db, seg, pin, err) != SQLITE_OK) {
                            p->set_err(err);
                            return SQLITE_ERROR;
                        }

                        if (memcmp(sqlite3_value_blob(v),
                                pin.data() + (size_t)ref.slot * seg->stride,
                                (size_t)p->cols[j].dim * 4)
                            == 0)
                            continue;
                    }
                }
            }
        }

        if (!parse_vector_value(v, p->cols[j].dim, vecs[k], err)) {
            p->set_err(err);
            return SQLITE_ERROR;
        }

        normalize_dim(vecs[k], p->cols[j].dim);
        has_vec[k] = true;
    }

    rc = is_insert ? p->apply_insert(new_rowid, vecs, key, order, err)
                   : p->apply_update(old_rowid, new_rowid, vecs, has_vec, key, order, err);

    if (rc != SQLITE_OK) {
        p->set_err(err);
        return rc;
    }

    *pRowid = new_rowid;
    return SQLITE_OK;
}

static void vecSearchFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    // The virtual table consumes this as an index constraint; evaluating it as
    // an ordinary scalar would silently return NULL (zero rows), so fail loud.
    sqlite3_result_error(context,
        "vec_search is only supported as a constraint on a vec_index table", -1);
}

static int vecIndexFindMethod(sqlite3_vtab* pVtab, int nArg, const char* zName,
    void (**pxFunc)(sqlite3_context*, int, sqlite3_value**), void** ppArg)
{
    if (nArg == 2 && sqlite3_stricmp(zName, "vec_search") == 0) {
        *pxFunc = vecSearchFunc;
        *ppArg = nullptr;
        return VEC_SEARCH_FUNCTION;
    }

    return 0;
}

static sqlite3_module vecIndexModule = {
    /* iVersion    */ 3,
    /* xCreate     */ vecIndexCreate,
    /* xConnect    */ vecIndexConnect,
    /* xBestIndex  */ vecIndexBestIndex,
    /* xDisconnect */ vecIndexDisconnect,
    /* xDestroy    */ vecIndexDestroy,
    /* xOpen       */ vecIndexOpen,
    /* xClose      */ vecIndexClose,
    /* xFilter     */ vecIndexFilter,
    /* xNext       */ vecIndexNext,
    /* xEof        */ vecIndexEof,
    /* xColumn     */ vecIndexColumn,
    /* xRowid      */ vecIndexRowid,
    /* xUpdate     */ vecIndexUpdate,
    /* xBegin      */ vecIndexBegin,
    /* xSync       */ vecIndexSync,
    /* xCommit     */ vecIndexCommit,
    /* xRollback   */ vecIndexRollback,
    /* xFindMethod */ vecIndexFindMethod,
    /* xRename     */ 0,
    /* xSavepoint  */ 0,
    /* xRelease    */ 0,
    /* xRollbackTo */ 0,
    /* xShadowName */ 0
};

//---------------------------------------------------------------------------
// SQL functions
//---------------------------------------------------------------------------

static void vecVersionFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    sqlite3_result_text(context, SQLITE_VEC_VERSION, -1, SQLITE_STATIC);
}

static bool value_vector(sqlite3_value* v, std::vector<float>& out)
{
    int type = sqlite3_value_type(v);

    if (type == SQLITE_BLOB) {
        const float* p = (const float*)sqlite3_value_blob(v);
        int bytes = sqlite3_value_bytes(v);

        if (!p || bytes <= 0 || bytes % 4 != 0)
            return false;

        out.resize((size_t)bytes / 4);
        memcpy(out.data(), p, (size_t)bytes);
        return true;
    }

    if (type == SQLITE_TEXT) {
        const char* t = (const char*)sqlite3_value_text(v);
        int bytes = sqlite3_value_bytes(v);
        std::string err;

        if (!t)
            return false;

        return json_vector(t, bytes, 0, out, err);
    }

    return false;
}

static void vecDistanceFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    std::vector<float> a, b;

    if (argc != 2 || !value_vector(argv[0], a) || !value_vector(argv[1], b)) {
        sqlite3_result_error(context, "Argument must be a vector", -1);
        return;
    }

    vec::normalize(a.data(), a.size());
    vec::normalize(b.data(), b.size());

    size_t n = std::max(a.size(), b.size());
    a.resize(n, 0.0f);
    b.resize(n, 0.0f);

    // Distances are float32 in the index; keep the same rounding here so the
    // two stay bit-identical on every build (including scalar ia32/ARM).
    volatile float d = 1.0f - vec::dot(a.data(), b.data(), n);
    sqlite3_result_double(context, (double)d);
}

static void vecDistanceIpFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    std::vector<float> a, b;

    if (argc != 2 || !value_vector(argv[0], a) || !value_vector(argv[1], b)) {
        sqlite3_result_error(context, "Argument must be a vector", -1);
        return;
    }

    size_t n = std::max(a.size(), b.size());
    a.resize(n, 0.0f);
    b.resize(n, 0.0f);

    volatile float d = 1.0f - vec::dot(a.data(), b.data(), n);
    sqlite3_result_double(context, (double)d);
}

static void vecNormalizeFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    std::vector<float> a;

    if (argc != 1 || !value_vector(argv[0], a)) {
        sqlite3_result_error(context, "Argument must be a vector", -1);
        return;
    }

    vec::normalize(a.data(), a.size());
    sqlite3_result_blob(context, a.data(), (int)a.size() * 4, SQLITE_TRANSIENT);
}

static bool stats_query(sqlite3* db, const char* sql, const std::string& table, int64_t& a,
    int64_t& b)
{
    sqlite3_stmt* st = nullptr;

    a = b = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(st, 1, table.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(st) == SQLITE_ROW) {
        a = sqlite3_column_int64(st, 0);
        b = sqlite3_column_int64(st, 1);
    }

    sqlite3_finalize(st);
    return true;
}

static void vecStatsFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if (argc != 1 || sqlite3_value_type(argv[0]) != SQLITE_TEXT) {
        sqlite3_result_error(context, "vec_stats(table) requires a table name", -1);
        return;
    }

    const char* table = (const char*)sqlite3_value_text(argv[0]);
    sqlite3* db = sqlite3_context_db_handle(context);

    sqlite3_stmt* st = nullptr;

    if (sqlite3_prepare_v2(db, "SELECT version,store_id FROM vec_index_state WHERE tbl=?1", -1, &st,
            nullptr)
        != SQLITE_OK) {
        sqlite3_result_error(context, "vec_index table not found", -1);
        return;
    }

    sqlite3_bind_text(st, 1, table, -1, SQLITE_TRANSIENT);

    int64_t version = 0;
    std::string store_id;

    if (sqlite3_step(st) == SQLITE_ROW) {
        version = sqlite3_column_int64(st, 0);
        const char* sid = (const char*)sqlite3_column_text(st, 1);
        if (sid)
            store_id = sid;
    } else {
        sqlite3_finalize(st);
        sqlite3_result_error(context, "vec_index table not found", -1);
        return;
    }

    sqlite3_finalize(st);

    int64_t partitions = 0, rows = 0, segments = 0, bytes = 0;

    stats_query(db, "SELECT count(*),coalesce(sum(count),0) FROM vec_index_part WHERE tbl=?1",
        table, partitions, rows);
    stats_query(db,
        "SELECT count(*),coalesce(sum(length(vecs)+length(meta)),0) FROM vec_index_seg "
        "WHERE tbl=?1",
        table, segments, bytes);

    nlohmann::json j;

    j["version"] = version;
    j["store_id"] = store_id;
    j["rows"] = rows;
    j["partitions"] = partitions;
    j["segments"] = segments;
    j["bytes"] = bytes;

    auto store = VecStore::lookup(db, table);
    VecStore::CacheStats cs;
    size_t pending = 0;

    if (store)
        cs = store->cache_stats();
    else if (VecStore::pending_limit(db, table, pending))
        // Configured before the vtab was opened: report that budget instead of
        // 0, which would read as "unlimited".
        cs.limit = (int64_t)pending;

    j["cache_bytes"] = cs.bytes;
    j["cache_limit"] = cs.limit;
    j["cache_segments"] = cs.segments;
    j["cache_hits"] = cs.hits;
    j["cache_misses"] = cs.misses;
    j["cache_evictions"] = cs.evictions;
    j["cache_loads"] = cs.loads;

    std::string s = j.dump();
    sqlite3_result_text(context, s.c_str(), -1, SQLITE_TRANSIENT);
}

// Does the table exist and is it a vec_index virtual table?  Needed because the
// store is only created when the vtab is opened, while the budget may be
// configured before the first query.
static void vecCacheLimitFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if (argc < 1 || argc > 2 || sqlite3_value_type(argv[0]) != SQLITE_TEXT) {
        sqlite3_result_error(context, "vec_cache_limit(table[, bytes]) requires a table name", -1);
        return;
    }

    const char* table = (const char*)sqlite3_value_text(argv[0]);
    sqlite3* db = sqlite3_context_db_handle(context);
    auto store = VecStore::lookup(db, table);

    if (argc == 2) {
        sqlite3_int64 bytes = sqlite3_value_int64(argv[1]);

        // 0 (or a negative value) disables the byte budget.
        size_t limit = VecStore::clamp_limit(bytes > 0 ? (size_t)bytes : 0);

        if (store)
            store->set_cache_limit(limit);
        else {
            // The vtab has not been opened on this connection yet: remember the
            // value and let VecStore::open() apply it, so a budget can be set
            // before the first query (and before anything is cached).
            if (!VecStore::table_is_vec_index(db, table)) {
                sqlite3_result_error(context, "vec_index table not found", -1);
                return;
            }

            VecStore::set_pending_limit(db, table, limit);
        }

        sqlite3_result_int64(context, (sqlite3_int64)limit);
        return;
    }

    if (store) {
        sqlite3_result_int64(context, (sqlite3_int64)store->cache_limit());
        return;
    }

    size_t pending;

    if (VecStore::pending_limit(db, table, pending)) {
        sqlite3_result_int64(context, (sqlite3_int64)pending);
        return;
    }

    if (!VecStore::table_is_vec_index(db, table)) {
        sqlite3_result_error(context, "vec_index table not found", -1);
        return;
    }

    sqlite3_result_error(context,
        "vec_index table is not open on this connection yet; query it once or call "
        "vec_cache_limit(table, bytes) first",
        -1);
}

int SQLite::vec_init()
{
    sqlite3* db = (sqlite3*)m_conn;
    int flags = SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_INNOCUOUS;

    sqlite3_create_function_v2(db, "vec_version", 0, flags, 0, vecVersionFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_search", 2, flags, 0, vecSearchFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_distance", 2, flags, 0, vecDistanceFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_distance_ip", 2, flags, 0, vecDistanceIpFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_normalize", 1, flags, 0, vecNormalizeFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_stats", 1, flags, 0, vecStatsFunc, 0, 0, 0);
    // Sets (2 args) or reads (1 arg) the byte budget of the segment cache;
    // it has side effects, so it is not marked deterministic/innocuous.
    sqlite3_create_function_v2(db, "vec_cache_limit", -1, SQLITE_UTF8, 0, vecCacheLimitFunc, 0, 0, 0);
    sqlite3_create_module_v2(db, "vec_index", &vecIndexModule, 0, 0);

    return 0;
}

} // namespace fibjs
