/*
 * HeapSnapshot.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "HeapSnapshot.h"
#include "HeapGraphNode.h"
#include "HeapGraphEdge.h"
#include "ifs/profiler.h"
#include "File.h"
#include "ifs/fs.h"
#include "ifs/encoding.h"
#include "StringBuffer.h"

namespace fibjs
{

result_t profiler_base::takeSnapshot(bool debug, obj_ptr<HeapSnapshot_base>& retVal)
{
	v8::HeapProfiler* profiler = Isolate::now()->m_isolate->GetHeapProfiler();

	assert(profiler != 0);
	assert(profiler->GetSnapshotCount() == 0);

	retVal = new HeapSnapshot(profiler->TakeHeapSnapshot(), debug);
	profiler->DeleteAllHeapSnapshots();

	return 0;
}

result_t profiler_base::diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal)
{
	obj_ptr<HeapSnapshot_base> s1, s2;

	takeSnapshot(false, s1);
	test->Call(v8::Undefined(Isolate::now()->m_isolate), 0, NULL);
	takeSnapshot(false, s2);

	return s2->diff(s1, retVal);
}

HeapSnapshot::HeapSnapshot(const v8::HeapSnapshot* snapshot, bool debug)
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

	m_time.now();

	m_nodes = new List();

	int32_t cnt = snapshot->GetNodesCount();
	int32_t i;

	for (i = 0; i < cnt; i ++) {
		const v8::HeapGraphNode* graphnode = snapshot->GetNode(i);
		int32_t id = (int32_t)graphnode->GetId();

		obj_ptr<HeapGraphNode> pNode = new HeapGraphNode(this, graphnode);
		_nodes.insert(std::pair<int32_t, int32_t>(id, i));
		m_nodes->append(pNode);
	}

	if (debug)
	{
		BufferStream bs;
		snapshot->Serialize(&bs);
		m_serialize = bs.result();
	}
}

result_t HeapSnapshot::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
	std::map<int32_t, int32_t>::iterator it;

	it = _nodes.find(id);
	if (it != _nodes.end())
	{
		Variant v;

		m_nodes->_indexed_getter(it->second, v);
		retVal = (HeapGraphNode*)v.object();
	}
	else
		return CALL_RETURN_NULL;

	return 0;
}

#define BUF_SIZE	1024
result_t HeapSnapshot::write(const char* fname, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	class buf_file
	{
	public:
		buf_file() : file(new File())
		{
		}

		~buf_file()
		{
			flush();
		}

	public:
		result_t open(const char* fname)
		{
			return file->open(fname, "w");
		}

		result_t append(std::string& s)
		{
			bufs.append(s);
			if (bufs.size() > BUF_SIZE)
				return flush();
			return 0;
		}

		result_t append(const char* s, int32_t l = -1)
		{
			bufs.append(s, l);
			if (bufs.size() > BUF_SIZE)
				return flush();
			return 0;
		}

		result_t append(char ch)
		{
			bufs.append(ch);
			if (bufs.size() > BUF_SIZE)
				return flush();
			return 0;
		}

		result_t flush()
		{
			std::string str = bufs.str();
			return file->Write(str.c_str(), (int32_t)str.length());
		}

	public:
		StringBuffer bufs;
		obj_ptr<File> file;
	};

	class name_ids
	{
	public:
		name_ids()
		{
			id("<dummy>");
		}

	public:
		int32_t id(std::string _name)
		{
			std::map<std::string, int32_t>::iterator it;
			int32_t _name_id;

			it = mapNames.find(_name);
			if (it == mapNames.end())
			{
				_name_id = (int32_t)names.size();
				mapNames.insert(std::pair<std::string, int32_t>(_name, _name_id));
				names.append(_name);
			} else
				_name_id = it->second;

			return _name_id;
		}

	public:
		std::map<std::string, int32_t> mapNames;
		QuickArray<std::string> names;
	};

	static char meta[] = "{\"snapshot\":{\"meta\":{\"node_fields\":"
	                     "[\"type\",\"name\",\"id\",\"self_size\","
	                     "\"edge_count\",\"trace_node_id\"],"
	                     "\"node_types\":[[\"hidden\",\"array\","
	                     "\"string\",\"object\",\"code\",\"closure\","
	                     "\"regexp\",\"number\",\"native\",\"synthetic\","
	                     "\"concatenated string\",\"sliced string\"],"
	                     "\"string\",\"number\",\"number\",\"number\","
	                     "\"number\",\"number\"],\"edge_fields\":"
	                     "[\"type\",\"name_or_index\",\"to_node\"],"
	                     "\"edge_types\":[[\"context\",\"element\","
	                     "\"property\",\"internal\",\"hidden\","
	                     "\"shortcut\",\"weak\"],\"string_or_number\","
	                     "\"node\"],\"trace_function_info_fields\":"
	                     "[\"function_id\",\"name\",\"script_name\","
	                     "\"script_id\",\"line\",\"column\"],"
	                     "\"trace_node_fields\":[\"id\",\"function_info_index\","
	                     "\"count\",\"size\",\"children\"],\"sample_fields\":"
	                     "[\"timestamp_us\",\"last_assigned_id\"]},";

	name_ids _ids;
	std::map<int32_t, int32_t> mapNodes;
	QuickArray<HeapGraphNode_base*> nodes;
	obj_ptr<List_base> childs;
	buf_file bufs;
	int32_t count, child_count = 0;
	int32_t n;
	int32_t i;
	result_t hr;

	hr = bufs.open(fname);
	if (hr < 0)
		return hr;

	hr = bufs.file->Write(meta, sizeof(meta) - 1);
	if (hr < 0)
		return hr;

	m_nodes->get_length(count);
	for (i = 0; i < count; i ++)
	{
		Variant v;
		HeapGraphNode* cur;

		m_nodes->_indexed_getter(i, v);
		cur = (HeapGraphNode*)v.object();

		nodes.append(cur);

		cur->get_childs(childs);

		childs->get_length(n);
		child_count += n;
	}

	std::string str;
	char buf[128];

	n = sprintf(buf, "\"node_count\":%d,\"edge_count\":%d,"
	            "\"trace_function_count\":0},\n\"nodes\":[", count, child_count);
	hr = bufs.append(buf, n);
	if (hr < 0)
		return hr;

	for (i = 0; i < count; i ++)
	{
		int32_t _id, _type, _name_id, _size, _child;
		std::string _name;
		HeapGraphNode_base* cur = nodes[i];

		cur->get_id(_id);
		cur->get_type(_type);
		cur->get_shallowSize(_size);
		cur->get_name(_name);
		_name_id = _ids.id(_name);

		mapNodes.insert(std::pair<int32_t, int32_t>(_id, i * 6));

		cur->get_childs(childs);
		childs->get_length(_child);

		if (i == 0)
			n = sprintf(buf, "%d,%d,%d,%d,%d,0\n", _type, _name_id, _id, _size, _child);
		else
			n = sprintf(buf, ",%d,%d,%d,%d,%d,0\n", _type, _name_id, _id, _size, _child);
		hr = bufs.append(buf, n);
		if (hr < 0)
			return hr;
	}

	hr = bufs.append("],\n\"edges\":[");
	if (hr < 0)
		return hr;

	for (i = 0; i < count; i ++)
	{
		int32_t _type, _name_id, _child, _toid, _toindex;
		std::string _name;
		HeapGraphNode_base* cur = nodes[i];

		cur->get_childs(childs);
		childs->get_length(_child);

		for (int32_t j = 0; j < _child; j ++)
		{
			Variant v;
			HeapGraphEdge* edge;

			childs->_indexed_getter(j, v);
			edge = (HeapGraphEdge*)v.object();

			edge->get_type(_type);
			edge->get_name(_name);

			if (_type == HeapGraphEdge_base::_Element ||
			        _type == HeapGraphEdge_base::_Hidden)
				_name_id = atoi(_name.c_str());
			else
				_name_id = _ids.id(_name);

			_toid = edge->toid();
			_toindex = mapNodes.find(_toid)->second;

			if (i == 0 && j == 0)
				n = sprintf(buf, "%d,%d,%d\n", _type, _name_id, _toindex);
			else
				n = sprintf(buf, ",%d,%d,%d\n", _type, _name_id, _toindex);
			hr = bufs.append(buf, n);
			if (hr < 0)
				return hr;
		}
	}

	hr = bufs.append("],\n\"trace_function_infos\":[],\n\"trace_tree\":[],\n"
	                 "\"samples\":[],\n\"strings\":[");
	if (hr < 0)
		return hr;

	count = (int32_t)_ids.names.size();
	for (i = 0; i < count; i ++)
	{
		if (i == 0)
			hr = bufs.append('\"');
		else
			hr = bufs.append("\",\n\"", 4);
		if (hr < 0)
			return hr;

		str.resize(0);
		encoding_base::jsstr(_ids.names[i].c_str(), true, str);
		hr = bufs.append(str);
		if (hr < 0)
			return hr;
	}

	hr = bufs.append("\"]}", 3);
	if (hr < 0)
		return hr;
	return 0;
}

result_t HeapSnapshot::get_time(date_t& retVal)
{
	retVal = m_time;
	return 0;
}

result_t HeapSnapshot::get_root(obj_ptr<HeapGraphNode_base>& retVal)
{
	return getNodeById(1, retVal);
}

result_t HeapSnapshot::get_nodes(obj_ptr<List_base>& retVal)
{
	retVal = m_nodes;
	return 0;
}

result_t HeapSnapshot::get_serialize(std::string& retVal)
{
	retVal = m_serialize;
	return 0;
}

}
