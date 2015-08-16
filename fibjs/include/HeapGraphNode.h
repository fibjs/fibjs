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
    HeapGraphNode(HeapSnapshot* snapshot, const v8::HeapGraphNode* graphnode);

public:
    // HeapGraphNode_base
    virtual result_t get_type(int32_t& retVal);
    virtual result_t get_name(std::string& retVal);
    virtual result_t get_id(int32_t& retVal);
    virtual result_t get_shallowSize(int32_t& retVal);
    virtual result_t get_childs(obj_ptr<List_base>& retVal);

private:
    int32_t m_id;
    int32_t m_type;
    std::string m_name;
    int32_t m_shallowSize;
    obj_ptr<List> m_childs;
};

}

#endif // _fj_HEAPGRAPHNODE_H
