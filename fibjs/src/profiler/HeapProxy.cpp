/*
 * HeapProxy.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "HeapProxy.h"
#include "HeapSnapshot.h"
#include "StringBuffer.h"
#include "ifs/fs.h"

namespace fibjs
{

result_t HeapSnapshotProxy::diff(HeapSnapshot_base* before,
                                 v8::Local<v8::Object>& retVal)
{
	return HeapSnapshot::diff(before, this, retVal);
}

void HeapSnapshotProxy::fill_nodes()
{
	if (m_nodes == 0)
	{
		m_nodes = new List();
		int32_t cnt = m_snapshot->GetNodesCount();

		for (int32_t i = 0; i < cnt; i ++)
		{
			const v8::HeapGraphNode* _node;

			_node = m_snapshot->GetNode(i);
			_nodes.insert(std::pair<int32_t, int32_t>(_node->GetId(), i));
			m_nodes->append(new HeapGraphNodeProxy(this, _node));
		}
	}
}

result_t HeapSnapshotProxy::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
	fill_nodes();

	std::map<int32_t, int32_t>::iterator it;

	it = _nodes.find(id);
	if (it == _nodes.end())
		return CALL_RETURN_NULL;

	Variant v;

	m_nodes->_indexed_getter(it->second, v);
	retVal = (HeapGraphNode_base*)v.object();

	return 0;
}

result_t HeapSnapshotProxy::save(const char* fname, AsyncEvent* ac)
{
	class BufferStream : public v8::OutputStream {
	public:
		virtual void EndOfStream()
		{}

		virtual WriteResult WriteAsciiChunk(char* data, int size)
		{
			m_buf.append(data, size);
			return kContinue;
		}

		std::string result()
		{
			return m_buf.str();
		}

	private:
		StringBuffer m_buf;
	};

	BufferStream bs;
	m_snapshot->Serialize(&bs);

	return fs_base::ac_writeFile(fname, bs.result().c_str());
}

result_t HeapSnapshotProxy::get_time(date_t& retVal)
{
	retVal = m_d;
	return 0;
}

result_t HeapSnapshotProxy::get_root(obj_ptr<HeapGraphNode_base>& retVal)
{
	retVal = new HeapGraphNodeProxy(this, m_snapshot->GetRoot());
	return 0;
}

result_t HeapSnapshotProxy::get_nodes(obj_ptr<List_base>& retVal)
{
	fill_nodes();
	retVal = m_nodes;
	return 0;
}

result_t HeapGraphNodeProxy::get_type(int32_t& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = m_node->GetType();
	return 0;
}

result_t HeapGraphNodeProxy::get_name(std::string& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	return GetArgumentValue(m_node->GetName(), retVal);
}

result_t HeapGraphNodeProxy::get_id(int32_t& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = m_node->GetId();
	return 0;
}

result_t HeapGraphNodeProxy::get_shallowSize(int32_t& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = (int32_t)m_node->GetShallowSize();
	return 0;
}

result_t HeapGraphNodeProxy::get_childs(obj_ptr<List_base>& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	if (m_childs == 0)
	{
		m_childs = new List();
		int32_t cnt = m_node->GetChildrenCount();

		for (int32_t i = 0; i < cnt; i ++)
			m_childs->append(new HeapGraphEdgeProxy(m_snapshot, m_node->GetChild(i)));
	}

	retVal = m_childs;
	return 0;
}

result_t HeapGraphEdgeProxy::get_type(int32_t& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = m_edge->GetType();
	return 0;
}

result_t HeapGraphEdgeProxy::get_name(std::string& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	return GetArgumentValue(m_edge->GetName(), retVal);
}

result_t HeapGraphEdgeProxy::getFromNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = new HeapGraphNodeProxy(m_snapshot, m_edge->GetFromNode());
	return 0;
}

result_t HeapGraphEdgeProxy::getToNode(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (m_snapshot == 0)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = new HeapGraphNodeProxy(m_snapshot, m_edge->GetToNode());
	return 0;
}

}
