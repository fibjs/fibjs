/*
 * DBResult.h
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#ifndef DBRESULT_H_
#define DBRESULT_H_

namespace fibjs {

class DBResult : public NArray {
public:
    DBResult(int32_t sz, int64_t affected, int64_t insertId)
    {
        if (sz)
            m_keys.resize(sz);

        add("affected", affected);
        add("insertId", insertId);
    }

    DBResult(int32_t sz, int64_t affected)
    {
        if (sz)
            m_keys.resize(sz);

        add("affected", affected);
    }

    DBResult(int32_t sz)
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
        m_jsRow = new NObject();
    }

    void endRow()
    {
        append(m_jsRow);
        m_jsRow.Release();
    }

    void rowValue(int32_t i, Variant& v)
    {
        m_jsRow->add(m_keys[i], v);
    }

private:
    std::vector<exlib::string> m_keys;
    obj_ptr<NObject> m_jsRow;
};

} /* namespace fibjs */
#endif /* DBRESULT_H_ */
