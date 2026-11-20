/*
 * HeapDiff.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "object.h"
#include "HeapSnapshot.h"
#include "ifs/HeapGraphEdge.h"
#include "ifs/v8.h"
#include <set>
#include <unordered_map>

namespace fibjs {

typedef std::set<int32_t> idset;

inline void buildIDSet(idset* seen, HeapSnapshot_base* snapshot, intptr_t& s)
{
    std::vector<obj_ptr<HeapGraphNode_base>> nodes;

    snapshot->get_nodes(nodes);

    for (auto& cur : nodes) {
        int32_t _id;
        int32_t _size;

        cur->get_id(_id);
        cur->get_shallowSize(_size);

        s += _size;
        seen->insert(_id);
    }
}

void setDiff(idset a, idset b, std::vector<int32_t>& c)
{
    for (idset::iterator i = a.begin(); i != a.end(); i++)
        if (b.find(*i) == b.end())
            c.push_back(*i);
}

class example {
public:
    int32_t type;
    exlib::string name;
    exlib::string value;
    exlib::string heap_value;
    intptr_t self_size;
    intptr_t retained_size;
    intptr_t retainers;

    example()
        : type(v8_base::C_Node_Hidden)
        , self_size(0)
        , retained_size(0)
        , retainers(0) {};
};

class change {
public:
    intptr_t size;
    intptr_t added;
    intptr_t released;
    std::vector<example> examples;

    change()
        : size(0)
        , added(0)
        , released(0)
    {
    }
};

typedef std::unordered_map<exlib::string, change> changeset;

inline void manageChange(changeset& changes, HeapGraphNode_base* node,
    bool added)
{
    exlib::string type;

    node->get_description(type);

    if (changes.find(type) == changes.end())
        changes[type] = change();

    changeset::iterator i = changes.find(type);

    int32_t _size;
    node->get_shallowSize(_size);

    i->second.size += _size * (added ? 1 : -1);
    if (added)
        i->second.added++;
    else
        i->second.released++;

    return;
}

static int32_t compare(const void* a, const void* b)
{
    return (int32_t)((*(changeset::iterator*)b)->second.size - (*(changeset::iterator*)a)->second.size);
}

inline v8::Local<v8::Value> changesetToObject(Isolate* isolate, changeset& changes)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate);
    std::vector<changeset::iterator> its;
    int32_t n = 0;

    its.resize(changes.size());
    for (changeset::iterator i = changes.begin(); i != changes.end(); i++)
        its[n++] = i;

    qsort(its.data(), its.size(), sizeof(changeset::iterator), compare);

    for (int32_t i = 0; i < n; i++) {
        changeset::iterator it = its[i];
        v8::Local<v8::Object> d = v8::Object::New(isolate->m_isolate);
        d->Set(context, isolate->NewString("type"),
             isolate->NewString(it->first))
            .IsJust();
        d->Set(context, isolate->NewString("size_bytes"),
             v8::Integer::New(isolate->m_isolate, (int32_t)it->second.size))
            .IsJust();
        d->Set(context, isolate->NewString("size"),
             isolate->NewString(niceSize(it->second.size)))
            .IsJust();
        d->Set(context, isolate->NewString("+"),
             v8::Integer::New(isolate->m_isolate, (int32_t)it->second.added))
            .IsJust();
        d->Set(context, isolate->NewString("-"),
             v8::Integer::New(isolate->m_isolate, (int32_t)it->second.released))
            .IsJust();
        a->Set(context, a->Length(), d).IsJust();
    }

    return a;
}

result_t HeapSnapshot::diff(HeapSnapshot_base* before, HeapSnapshot_base* after,
    obj_ptr<HeapSnapshot_base::DiffType>& retVal)
{
    Isolate* isolate = Isolate::current();
    intptr_t s, diffBytes;
    std::vector<obj_ptr<HeapGraphNode_base>> nodes;
    int32_t _count;
    date_t d;

    retVal = new HeapSnapshot_base::DiffType();
    retVal->before = new HeapSnapshot_base::DiffType_before();
    retVal->after = new HeapSnapshot_base::DiffType_after();
    retVal->change = new HeapSnapshot_base::DiffType_change();

    before->get_nodes(nodes);
    _count = (int32_t)nodes.size();
    retVal->before->nodes = _count;
    before->get_time(d);
    retVal->before->time = d;

    after->get_nodes(nodes);
    _count = (int32_t)nodes.size();
    retVal->after->nodes = _count;
    after->get_time(d);
    retVal->after->time = d;

    idset beforeIDs, afterIDs;

    s = 0;
    buildIDSet(&beforeIDs, before, s);

    retVal->before->size_bytes = (int32_t)s;
    retVal->before->size = niceSize(s);

    diffBytes = s;
    s = 0;
    buildIDSet(&afterIDs, after, s);

    retVal->after->size_bytes = (int32_t)s;
    retVal->after->size = niceSize(s);

    diffBytes = s - diffBytes;
    retVal->change->size_bytes = (int32_t)diffBytes;
    retVal->change->size = niceSize(diffBytes);

    std::vector<int32_t> changedIDs;
    setDiff(beforeIDs, afterIDs, changedIDs);
    retVal->change->freed_nodes = (int32_t)changedIDs.size();

    changeset changes;

    for (size_t i = 0; i < changedIDs.size(); i++) {
        obj_ptr<HeapGraphNode_base> n;

        before->getNodeById(changedIDs[i], n);
        manageChange(changes, n, false);
    }

    changedIDs.clear();

    setDiff(afterIDs, beforeIDs, changedIDs);
    retVal->change->allocated_nodes = (int32_t)changedIDs.size();

    for (size_t i = 0; i < changedIDs.size(); i++) {
        obj_ptr<HeapGraphNode_base> n;

        after->getNodeById(changedIDs[i], n);
        manageChange(changes, n, true);
    }

    retVal->change->details = changesetToObject(isolate, changes);

    return result_t();
}

result_t HeapSnapshot::diff(HeapSnapshot_base* before, obj_ptr<DiffType>& retVal)
{
    return diff(before, this, retVal);
}
}
