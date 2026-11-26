/*
 * HeapProxy.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "object.h"
#include "HeapProxy.h"
#include "HeapSnapshot.h"
#include "HeapGraphNode.h"
#include "HeapGraphEdge.h"
#include "FileStream.h"

namespace fibjs {

result_t HeapSnapshotProxy::diff(HeapSnapshot_base* before,
    obj_ptr<DiffType>& retVal)
{
    return HeapSnapshot::diff(before, this, retVal);
}

void HeapSnapshotProxy::fill_nodes()
{
    if (!m_nodes_filled) {
        m_nodes_filled = true;

        int32_t cnt = m_snapshot->GetNodesCount();

        for (int32_t i = 0; i < cnt; i++) {
            const v8::HeapGraphNode* _node;

            _node = m_snapshot->GetNode(i);
            _nodes.insert(std::pair<int32_t, int32_t>(_node->GetId(), i));
            m_nodes.push_back(new HeapGraphNodeProxy(this, _node));
        }
    }
}

result_t HeapSnapshotProxy::getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal)
{
    fill_nodes();

    std::unordered_map<int32_t, int32_t>::iterator it;

    it = _nodes.find(id);
    if (it == _nodes.end())
        return result_t::return_null();

    retVal = m_nodes[it->second];

    return result_t();
}

result_t HeapSnapshotProxy::save(exlib::string fname, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    // fs.writeTextFile enforced the worker's file-system switch; the stream is
    // opened directly here, so the same check has to be made explicitly. The
    // cc_ entry carries no isolate because it may run outside one; the static
    // v8.saveSnapshot checks its own isolate before reaching this point.
    if (ac.callType() != AsyncEvent::kCAsyncCall
        && !ac.isolate()->m_enable_FileSystem)
        return result_t::invalid_call();

    // The serialized graph is written chunk by chunk instead of being collected
    // into one string: a snapshot of a large heap is hundreds of MB, and the
    // StringBuffer plus the string and Buffer copies that
    // fs.writeTextFile would make exhausted the 32-bit address space. The
    // loaded-snapshot writer (HeapSnapshot::save) streams the same way.
    class FileStreamOutput : public v8::OutputStream {
    public:
        FileStreamOutput(FileStream* file)
            : m_file(file)
        {
        }

        virtual void EndOfStream()
        {
        }

        virtual WriteResult WriteAsciiChunk(char* data, int size)
        {
            result_t hr = m_file->Write(data, size);
            if (hr.failed()) {
                if (!m_hr.failed())
                    m_hr = hr;
                return kAbort;
            }
            return kContinue;
        }

        result_t result() const
        {
            return m_hr;
        }

    private:
        FileStream* m_file;
        result_t m_hr;
    };

    obj_ptr<FileStream> file = new FileStream();
    result_t hr = file->open(fname, "w");
    if (hr.failed())
        return hr;

    FileStreamOutput out(file);
    m_snapshot->Serialize(&out);
    file->close();

    return out.result();
}

result_t HeapSnapshotProxy::get_time(date_t& retVal)
{
    retVal = m_d;
    return result_t();
}

result_t HeapSnapshotProxy::get_root(obj_ptr<HeapGraphNode_base>& retVal)
{
    retVal = new HeapGraphNodeProxy(this, m_snapshot->GetRoot());
    return result_t();
}

result_t HeapSnapshotProxy::get_nodes(std::vector<obj_ptr<HeapGraphNode_base>>& retVal)
{
    fill_nodes();
    retVal = m_nodes;
    return result_t();
}

result_t HeapGraphNodeProxy::get_type(int32_t& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = m_node->GetType();
    return result_t();
}

result_t HeapGraphNodeProxy::get_name(exlib::string& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    return GetArgumentValue(holder(), m_node->GetName(), retVal);
}

result_t HeapGraphNodeProxy::get_description(exlib::string& retVal)
{
    HeapGraphNode::get_description(this, retVal);
    return result_t();
}

result_t HeapGraphNodeProxy::get_id(int32_t& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = m_node->GetId();
    return result_t();
}

result_t HeapGraphNodeProxy::get_shallowSize(int32_t& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = (int32_t)m_node->GetShallowSize();
    return result_t();
}

result_t HeapGraphNodeProxy::get_childs(std::vector<obj_ptr<HeapGraphEdge_base>>& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    if (!m_childs_filled) {
        m_childs_filled = true;

        int32_t cnt = m_node->GetChildrenCount();

        for (int32_t i = 0; i < cnt; i++)
            m_childs.push_back(new HeapGraphEdgeProxy(m_snapshot, m_node->GetChild(i)));
    }

    retVal = m_childs;
    return result_t();
}

result_t HeapGraphEdgeProxy::get_type(int32_t& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = m_edge->GetType();
    return result_t();
}

result_t HeapGraphEdgeProxy::get_name(exlib::string& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    return GetArgumentValue(holder(), m_edge->GetName(), retVal);
}

result_t HeapGraphEdgeProxy::get_description(exlib::string& retVal)
{
    HeapGraphEdge::get_description(this, retVal);
    return result_t();
}

result_t HeapGraphEdgeProxy::getFromNode(obj_ptr<HeapGraphNode_base>& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = new HeapGraphNodeProxy(m_snapshot, m_edge->GetFromNode());
    return result_t();
}

result_t HeapGraphEdgeProxy::getToNode(obj_ptr<HeapGraphNode_base>& retVal)
{
    if (m_snapshot == 0)
        return result_t::invalid_arg();

    retVal = new HeapGraphNodeProxy(m_snapshot, m_edge->GetToNode());
    return result_t();
}
}
