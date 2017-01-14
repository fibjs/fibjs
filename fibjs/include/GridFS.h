/*
 * GridFS.h
 *
 *  Created on: Nov 8, 2012
 *      Author: lion
 */

#include "ifs/GridFS.h"
#include "MongoDB.h"

#ifndef GRIDFS_H_
#define GRIDFS_H_

namespace fibjs
{

class GridFS: public GridFS_base
{
public:
    GridFS(MongoDB *db) :
        m_db(db)
    {
        m_bgridfs = false;
    }

    ~GridFS()
    {
        if (m_bgridfs)
            gridfs_destroy(&m_fs);
    }

public:
    virtual bool enterTask(exlib::Task_base *current)
    {
        return m_db->enterTask(current);
    }

    virtual void enter()
    {
        m_db->enter();
    }

    virtual void leave(exlib::Task_base *current = NULL)
    {
        m_db->leave(current);
    }

public:
    // GridFS_base
    virtual result_t retrieve(exlib::string name, obj_ptr<MemoryStream_base> &retVal);
    virtual result_t store(exlib::string name, Stream_base *src);
    virtual result_t store(exlib::string name, Buffer_base *data);
    virtual result_t exists(exlib::string name, bool &retVal);
    virtual result_t remove(exlib::string name);
    virtual result_t get_files(obj_ptr<MongoCollection_base> &retVal);
    virtual result_t get_chunks(obj_ptr<MongoCollection_base> &retVal);

private:
    int32_t check_fs()
    {
        if (!m_bgridfs)
        {
            if (gridfs_init(&m_db->m_conn, m_db->m_ns.c_str(), NULL, &m_fs) != MONGO_OK)
                return MONGO_ERROR;
            m_bgridfs = true;
        }

        return MONGO_OK;
    }

    result_t get_list(const char *name, obj_ptr<MongoCollection_base> &retVal);

private:
    obj_ptr<MongoDB> m_db;
    bool m_bgridfs;
    gridfs m_fs;
};

} /* namespace fibjs */
#endif /* GRIDFS_H_ */
