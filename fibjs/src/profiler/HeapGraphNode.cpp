/*
 * HeapGraphNode.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "HeapGraphNode.h"
#include "HeapGraphEdge.h"
#include "HeapSnapshot.h"

namespace fibjs
{

result_t HeapGraphNode::get_type(int32_t& retVal)
{
	retVal = m_type;
	return 0;
}

result_t HeapGraphNode::get_name(std::string& retVal)
{
	retVal = m_name;
	return 0;
}

result_t HeapGraphNode::get_description(std::string& retVal)
{
	get_description(this, retVal);
	return 0;
}

result_t HeapGraphNode::get_id(int32_t& retVal)
{
	retVal = m_id;
	return 0;
}

result_t HeapGraphNode::get_shallowSize(int32_t& retVal)
{
	retVal = m_shallowSize;
	return 0;
}

result_t HeapGraphNode::get_childs(obj_ptr<List_base>& retVal)
{
	retVal = m_childs;
	return 0;
}

}
