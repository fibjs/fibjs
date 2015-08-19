/*
 * HeapGraphNode.h
 *
 *  Created on: Aug 9, 2015
 *      Author: lion
 */

#include "ifs/HeapGraphNode.h"
#include <v8/include/v8-profiler.h>
#include "List.h"

#ifndef _fj_HEAPGRAPHNODE_H
#define _fj_HEAPGRAPHNODE_H

namespace fibjs
{

class HeapSnapshot;
class HeapGraphNode : public HeapGraphNode_base
{
public:
    HeapGraphNode(int32_t _type, std::string& _name, int32_t _id,
                  int32_t _size, List* _childs) :
        m_type(_type), m_name(_name), m_id(_id),
        m_shallowSize(_size), m_childs(_childs)
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

public:
    static void get_description(HeapGraphNode_base* node, std::string& retVal)
    {
        static char* types[] = {
            "Hidden",
            "Array",
            "String",
            "Object",
            "Code",
            "Closure",
            "RegExp",
            "HeapNumber",
            "Native",
            "Synthetic",
            "ConsString",
            "SlicedString",
            "Symbol",
            "SimdValue"
        };

        int32_t type;
        std::string name;

        node->get_type(type);
        node->get_name(retVal);

        retVal.append(1, '[');
        if (type >= 0 && type < ARRAYSIZE(types))
            retVal.append(types[type]);
        else
            retVal.append("Unknown");
        retVal.append(1, ']');
    }

private:
    int32_t m_type;
    std::string m_name;
    int32_t m_id;
    int32_t m_shallowSize;
    obj_ptr<List> m_childs;
};

}

#endif // _fj_HEAPGRAPHNODE_H
