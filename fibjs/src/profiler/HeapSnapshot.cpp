/*
 * HeapSnapshot.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "HeapSnapshot.h"
#include "HeapGraphNode.h"
#include "ifs/profiler.h"
#include "StringBuffer.h"

namespace fibjs
{

result_t profiler_base::takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal)
{
	retVal = new HeapSnapshot(Isolate::now()->m_isolate->GetHeapProfiler()->TakeHeapSnapshot());
	return 0;
}

result_t profiler_base::diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal)
{
	obj_ptr<HeapSnapshot_base> s1, s2;

	takeSnapshot(s1);
	test->Call(v8::Undefined(Isolate::now()->m_isolate), 0, NULL);
	takeSnapshot(s2);

	return s2->diff(s1, retVal);
}

result_t HeapSnapshot::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = Node(m_snapshot->GetNodeById(id));

	return 0;
}

result_t HeapSnapshot::_delete()
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	m_snapshot->Delete();
	m_snapshot = NULL;

	for (std::map<const v8::HeapGraphNode*, obj_ptr<HeapGraphNode> >::iterator it = _nodes.begin(); it != _nodes.end(); ++it)
		it->second->disable();
	_nodes.clear();

	for (std::map<const v8::HeapGraphEdge*, obj_ptr<HeapGraphEdge> >::iterator it = _edges.begin(); it != _edges.end(); ++it)
		it->second->disable();
	_edges.clear();

	return 0;
}

result_t HeapSnapshot::serialize(std::string& retVal)
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

	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	BufferStream bs;

	m_snapshot->Serialize(&bs);
	retVal = bs.result();

	return 0;
}

result_t HeapSnapshot::get_time(date_t& retVal)
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_time;
	return 0;
}

result_t HeapSnapshot::get_nodesCount(int32_t& retVal)
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = m_snapshot->GetNodesCount();
	return 0;
}

result_t HeapSnapshot::get_root(obj_ptr<HeapGraphNode_base>& retVal)
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	retVal = Node(m_snapshot->GetRoot());
	return 0;
}

result_t HeapSnapshot::get_nodes(obj_ptr<List_base>& retVal)
{
	if (!is_alive())
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (m_nodes == 0)
	{
		m_nodes = new List();

		int32_t cnt = m_snapshot->GetNodesCount();
		int32_t i;

		for (i = 0; i < cnt; i ++)
			m_nodes->append(Node(m_snapshot->GetNode(i)));
	}

	retVal = m_nodes;
	return 0;
}

}
