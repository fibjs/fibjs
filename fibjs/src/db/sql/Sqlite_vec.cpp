/*
 * SQLite_vec.cpp
 *
 *  Created on: Mar 25, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"
#include "SQLite.h"
#include "hnswlib/hnswlib.h"
#include "hnswlib/bruteforce.h"
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace fibjs {

#define SQLITE_VEC_VERSION "v0.0.1"
#define SQLITE_VEC_MAX_DIMENSIONS 4096

#define VEC_INDEX_COLUMN_DISTANCE 0
#define VEC_INDEX_COLUMN_OPERATION 1
#define VEC_INDEX_COLUMN_VECTORS 2

#define VEC_SEARCH_FUNCTION SQLITE_INDEX_CONSTRAINT_FUNCTION

#define VEC_INDEX_BLOCK_SIZE 4096

static void vec_version(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    sqlite3_result_text(context, SQLITE_VEC_VERSION, -1, SQLITE_STATIC);
}

class VecColumn : public hnswlib::BruteforceSearch<float> {
public:
    VecColumn()
        : hnswlib::BruteforceSearch<float>(nullptr)
    {
    }

    ~VecColumn()
    {
        if (space)
            delete space;
    }

public:
    int init(std::string _name, size_t dim)
    {
        name = _name;

        space = new hnswlib::InnerProductSpace(dim);
        data_size_ = space->get_data_size();
        fstdistfunc_ = space->get_dist_func();
        dist_func_param_ = space->get_dist_func_param();
        size_per_element_ = data_size_ + sizeof(hnswlib::labeltype);

        maxelements_ = 0;
        data_ = nullptr;
        cur_element_count = 0;

        return SQLITE_OK;
    }

    int load(const void* data, size_t size)
    {
        cur_element_count = size / size_per_element_;
        maxelements_ = (cur_element_count + VEC_INDEX_BLOCK_SIZE - 1) / VEC_INDEX_BLOCK_SIZE * VEC_INDEX_BLOCK_SIZE;
        data_ = (char*)malloc(maxelements_ * size_per_element_);
        if (data_ == nullptr)
            return SQLITE_NOMEM;
        memcpy(data_, data, cur_element_count * size_per_element_);

        for (size_t i = 0; i < cur_element_count; i++)
            dict_external_to_internal[rowid(i)] = i;

        return SQLITE_OK;
    }

    size_t dim() const
    {
        return data_size_ / sizeof(float);
    }

    void addPoint(const void* datapoint, hnswlib::labeltype label, bool replace_deleted = false)
    {
        if (cur_element_count == maxelements_) {
            maxelements_ += VEC_INDEX_BLOCK_SIZE;
            data_ = (char*)realloc(data_, maxelements_ * size_per_element_);
        }

        hnswlib::BruteforceSearch<float>::addPoint(datapoint, label, replace_deleted);
    }

    void removePoint(hnswlib::labeltype cur_external)
    {
        size_t cur_c = dict_external_to_internal[cur_external];

        dict_external_to_internal.erase(cur_external);
        memcpy(data_ + size_per_element_ * cur_c,
            data_ + size_per_element_ * (cur_element_count - 1), size_per_element_);
        cur_element_count--;
    }

    static bool appendResult(std::priority_queue<std::pair<float, hnswlib::labeltype>>& topResults, size_t k,
        hnswlib::labeltype label, float dist)
    {
        if (topResults.size() == k && dist > topResults.top().first)
            return false;

        topResults.push(std::pair<float, hnswlib::labeltype>(dist, label));
        if (topResults.size() > k)
            topResults.pop();

        return true;
    }

    class search_job {
    public:
        const VecColumn* index;
        size_t begin, end;
        const void* query_data;
        size_t k;
        std::priority_queue<std::pair<float, hnswlib::labeltype>> topResults;
        exlib::Event ev;
    };

    static int search_thread(search_job* job)
    {
        for (size_t i = job->begin; i < job->end; i++) {
            hnswlib::labeltype label = *((hnswlib::labeltype*)(job->index->data_ + job->index->size_per_element_ * i + job->index->data_size_));
            float dist = job->index->fstdistfunc_(job->query_data, job->index->data_ + job->index->size_per_element_ * i, job->index->dist_func_param_);
            appendResult(job->topResults, job->k, label, dist);
        }
        job->ev.set();

        return 0;
    }

    std::priority_queue<std::pair<float, hnswlib::labeltype>> search(const void* query_data, size_t k) const
    {
        assert(k <= cur_element_count);

        int32_t cpus = 0;
        os_base::cpuNumbers(cpus);

        int worker_count = cpus - 2;
        if (worker_count * 32 > cur_element_count)
            worker_count = cur_element_count / 32;

        if (worker_count < 1)
            worker_count = 1;

        search_job* jobs = new search_job[worker_count];
        size_t step = cur_element_count / worker_count;

        for (int i = 0; i < worker_count; i++) {
            jobs[i].index = this;
            jobs[i].begin = i * step;

            if (i == worker_count - 1)
                jobs[i].end = cur_element_count;
            else
                jobs[i].end = (i + 1) * step;

            jobs[i].query_data = query_data;
            jobs[i].k = k;

            if (i > 0)
                async([jobs, i]() {
                    search_thread(&jobs[i]);
                }, CALL_E_LONGSYNC);
        }

        search_thread(&jobs[0]);

        for (int i = 0; i < worker_count; i++)
            jobs[i].ev.wait();

        std::priority_queue<std::pair<float, hnswlib::labeltype>> topResults;
        for (int i = 0; i < worker_count; i++) {
            while (!jobs[i].topResults.empty()) {
                auto r = jobs[i].topResults.top();
                appendResult(topResults, k, r.second, r.first);
                jobs[i].topResults.pop();
            }
        }

        delete[] jobs;

        return topResults;
    }

    hnswlib::labeltype rowid(size_t idx) const
    {
        return *(hnswlib::labeltype*)(data_ + idx * size_per_element_ + data_size_);
    }

public:
    std::string name;
    hnswlib::InnerProductSpace* space = nullptr;
};

class VecIndexColumn {
public:
    std::string name;
    sqlite3_int64 dimensions;
};

class VecIndex : public sqlite3_vtab {
public:
    class op {
    public:
        hnswlib::labeltype rowid;
        std::vector<std::vector<float>> datas;
    };

public:
    VecIndex(sqlite3* db, const char* _name, std::vector<VecIndexColumn>& _columns)
        : db(db)
        , name(_name)
    {
        memset(this, 0, sizeof(sqlite3_vtab));

        indexCount = _columns.size();
        columns = new VecColumn[indexCount];

        for (int i = 0; i < indexCount; i++)
            columns[i].init(_columns[i].name, _columns[i].dimensions);
    }

    ~VecIndex()
    {
        delete[] columns;
    }

public:
    int create_index()
    {
        const char* zQuery;
        int rc;

        rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS vec_index(tbl, name, data)", 0, 0, 0);
        if (rc != SQLITE_OK)
            return rc;

        rc = sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS vi_index ON vec_index (tbl, name)", 0, 0, 0);
        if (rc != SQLITE_OK)
            return rc;

        for (int i = 0; i < indexCount; i++) {
            zQuery = sqlite3_mprintf("INSERT INTO vec_index(tbl, name) VALUES (\"%w\", \"%w\")",
                name.c_str(), columns[i].name.c_str());
            rc = sqlite3_exec(db, zQuery, 0, 0, 0);
            sqlite3_free((void*)zQuery);
            if (rc != SQLITE_OK)
                return rc;
        }

        return SQLITE_OK;
    }

    int drop_index()
    {
        const char* zQuery;

        zQuery = sqlite3_mprintf("DELETE FROM vec_index WHERE tbl = \"%w\"", name.c_str());
        int rc = sqlite3_exec(db, zQuery, 0, 0, 0);
        sqlite3_free((void*)zQuery);
        return rc;
    }

    int load_index()
    {
        const char* zQuery;
        sqlite3_stmt* stmt;
        int rc;

        for (int i = 0; i < indexCount; i++) {
            zQuery = sqlite3_mprintf("SELECT data  FROM vec_index WHERE tbl = \"%w\" AND name = \"%w\"",
                name.c_str(), columns[i].name.c_str());
            rc = sqlite3_prepare_v2(db, zQuery, -1, &stmt, 0);
            sqlite3_free((void*)zQuery);
            if (rc != SQLITE_OK)
                return rc;

            if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                const void* idx_data = sqlite3_column_blob(stmt, 0);
                size_t idx_size = sqlite3_column_bytes(stmt, 0);

                rc = columns[i].load(idx_data, idx_size);
            }

            sqlite3_finalize(stmt);
            if (rc != SQLITE_OK)
                return rc;
        }

        return SQLITE_OK;
    }

    int sync_index()
    {
        if (ops.size()) {
            std::vector<bool> dirty;
            dirty.resize(indexCount);

            for (auto& op : ops) {
                if (op.datas.size() == 0) {
                    // delete
                    for (int i = 0; i < indexCount; i++) {
                        dirty[i] = true;
                        columns[i].removePoint(op.rowid);
                    }
                } else {
                    // insert or update
                    for (int i = 0; i < indexCount; i++)
                        if (op.datas[i].size()) {
                            dirty[i] = true;
                            columns[i].addPoint(op.datas[i].data(), op.rowid);
                        }
                }
            }

            rollback();

            int rc;
            const char* zQuery;
            sqlite3_stmt* stmt;

            for (int i = 0; i < indexCount; i++)
                if (dirty[i]) {
                    zQuery = sqlite3_mprintf("UPDATE vec_index SET data = ? WHERE tbl = \"%w\" AND name = \"%w\"",
                        name.c_str(), columns[i].name.c_str());
                    rc = sqlite3_prepare_v2(db, zQuery, -1, &stmt, 0);
                    if (rc != SQLITE_OK || stmt == 0) {
                        return rc;
                    }
                    const VecColumn& _idx = columns[i];
                    rc = sqlite3_bind_blob64(stmt, 1, _idx.data_, _idx.cur_element_count * _idx.size_per_element_, SQLITE_TRANSIENT);
                    if (rc != SQLITE_OK) {
                        sqlite3_free((void*)zQuery);
                        return rc;
                    }

                    rc = sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    sqlite3_free((void*)zQuery);

                    if (rc != SQLITE_DONE)
                        return rc;
                }
        }

        return SQLITE_OK;
    }

    void rollback()
    {
        ops.clear();
        incr_ops.clear();
    }

    bool exists(hnswlib::labeltype rowid)
    {
        auto it = incr_ops.find(rowid);
        if (it != incr_ops.end())
            return it->second;

        const VecColumn& idx = columns[0];
        return idx.dict_external_to_internal.find(rowid) != idx.dict_external_to_internal.end();
    }

public:
    sqlite3* db;

    std::string name;

    int32_t indexCount;
    VecColumn* columns;

    std::vector<op> ops;
    std::unordered_map<hnswlib::labeltype, bool> incr_ops;
};

enum QueryType {
    search,
    fullscan
};

class VecCursor : public sqlite3_vtab_cursor {
public:
    VecCursor(VecIndex* tab)
    {
        pVtab = tab;
    }

public:
    QueryType query_type;
    std::vector<std::pair<float, hnswlib::labeltype>> search_result;
    size_t iCurrent;
};

std::vector<VecIndexColumn> parse_constructor(int argc, const char* const* argv)
{
    std::vector<VecIndexColumn> columns;
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];

        std::size_t lparen = arg.find("(");
        std::size_t rparen = arg.find(")");

        if (lparen == std::string::npos || rparen == std::string::npos || lparen >= rparen) {
            columns.clear();
            return columns;
        }
        std::string name = arg.substr(0, lparen);
        std::string sDimensions = arg.substr(lparen + 1, rparen - lparen - 1);
        sqlite3_int64 dimensions = std::atoi(sDimensions.c_str());
        if (dimensions <= 0 || dimensions > SQLITE_VEC_MAX_DIMENSIONS) {
            columns.clear();
            return columns;
        }

        columns.push_back({ name, dimensions });
    }

    return columns;
}

void normalize_vector(std::vector<float>& v)
{
    float norm = 0.0f;
    for (int i = 0; i < v.size(); i++)
        norm += v[i] * v[i];
    norm = 1.0f / (sqrtf(norm) + 1e-30f);

    for (int i = 0; i < v.size(); i++)
        v[i] *= norm;
}

std::vector<float> parse_vector(const char* txt, size_t dim)
{
    std::vector<float> v;

    try {
        nlohmann::json data = nlohmann::json::parse(txt);
        data.get_to(v);
        if (dim > 0 && v.size() < dim)
            v.resize(dim, 0.0f);

        normalize_vector(v);

        return v;
    } catch (const nlohmann::json::exception&) {
    }

    return std::vector<float>();
}

std::vector<float> parse_vector(sqlite3_value* value, size_t dim)
{
    std::vector<float> v;

    int type = sqlite3_value_type(value);
    if (type == SQLITE_BLOB) {
        const float* blob = (float*)sqlite3_value_blob(value);
        if (blob == NULL)
            return v;

        size_t len = sqlite3_value_bytes(value);
        if (len % sizeof(float) != 0 || (dim > 0 && len > dim * sizeof(float)))
            return v;

        for (int i = 0; i < len / sizeof(float); i++)
            if (std::isnan(blob[i]))
                return v;

        if (dim > 0)
            v.resize(dim);
        else
            v.resize(len);

        memcpy(v.data(), blob, len);
        for (int i = len / sizeof(float); i < dim; i++)
            v[i] = 0.0f;

        normalize_vector(v);

        return v;
    } else if (type == SQLITE_TEXT) {
        const char* txt = (const char*)sqlite3_value_text(value);
        if (txt == NULL)
            return v;

        return parse_vector(txt, dim);
    }
}

static int init(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr, bool isCreate)
{
    sqlite3_vtab_config(db, SQLITE_VTAB_CONSTRAINT_SUPPORT, 1);
    int rc;

    std::vector<VecIndexColumn> columns = parse_constructor(argc, argv);
    if (columns.size() == 0) {
        *pzErr = sqlite3_mprintf("Error parsing constructor");
        return SQLITE_ERROR;
    }

    sqlite3_str* str = sqlite3_str_new(NULL);
    sqlite3_str_appendall(str, "CREATE TABLE x(distance hidden, operation hidden");

    for (auto column = columns.begin(); column != columns.end(); ++column)
        sqlite3_str_appendf(str, ", \"%w\"", column->name.c_str());

    sqlite3_str_appendall(str, ")");
    const char* query = sqlite3_str_finish(str);
    rc = sqlite3_declare_vtab(db, query);
    sqlite3_free((void*)query);

    if (rc != SQLITE_OK)
        return rc;

    VecIndex* pNew = new VecIndex(db, argv[2], columns);
    *ppVtab = pNew;

    if (isCreate)
        return pNew->create_index();
    else
        return pNew->load_index();
}

static int vecIndexCreate(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr)
{
    return init(db, pAux, argc, argv, ppVtab, pzErr, true);
}

static int vecIndexConnect(sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr)
{
    return init(db, pAux, argc, argv, ppVtab, pzErr, false);
}

static int vecIndexBestIndex(sqlite3_vtab* tab, sqlite3_index_info* pIdxInfo)
{
    for (int i = 0; i < pIdxInfo->nConstraint; i++) {
        auto constraint = pIdxInfo->aConstraint[i];

        if (constraint.usable) {
            if (constraint.op == VEC_SEARCH_FUNCTION) {
                pIdxInfo->aConstraintUsage[i].argvIndex = 1;
                pIdxInfo->aConstraintUsage[i].omit = 1;

                pIdxInfo->idxStr = (char*)"search";
                pIdxInfo->idxNum = constraint.iColumn - VEC_INDEX_COLUMN_VECTORS;

                pIdxInfo->estimatedCost = 300.0;
                pIdxInfo->estimatedRows = 10;
                return SQLITE_OK;
            } else if (constraint.op == SQLITE_INDEX_CONSTRAINT_EQ && constraint.iColumn == -1) {
                pIdxInfo->aConstraintUsage[i].argvIndex = 1;
                pIdxInfo->aConstraintUsage[i].omit = 1;

                pIdxInfo->idxStr = (char*)"equal";
                pIdxInfo->idxNum = -1;

                pIdxInfo->estimatedCost = 300.0;
                pIdxInfo->estimatedRows = 10;
                return SQLITE_OK;
            } else {
                // printf("unsupported constraint: %d\n", constraint.op);
            }
        }
    }

    if (pIdxInfo->idxStr == NULL) {
        pIdxInfo->idxNum = -1;
        pIdxInfo->idxStr = (char*)"fullscan";
        pIdxInfo->estimatedCost = 3000000.0;
        pIdxInfo->estimatedRows = 100000;
    }

    return SQLITE_OK;
}

static int vecIndexDisconnect(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;
    delete p;

    return SQLITE_OK;
}

static int vecIndexDestroy(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;
    p->drop_index();
    vecIndexDisconnect(pVtab);

    return SQLITE_OK;
}

static int vecIndexOpen(sqlite3_vtab* pVtab, sqlite3_vtab_cursor** ppCursor)
{
    *ppCursor = new VecCursor((VecIndex*)pVtab);
    return SQLITE_OK;
}

static int vecIndexClose(sqlite3_vtab_cursor* cur)
{
    VecCursor* pCur = (VecCursor*)cur;
    delete pCur;
    return SQLITE_OK;
}

static int vecIndexFilter(sqlite3_vtab_cursor* pVtabCursor, int idxNum, const char* idxStr,
    int argc, sqlite3_value** argv)
{
    VecCursor* pCur = (VecCursor*)pVtabCursor;

    if (strcmp(idxStr, "search") == 0) {
        pCur->query_type = QueryType::search;
        VecColumn& column = ((VecIndex*)pCur->pVtab)->columns[idxNum];
        const char* txt = (const char*)sqlite3_value_text(argv[0]);
        std::string tmp;
        int nlimit = 1024;

        if (txt) {
            const char* limit = qstrchr(txt, ':');
            if (limit) {
                tmp = std::string(txt, limit - txt);
                txt = tmp.c_str();
                nlimit = atoi(limit + 1);
                if (nlimit <= 0) {
                    ((VecIndex*)pCur->pVtab)->zErrMsg = sqlite3_mprintf("The limit must be greater than 0");
                    return SQLITE_ERROR;
                }
            }
        }

        std::vector<float> query_vector = parse_vector(txt, column.dim());
        if (query_vector.size() == 0) {
            ((VecIndex*)pCur->pVtab)->zErrMsg = sqlite3_mprintf("The variable \"%s\" must be a vector", column.name.c_str());
            return SQLITE_ERROR;
        }
        if (query_vector.size() > column.dim()) {
            ((VecIndex*)pCur->pVtab)->zErrMsg = sqlite3_mprintf("Vector \"%s\" size must be less than or equal to %d",
                column.name.c_str(), column.dim());
            return SQLITE_ERROR;
        }

        std::priority_queue<std::pair<float, hnswlib::labeltype>> search_result;
        search_result = column.search(query_vector.data(), nlimit < column.cur_element_count ? nlimit : column.cur_element_count);

        size_t sz = search_result.size();

        pCur->search_result.clear();
        pCur->search_result.resize(sz);

        while (!search_result.empty()) {
            pCur->search_result[sz-- - 1] = search_result.top();
            search_result.pop();
        }
        pCur->iCurrent = 0;

        return SQLITE_OK;
    }

    if (strcmp(idxStr, "equal") == 0) {
        pCur->query_type = QueryType::search;
        hnswlib::labeltype id = (hnswlib::labeltype)sqlite3_value_int64(argv[0]);

        pCur->search_result.clear();
        if (((VecIndex*)pCur->pVtab)->exists(id))
            pCur->search_result.push_back(std::make_pair(0.0f, id));

        pCur->iCurrent = 0;
        return SQLITE_OK;
    }

    if (strcmp(idxStr, "fullscan") == 0) {
        pCur->query_type = QueryType::fullscan;
        pCur->iCurrent = 0;
        return SQLITE_OK;
    }

    return SQLITE_OK;
}

static int vecIndexNext(sqlite3_vtab_cursor* cur)
{
    VecCursor* pCur = (VecCursor*)cur;

    switch (pCur->query_type) {
    case QueryType::search:
        pCur->iCurrent++;
        break;
    case QueryType::fullscan:
        pCur->iCurrent++;
        break;
    }

    return SQLITE_OK;
}

static int vecIndexEof(sqlite3_vtab_cursor* cur)
{
    VecCursor* pCur = (VecCursor*)cur;

    switch (pCur->query_type) {
    case QueryType::search:
        return pCur->iCurrent >= pCur->search_result.size();
    case QueryType::fullscan:
        return pCur->iCurrent >= ((VecIndex*)pCur->pVtab)->columns[0].cur_element_count;
    default:
        exit(0);
        return 1;
    }
    return 1;
}

static int vecIndexColumn(sqlite3_vtab_cursor* cur, sqlite3_context* ctx, int i)
{
    VecCursor* pCur = (VecCursor*)cur;

    if (i == VEC_INDEX_COLUMN_DISTANCE) {
        switch (pCur->query_type) {
        case QueryType::search:
            sqlite3_result_double(ctx, pCur->search_result[pCur->iCurrent].first);
            break;
        case QueryType::fullscan:
            break;
        }
    }

    return SQLITE_OK;
}

static int vecIndexRowid(sqlite3_vtab_cursor* cur, sqlite_int64* pRowid)
{
    VecCursor* pCur = (VecCursor*)cur;

    switch (pCur->query_type) {
    case QueryType::search:
        *pRowid = pCur->search_result[pCur->iCurrent].second;
        break;
    case QueryType::fullscan:
        *pRowid = ((VecIndex*)pCur->pVtab)->columns[0].rowid(pCur->iCurrent);
        break;
    default:
        exit(0);
        return 1;
    }

    return SQLITE_OK;
}

static int vecIndexUpdate(sqlite3_vtab* pVtab, int argc, sqlite3_value** argv, sqlite_int64* pRowid)
{
    VecIndex* p = (VecIndex*)pVtab;
    VecIndex::op op;

    if (argc == 1 && sqlite3_value_type(argv[0]) != SQLITE_NULL) {
        // DELETE operation
        if (SQLITE_INTEGER != sqlite3_value_type(argv[0])) {
            p->zErrMsg = sqlite3_mprintf("rowid must be an integer");
            return SQLITE_ERROR;
        }

        op.rowid = sqlite3_value_int64(argv[0]);
        p->incr_ops[op.rowid] = false;
    } else if (argc > 1 && sqlite3_value_type(argv[0]) == SQLITE_NULL) {
        // INSERT operation
        if (SQLITE_INTEGER != sqlite3_value_type(argv[1])) {
            p->zErrMsg = sqlite3_mprintf("rowid must be an integer");
            return SQLITE_ERROR;
        }

        op.rowid = sqlite3_value_int64(argv[1]);
        if (p->exists(op.rowid)) {
            p->zErrMsg = sqlite3_mprintf("rowid[%lld] already exists", op.rowid);
            return SQLITE_ERROR;
        }

        for (int i = 0; i < p->indexCount; i++) {
            std::vector<float> vec = parse_vector(argv[2 + VEC_INDEX_COLUMN_VECTORS + i], p->columns[i].dim());
            if (vec.size() == 0) {
                p->zErrMsg = sqlite3_mprintf("The variable \"%s\" must be a vector", p->columns[i].name.c_str());
                return SQLITE_ERROR;
            }
            if (vec.size() > p->columns[i].dim()) {
                p->zErrMsg = sqlite3_mprintf("Vector \"%s\" size must be less than or equal to %d",
                    p->columns[i].name.c_str(), p->columns[i].dim());
                return SQLITE_ERROR;
            }

            op.datas.push_back(vec);
        }

        p->incr_ops[op.rowid] = true;
    } else {
        // some UPDATE operations
        if (SQLITE_INTEGER != sqlite3_value_type(argv[1])) {
            p->zErrMsg = sqlite3_mprintf("rowid must be an integer");
            return SQLITE_ERROR;
        }

        op.rowid = sqlite3_value_int64(argv[1]);

        for (int i = 0; i < p->indexCount; i++) {
            std::vector<float> vec;

            if (SQLITE_NULL != sqlite3_value_type(argv[2 + VEC_INDEX_COLUMN_VECTORS + i])) {
                vec = parse_vector(argv[2 + VEC_INDEX_COLUMN_VECTORS + i], p->columns[i].dim());
                if (vec.size() == 0) {
                    p->zErrMsg = sqlite3_mprintf("The variable \"%s\" must be a vector", p->columns[i].name.c_str());
                    return SQLITE_ERROR;
                }
                if (vec.size() > p->columns[i].dim()) {
                    p->zErrMsg = sqlite3_mprintf("Vector \"%s\" size must be less than or equal to %d",
                        p->columns[i].name.c_str(), p->columns[i].dim());
                    return SQLITE_ERROR;
                }
            }

            op.datas.push_back(vec);
        }
    }

    p->ops.push_back(op);

    return SQLITE_OK;
}

static int vecIndexBegin(sqlite3_vtab* tab)
{
    return SQLITE_OK;
}

static int vecIndexSync(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;
    return p->sync_index();
}

static int vecIndexCommit(sqlite3_vtab* pVtab)
{
    return SQLITE_OK;
}

static int vecIndexRollback(sqlite3_vtab* pVtab)
{
    VecIndex* p = (VecIndex*)pVtab;
    p->rollback();
    return SQLITE_OK;
}

static void vecDistanceFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if (argc != 2) {
        sqlite3_result_error(context, "Incorrect number of arguments", -1);
        return;
    }

    std::vector<float> vec0 = parse_vector(argv[0], 0);
    std::vector<float> vec1 = parse_vector(argv[1], 0);
    if (vec0.size() == 0 || vec1.size() == 0) {
        sqlite3_result_error(context, "Argument must be a vector", -1);
        return;
    }

    if (vec0.size() < vec1.size())
        vec0.resize(vec1.size(), 0.0f);
    else if (vec0.size() > vec1.size())
        vec1.resize(vec0.size(), 0.0f);

    size_t dim = vec0.size();
    sqlite3_result_double(context, hnswlib::InnerProductDistance(vec0.data(), vec1.data(), &dim));
}

static void vecSearchFunc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
}

static int vecIndexFindMethod(sqlite3_vtab* pVtab, int nArg, const char* zName,
    void (**pxFunc)(sqlite3_context*, int, sqlite3_value**), void** ppArg)
{
    if (sqlite3_stricmp(zName, "vec_search") == 0) {
        *pxFunc = vecSearchFunc;
        *ppArg = 0;
        return VEC_SEARCH_FUNCTION;
    }

    return SQLITE_OK;
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

int SQLite::vec_init()
{
    sqlite3* db = (sqlite3*)m_conn;

    sqlite3_create_function_v2(db, "vec_version", 0, SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_INNOCUOUS, 0, vec_version, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_search", 2, SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_INNOCUOUS, 0, vecSearchFunc, 0, 0, 0);
    sqlite3_create_function_v2(db, "vec_distance", 2, SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_INNOCUOUS, 0, vecDistanceFunc, 0, 0, 0);
    sqlite3_create_module_v2(db, "vec_index", &vecIndexModule, 0, 0);

    return 0;
}
}
