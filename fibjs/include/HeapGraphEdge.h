/*
 * HeapGraphEdge.h
 *
 *  Created on: Aug 9, 2015
 *      Author: lion
 */

#pragma once

#include "ifs/HeapGraphEdge.h"
#include "ifs/HeapSnapshot.h"
#include <v8-profiler.h>

namespace fibjs {

class HeapGraphEdge : public HeapGraphEdge_base {
public:
    HeapGraphEdge(HeapSnapshot_base* snapshot, int32_t _type,
        exlib::string& _name, int32_t _fromid, int32_t _toid)
        : m_snapshot(snapshot)
        , m_type(_type)
        , m_name(_name)
        , m_fromId(_fromid)
        , m_toId(_toid)
    {
    }

public:
    // HeapGraphEdge_base
    virtual result_t get_type(int32_t& retVal);
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_description(exlib::string& retVal);
    virtual result_t getFromNode(obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t getToNode(obj_ptr<HeapGraphNode_base>& retVal);

public:
    int32_t toid()
    {
        return m_toId;
    }

    static void get_description(HeapGraphEdge_base* edge, exlib::string& retVal)
    {
        static const char* types[] = {
            "ContextVariable",
            "Element",
            "Property",
            "Internal",
            "Hidden",
            "Shortcut",
            "Weak"
        };

        int32_t type;
        exlib::string name;

        edge->get_type(type);
        edge->get_name(retVal);

        retVal.append(1, '[');
        if (type >= 0 && type < (int32_t)ARRAYSIZE(types))
            retVal.append(types[type]);
        else
            retVal.append("Unknown");
        retVal.append(1, ']');
    }

private:
    weak_ptr<HeapSnapshot_base> m_snapshot;
    int32_t m_type;
    exlib::string m_name;
    int32_t m_fromId;
    int32_t m_toId;
};
}
