/*
 * HeapGraphEdge.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "HeapGraphEdge.h"
#include "HeapGraphNode.h"
#include "HeapSnapshot.h"

namespace fibjs
{

result_t HeapGraphEdge::get_type(std::string& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	switch (m_graphedge->GetType()) {
	case v8::HeapGraphEdge::kContextVariable :
		retVal = "ContextVariable";
		break;
	case v8::HeapGraphEdge::kElement :
		retVal = "Element";
		break;
	case v8::HeapGraphEdge::kProperty :
		retVal = "Property";
		break;
	case v8::HeapGraphEdge::kInternal :
		retVal = "Internal";
		break;
	case v8::HeapGraphEdge::kHidden :
		retVal = "Hidden";
		break;
	case v8::HeapGraphEdge::kShortcut :
		retVal = "Shortcut";
		break;
	case v8::HeapGraphEdge::kWeak :
		retVal = "Weak";
		break;
	default :
		retVal = "Undefined";
	}

	return 0;
}

result_t HeapGraphEdge::get_name(std::string& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	v8::Local<v8::Value> v = m_graphedge->GetName();
	return GetArgumentValue(v, retVal);
}

result_t HeapGraphEdge::getFromNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_snapshot->Node(m_graphedge->GetFromNode());

	return 0;
}

result_t HeapGraphEdge::getToNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_snapshot->Node(m_graphedge->GetToNode());
	return 0;
}

}
