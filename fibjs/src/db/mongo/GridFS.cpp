/*
 * GridFS.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: lion
 */

#include "GridFS.h"
#include "MongoCollection.h"
#include "MemoryStream.h"

namespace fibjs
{

result_t GridFS::retrieve(const char *name, obj_ptr<MemoryStream_base> &retVal)
{
    if (check_fs() != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    gridfile f;

    if (gridfs_find_filename(&m_fs, name, &f) != MONGO_OK)
    {
        result_t hr = CHECK_ERROR(m_db->error());
        if (hr < 0)
            return hr;

        return -2; // file not found.
    }

    qstring strBuf;
    gridfs_offset len = gridfile_get_contentlength(&f);

    if (len > 0)
    {
        strBuf.resize((size_t)len);
        if (gridfile_read_buffer(&f, &strBuf[0], len) != len)
        {
            gridfile_destroy(&f);
            return CHECK_ERROR(m_db->error());
        }
    }

    gridfile_destroy(&f);

    retVal = new MemoryStream::CloneStream(strBuf, 0);

    return 0;
}

result_t GridFS::store(const char *name, Stream_base *src)
{
    if (check_fs() != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    result_t hr;
    gridfile f;

    gridfile_writer_init(&f, &m_fs, name, "", GRIDFILE_DEFAULT);

    while (true)
    {
        obj_ptr<Buffer_base> data;

        hr = src->ac_read(-1, data);
        if (hr < 0)
        {
            gridfile_destroy(&f);
            return hr;
        }

        if (hr == CALL_RETURN_NULL)
            break;

        qstring strBuf;
        data->toString(strBuf);

        gridfile_write_buffer(&f, strBuf.c_str(), strBuf.length());
    }

    if (gridfile_writer_done(&f) != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    return 0;
}

result_t GridFS::store(const char *name, Buffer_base *data)
{
    if (check_fs() != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    gridfile f;

    gridfile_writer_init(&f, &m_fs, name, "", GRIDFILE_DEFAULT);

    qstring strBuf;
    data->toString(strBuf);

    gridfile_write_buffer(&f, strBuf.c_str(), strBuf.length());

    if (gridfile_writer_done(&f) != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    return 0;
}

result_t GridFS::exists(const char *name, bool &retVal)
{
    if (check_fs() != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    gridfile f;

    if (gridfs_find_filename(&m_fs, name, &f) != MONGO_OK)
    {
        result_t hr = CHECK_ERROR(m_db->error());
        if (hr < 0)
            return hr;

        retVal = false;
        return 0;
    }

    gridfile_destroy(&f);

    retVal = true;
    return 0;
}

result_t GridFS::remove(const char *name)
{
    if (check_fs() != MONGO_OK)
        return CHECK_ERROR(m_db->error());

    gridfs_remove_filename(&m_fs, name);
    return 0;
}

result_t GridFS::get_list(const char *name,
                          obj_ptr<MongoCollection_base> &retVal)
{
    qstring ns(m_db->m_ns);
    qstring nmStr;

    nmStr.append("fs", 2);
    nmStr += '.';
    nmStr.append(name);

    if (!ns.empty())
        ns += '.';

    ns.append(nmStr);

    retVal = new MongoCollection(m_db, ns.c_str(), nmStr.c_str());

    return 0;
}

result_t GridFS::get_files(obj_ptr<MongoCollection_base> &retVal)
{
    return get_list("files", retVal);
}

result_t GridFS::get_chunks(obj_ptr<MongoCollection_base> &retVal)
{
    return get_list("chunks", retVal);
}

} /* namespace fibjs */
