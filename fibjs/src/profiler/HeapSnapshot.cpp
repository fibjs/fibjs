/*
 * HeapSnapshot.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "object.h"
#include "HeapSnapshot.h"
#include "HeapGraphNode.h"
#include "HeapGraphEdge.h"
#include "HeapProxy.h"
#include "ifs/profiler.h"
#include "File.h"
#include "ifs/fs.h"
#include "ifs/global.h"
#include "ifs/encoding.h"
#include "StringBuffer.h"

namespace fibjs {

DECLARE_MODULE(profiler);

class BufferStream : public v8::OutputStream {
public:
    virtual void EndOfStream()
    {
    }

    virtual WriteResult WriteAsciiChunk(char* data, int size)
    {
        m_buf.append(data, size);
        return kContinue;
    }

    exlib::string result()
    {
        return m_buf.str();
    }

private:
    StringBuffer m_buf;
};

result_t profiler_base::takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal)
{
    global_base::GC();

    v8::HeapProfiler* profiler = Isolate::current()->m_isolate->GetHeapProfiler();
    retVal = new HeapSnapshotProxy(profiler->TakeHeapSnapshot());
    return 0;
}

result_t profiler_base::diff(v8::Local<v8::Function> test, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::HeapProfiler* profiler = isolate->m_isolate->GetHeapProfiler();
    obj_ptr<HeapSnapshot_base> s1, s2;

    global_base::GC();
    s1 = new HeapSnapshotProxy(profiler->TakeHeapSnapshot());

    test->Call(v8::Undefined(isolate->m_isolate), 0, NULL);

    global_base::GC();
    s2 = new HeapSnapshotProxy(profiler->TakeHeapSnapshot());

    return s2->diff(s1, retVal);
}

result_t profiler_base::saveSnapshot(exlib::string fname)
{
    obj_ptr<HeapSnapshot_base> snapshot;
    takeSnapshot(snapshot);
    return snapshot->cc_save(fname);
}

result_t profiler_base::loadSnapshot(exlib::string fname, obj_ptr<HeapSnapshot_base>& retVal)
{
    result_t hr;

    obj_ptr<HeapSnapshot> hs = new HeapSnapshot();
    hr = hs->load(fname);
    if (hr < 0)
        return hr;

    retVal = hs;
    return 0;
}

result_t HeapSnapshot::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
    std::map<int32_t, int32_t>::iterator it;

    it = _nodes.find(id);
    if (it != _nodes.end()) {
        Variant v;

        m_nodes->_indexed_getter(it->second, v);
        retVal = (HeapGraphNode*)v.object();
    } else
        return CALL_RETURN_NULL;

    return 0;
}

template <typename T>
inline result_t GetArray(v8::Local<v8::Value> v, QuickArray<T>& n)
{
    if (v.IsEmpty() || !v->IsArray())
        return CALL_E_INVALIDARG;

    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
    result_t hr;

    for (int32_t i = 0; i < (int32_t)a->Length(); i++) {
        T vr;
        hr = GetArgumentValue(a->Get(i), vr, true);
        if (hr < 0)
            return hr;

        n.append(vr);
    }

    return 0;
}

inline bool checkArray(QuickArray<exlib::string>& a, const char* chks[], int32_t sz)
{
    int32_t i;

    if ((int32_t)a.size() < sz)
        return false;

    for (i = 0; i < sz; i++)
        if (a[i] == chks[i])
            return false;

    return true;
}

inline bool is_num_type(int32_t _type)
{
    return _type == profiler_base::_Edge_Element || _type == profiler_base::_Edge_Hidden;
}

result_t HeapSnapshot::load(exlib::string fname)
{
    Isolate* isolate = holder();
    result_t hr;
    v8::Local<v8::Value> v;
    v8::Local<v8::Object> o;

    QuickArray<int32_t> nodes;
    QuickArray<int32_t> edges;
    QuickArray<exlib::string> names;
    QuickArray<exlib::string> node_fields;
    QuickArray<exlib::string> node_types;
    QuickArray<exlib::string> edge_fields;
    QuickArray<exlib::string> edge_types;
    int32_t node_count, edge_count;
    static const char* node_fields_chk[] = { "type", "name", "id", "self_size", "edge_count" };
    static const char* node_types_chk[] = { "hidden", "array", "string", "object",
        "code", "closure", "regexp", "number",
        "native", "synthetic", "concatenated string",
        "sliced string" };
    static const char* edge_fields_chk[] = { "type", "name_or_index", "to_node" };
    static const char* edge_types_chk[] = { "context", "element", "property",
        "internal", "hidden", "shortcut", "weak" };

    exlib::string data;
    hr = fs_base::ac_readTextFile(fname, data);
    if (hr < 0)
        return hr;

    hr = json_base::decode(data, v);
    if (hr < 0)
        return hr;
    data.resize(0);

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    o = v8::Local<v8::Object>::Cast(v);
    hr = GetArray(o->Get(isolate->NewString("nodes")),
        nodes);
    if (hr < 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetArray(o->Get(isolate->NewString("edges")),
        edges);
    if (hr < 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetArray(o->Get(isolate->NewString("strings")),
        names);
    if (hr < 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    v = o->Get(isolate->NewString("snapshot"));
    if (v.IsEmpty() || !v->IsObject())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    o = v8::Local<v8::Object>::Cast(v);
    hr = GetConfigValue(isolate->m_isolate, o, "node_count", node_count);
    if (hr < 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetConfigValue(isolate->m_isolate, o, "edge_count", edge_count);
    if (hr < 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    v = o->Get(isolate->NewString("meta"));
    if (v.IsEmpty() || !v->IsObject())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    o = v8::Local<v8::Object>::Cast(v);
    hr = GetArray(o->Get(isolate->NewString("node_fields")),
        node_fields);
    if (hr < 0 || checkArray(node_fields, node_fields_chk, ARRAYSIZE(node_fields_chk)))
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetArray(o->Get(isolate->NewString("edge_fields")),
        edge_fields);
    if (hr < 0 || checkArray(edge_fields, edge_fields_chk, ARRAYSIZE(edge_fields_chk)))
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    if (node_fields.size() * node_count != nodes.size())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    if (edge_fields.size() * edge_count != edges.size())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    v = o->Get(isolate->NewString("node_types"));
    if (v.IsEmpty() || !v->IsArray())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetArray(v8::Local<v8::Array>::Cast(v)->Get(0), node_types);
    if (hr < 0 || checkArray(node_types, node_types_chk, ARRAYSIZE(node_types_chk)))
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    v = o->Get(isolate->NewString("edge_types"));
    if (v.IsEmpty() || !v->IsArray())
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    hr = GetArray(v8::Local<v8::Array>::Cast(v)->Get(0), edge_types);
    if (hr < 0 || checkArray(edge_types, edge_types_chk, ARRAYSIZE(edge_types_chk)))
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    int32_t node_pos = 0, edge_pos = 0;

    m_nodes = new List();
    while (node_pos < node_count) {
        int32_t _base = node_pos * (int32_t)node_fields.size();
        int32_t _node_type = nodes[_base];
        int32_t _node_name_id = nodes[_base + 1];
        if (_node_name_id < 0 || _node_name_id >= (int32_t)names.size())
            return CHECK_ERROR(CALL_E_INVALID_DATA);
        exlib::string _node_name = names[_node_name_id];
        int32_t _node_id = nodes[_base + 2];
        int32_t _node_size = nodes[_base + 3];
        int32_t _node_edge = nodes[_base + 4];
        obj_ptr<List> _edges = new List();

        if (edge_pos + _node_edge > edge_count)
            return CHECK_ERROR(CALL_E_INVALID_DATA);

        while (_node_edge--) {
            int32_t _base = edge_pos * (int32_t)edge_fields.size();
            int32_t _edge_type = edges[_base];
            int32_t _edge_name_id = edges[_base + 1];
            int32_t _edge_toid = edges[_base + 2];
            exlib::string _edge_name;

            if (is_num_type(_edge_type)) {
                char buf[64];

                sprintf(buf, "%d", _edge_name_id);
                _edge_name = buf;
            } else
                _edge_name = names[_edge_name_id];

            if (_edge_toid % node_fields.size() != 0 || _edge_toid >= (int32_t)edges.size())
                return CHECK_ERROR(CALL_E_INVALID_DATA);
            _edge_toid = nodes[_edge_toid + 2];

            obj_ptr<HeapGraphEdge> _edge = new HeapGraphEdge(this, _edge_type,
                _edge_name, _node_id, _edge_toid);
            _edges->append(_edge);

            edge_pos++;
        }

        _edges->freeze();

        obj_ptr<HeapGraphNode> _node = new HeapGraphNode(_node_type,
            _node_name, _node_id, _node_size, _edges);

        _nodes.insert(std::pair<int32_t, int32_t>(_node_id, node_pos));
        m_nodes->append(_node);

        node_pos++;
    }

    m_nodes->freeze();

    return 0;
}

#define BUF_SIZE 8192
result_t HeapSnapshot::save(exlib::string fname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    class buf_file {
    public:
        buf_file()
            : file(new File())
        {
        }

        ~buf_file()
        {
            flush();
        }

    public:
        result_t open(exlib::string fname)
        {
            return file->open(fname, "w");
        }

        result_t append(exlib::string& s)
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
            exlib::string str = bufs.str();
            return file->Write(str);
        }

    public:
        StringBuffer bufs;
        obj_ptr<File> file;
    };

    class name_ids {
    public:
        name_ids()
        {
            id("<dummy>");
        }

    public:
        int32_t id(exlib::string _name)
        {
            std::map<exlib::string, int32_t>::iterator it;
            int32_t _name_id;

            it = mapNames.find(_name);
            if (it == mapNames.end()) {
                _name_id = (int32_t)names.size();
                mapNames.insert(std::pair<exlib::string, int32_t>(_name, _name_id));
                names.append(_name);
            } else
                _name_id = it->second;

            return _name_id;
        }

    public:
        std::map<exlib::string, int32_t> mapNames;
        QuickArray<exlib::string> names;
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
    for (i = 0; i < count; i++) {
        Variant v;
        HeapGraphNode* cur;

        m_nodes->_indexed_getter(i, v);
        cur = (HeapGraphNode*)v.object();

        nodes.append(cur);

        cur->get_childs(childs);

        childs->get_length(n);
        child_count += n;
    }

    exlib::string str;
    char buf[128];

    n = sprintf(buf, "\"node_count\":%d,\"edge_count\":%d,"
                     "\"trace_function_count\":0},\n\"nodes\":[",
        count, child_count);
    hr = bufs.append(buf, n);
    if (hr < 0)
        return hr;

    for (i = 0; i < count; i++) {
        int32_t _id, _type, _name_id, _size, _child;
        exlib::string _name;
        HeapGraphNode_base* cur = nodes[i];

        cur->get_id(_id);
        cur->get_type(_type);
        cur->get_shallowSize(_size);
        cur->get_name(_name);
        _name_id = _ids.id(_name);

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

    for (i = 0; i < count; i++) {
        int32_t _type, _name_id, _child, _toid, _toindex;
        exlib::string _name;
        HeapGraphNode_base* cur = nodes[i];

        cur->get_childs(childs);
        childs->get_length(_child);

        for (int32_t j = 0; j < _child; j++) {
            Variant v;
            HeapGraphEdge* edge;

            childs->_indexed_getter(j, v);
            edge = (HeapGraphEdge*)v.object();

            edge->get_type(_type);
            edge->get_name(_name);

            if (is_num_type(_type))
                _name_id = atoi(_name.c_str());
            else
                _name_id = _ids.id(_name);

            _toid = edge->toid();
            _toindex = _nodes.find(_toid)->second * 6;

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
    for (i = 0; i < count; i++) {
        if (i == 0)
            hr = bufs.append('\"');
        else
            hr = bufs.append("\",\n\"", 4);
        if (hr < 0)
            return hr;

        str.resize(0);
        encoding_base::jsstr(_ids.names[i], true, str);
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
}
