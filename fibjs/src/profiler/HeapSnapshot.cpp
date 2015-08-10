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
	Isolate* isolate = Isolate::now();
	const v8::HeapSnapshot* snapshot;

	snapshot = isolate->m_isolate->GetHeapProfiler()->TakeHeapSnapshot();
	retVal = new HeapSnapshot(snapshot);

	return 0;
}

result_t profiler_base::DeleteAllHeapSnapshots()
{
	Isolate* isolate = Isolate::now();
	const v8::HeapSnapshot* snapshot;

	isolate->m_isolate->GetHeapProfiler()->DeleteAllHeapSnapshots();
	return 0;
}

result_t HeapSnapshot::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
	retVal = new HeapGraphNode(m_snapshot->GetNodeById(id));
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

	BufferStream bs;

	m_snapshot->Serialize(&bs);
	retVal = bs.result();

	return 0;
}

result_t HeapSnapshot::get_time(date_t& retVal)
{
	retVal = m_time;
	return 0;
}

result_t HeapSnapshot::get_nodesCount(int32_t& retVal)
{
	retVal = m_snapshot->GetNodesCount();
	return 0;
}

result_t HeapSnapshot::get_root(obj_ptr<HeapGraphNode_base>& retVal)
{
	retVal = new HeapGraphNode(m_snapshot->GetRoot());
	return 0;
}

result_t HeapSnapshot::get_nodes(obj_ptr<List_base>& retVal)
{
	if (m_nodes == 0)
	{
		m_nodes = new List();

		int32_t cnt = m_snapshot->GetNodesCount();
		int32_t i;

		for (i = 0; i < cnt; i ++)
			m_nodes->append(new HeapGraphNode(m_snapshot->GetNode(i)));
	}

	retVal = m_nodes;
	return 0;
}

}
