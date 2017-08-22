/*
 * DBResult.h
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "SimpleObject.h"

#ifndef DBRESULT_H_
#define DBRESULT_H_

namespace fibjs {

class DBResult : public SimpleObject {
public:
    DBResult(int32_t sz, int64_t affected, int64_t insertId)
        : SimpleObject(true)
    {
        if (sz)
            m_keys.resize(sz);

        add("affected", affected);
        add("insertId", insertId);
    }

    DBResult(int32_t sz, int64_t affected)
        : SimpleObject(true)
    {
        if (sz)
            m_keys.resize(sz);

        add("affected", affected);
    }

    DBResult(int32_t sz)
        : SimpleObject(true)
    {
        if (sz)
            m_keys.resize(sz);
    }

public:
    void setField(int32_t i, exlib::string& s)
    {
        m_keys[i] = s;
    }

    void beginRow()
    {
        m_jsRow = new SimpleObject();
    }

    void endRow()
    {
        add(m_jsRow);
        m_jsRow.Release();
    }

    void rowValue(int32_t i, Variant& v)
    {
        m_jsRow->add(m_keys[i], v);
        extMemory((int32_t)v.size());
    }

private:
    std::vector<exlib::string> m_keys;
    obj_ptr<SimpleObject> m_jsRow;
};

} /* namespace fibjs */
#endif /* DBRESULT_H_ */
