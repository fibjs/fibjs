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

HeapGraphNode::HeapGraphNode(HeapSnapshot* snapshot, const v8::HeapGraphNode* graphnode)
{
	m_id = (int32_t)graphnode->GetId();
	m_type = (int32_t)graphnode->GetType();

	v8::Local<v8::Value> v = graphnode->GetName();
	GetArgumentValue(v, m_name);
	m_shallowSize = (int32_t)graphnode->GetShallowSize();

	m_childs = new List();

	int32_t cnt = graphnode->GetChildrenCount();
	int32_t i;

	for (i = 0; i < cnt; i ++)
		m_childs->append(new HeapGraphEdge(snapshot, graphnode->GetChild(i)));
}

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
