/*
 * HeapGraphEdge.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "object.h"
#include "HeapGraphEdge.h"
#include "HeapGraphNode.h"
#include "HeapSnapshot.h"

namespace fibjs
{

result_t HeapGraphEdge::get_type(int32_t& retVal)
{
	retVal = m_type;
	return 0;
}

result_t HeapGraphEdge::get_name(exlib::string& retVal)
{
	retVal = m_name;
	return 0;
}

result_t HeapGraphEdge::get_description(exlib::string& retVal)
{
	get_description(this, retVal);
	return 0;
}

result_t HeapGraphEdge::getFromNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!m_snapshot)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_snapshot->getNodeById(m_fromId, retVal);
}

result_t HeapGraphEdge::getToNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!m_snapshot)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_snapshot->getNodeById(m_toId, retVal);
}

}
