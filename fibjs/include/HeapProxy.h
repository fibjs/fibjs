/*
 * HeapProxy.h
 *
 *  Created on: Aug 16, 2015
 *      Author: lion
 */

#include "ifs/HeapSnapshot.h"
#include "ifs/HeapGraphEdge.h"
#include "List.h"
#include <v8/include/v8-profiler.h>
#include <map>

#ifndef _fj_HEAPPROXY_H
#define _fj_HEAPPROXY_H

namespace fibjs
{

class HeapSnapshotProxy : public HeapSnapshot_base
{
public:
    HeapSnapshotProxy(const v8::HeapSnapshot* snapshot) :
        m_snapshot(snapshot)
    {
    }

    static void del_(const v8::HeapSnapshot* snapshot)
    {
        ((v8::HeapSnapshot*)snapshot)->Delete();
    }

    ~HeapSnapshotProxy()
    {
        if (in_gc())
            syncCall(holder(), del_, m_snapshot);
        else
            ((v8::HeapSnapshot*)m_snapshot)->Delete();
    }

public:
    // HeapSnapshot_base
    virtual result_t diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal);
    virtual result_t getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t save(const char* fname, AsyncEvent* ac);
    virtual result_t get_time(date_t& retVal);
    virtual result_t get_root(obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t get_nodes(obj_ptr<List_base>& retVal);

private:
    void fill_nodes();

private:
    const v8::HeapSnapshot* m_snapshot;
    date_t m_d;
    std::map<int32_t, int32_t> _nodes;
    obj_ptr<List> m_nodes;
};

class HeapGraphNodeProxy : public HeapGraphNode_base
{
public:
    HeapGraphNodeProxy(HeapSnapshotProxy* snapshot, const v8::HeapGraphNode* node) :
        m_snapshot(snapshot), m_node(node)
    {
    }

public:
    // HeapGraphNode_base
    virtual result_t get_type(int32_t& retVal);
    virtual result_t get_name(std::string& retVal);
    virtual result_t get_description(std::string& retVal);
    virtual result_t get_id(int32_t& retVal);
    virtual result_t get_shallowSize(int32_t& retVal);
    virtual result_t get_childs(obj_ptr<List_base>& retVal);

private:
    weak_ptr<HeapSnapshotProxy> m_snapshot;
    const v8::HeapGraphNode* m_node;
    obj_ptr<List> m_childs;
};

class HeapGraphEdgeProxy : public HeapGraphEdge_base
{
public:
    HeapGraphEdgeProxy(HeapSnapshotProxy* snapshot, const v8::HeapGraphEdge* edge) :
        m_snapshot(snapshot), m_edge(edge)
    {
    }

public:
    // HeapGraphEdge_base
    virtual result_t get_type(int32_t& retVal);
    virtual result_t get_name(std::string& retVal);
    virtual result_t get_description(std::string& retVal);
    virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal);

private:
    weak_ptr<HeapSnapshotProxy> m_snapshot;
    const v8::HeapGraphEdge* m_edge;
};

}

#endif // _fj_HEAPPROXY_H
