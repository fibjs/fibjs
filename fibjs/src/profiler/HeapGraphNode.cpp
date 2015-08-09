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

inline std::string handleToStr(const v8::Local<v8::Value> & str)
{
	v8::String::Utf8Value utfString(str->ToString());
	return *utfString;
}

result_t HeapGraphNode::get_type(std::string& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	switch (m_graphnode->GetType()) {
	case v8::HeapGraphNode::kArray :
		retVal = "Array";
		break;
	case v8::HeapGraphNode::kString :
		retVal = "String";
		break;
	case v8::HeapGraphNode::kObject :
		retVal = handleToStr(m_graphnode->GetName());
		retVal = "Object";
		break;
	case v8::HeapGraphNode::kCode :
		retVal = "Code";
		break;
	case v8::HeapGraphNode::kClosure :
		retVal = "Closure";
		break;
	case v8::HeapGraphNode::kRegExp :
		retVal = "RegExp";
		break;
	case v8::HeapGraphNode::kHeapNumber :
		retVal = "HeapNumber";
		break;
	case v8::HeapGraphNode::kNative :
		retVal = "Native";
		break;
	case v8::HeapGraphNode::kSynthetic :
		retVal = "Synthetic";
		break;
	case v8::HeapGraphNode::kConsString :
		retVal = "ConsString";
		break;
	case v8::HeapGraphNode::kSlicedString :
		retVal = "SlicedString";
		break;
	default :
		retVal = "Hidden";
	}

	return 0;
}

result_t HeapGraphNode::get_name(std::string& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	v8::Local<v8::Value> v = m_graphnode->GetName();
	return GetArgumentValue(v, retVal);
}

result_t HeapGraphNode::get_id(int32_t& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_graphnode->GetId();

	return 0;
}

result_t HeapGraphNode::get_shallowSize(int32_t& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_graphnode->GetShallowSize();
	return 0;
}

result_t HeapGraphNode::get_childsCount(int32_t& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_graphnode->GetChildrenCount();
	return 0;
}

result_t HeapGraphNode::get_childs(obj_ptr<List_base>& retVal)
{
	if (!m_snapshot->is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (m_childs == 0)
	{
		m_childs = new List();

		int32_t cnt = m_graphnode->GetChildrenCount();
		int32_t i;

		for (i = 0; i < cnt; i ++)
			m_childs->append(m_snapshot->Edge(m_graphnode->GetChild(i)));
	}

	retVal = m_childs;

	return 0;
}

}
