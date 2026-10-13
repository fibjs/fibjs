/*
 * HeapGraphNode.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "object.h"
#include "HeapGraphNode.h"
#include "HeapGraphEdge.h"
#include "HeapSnapshot.h"

namespace fibjs {

result_t HeapGraphNode::get_type(int32_t& retVal)
{
    retVal = m_type;
    return result_t();
}

result_t HeapGraphNode::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return result_t();
}

result_t HeapGraphNode::get_description(exlib::string& retVal)
{
    get_description(this, retVal);
    return result_t();
}

result_t HeapGraphNode::get_id(int32_t& retVal)
{
    retVal = m_id;
    return result_t();
}

result_t HeapGraphNode::get_shallowSize(int32_t& retVal)
{
    retVal = m_shallowSize;
    return result_t();
}

result_t HeapGraphNode::get_childs(std::vector<obj_ptr<HeapGraphEdge_base>>& retVal)
{
    retVal = m_childs;
    return result_t();
}
}
