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
    HeapGraphNode(const v8::HeapGraphNode* graphnode)
        : m_graphnode(graphnode)
    {}

public:
    // HeapGraphNode_base
    virtual result_t get_type(std::string& retVal);
    virtual result_t get_name(std::string& retVal);
    virtual result_t get_id(int32_t& retVal);
    virtual result_t get_shallowSize(int32_t& retVal);
    virtual result_t get_childsCount(int32_t& retVal);
    virtual result_t get_childs(obj_ptr<List_base>& retVal);

private:
    const v8::HeapGraphNode* m_graphnode;
    obj_ptr<List> m_childs;
};

}

#endif // _fj_HEAPGRAPHNODE_H
