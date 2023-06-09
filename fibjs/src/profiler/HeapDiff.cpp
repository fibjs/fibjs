/*
 * HeapDiff.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "object.h"
#include "HeapSnapshot.h"
#include "ifs/HeapGraphEdge.h"
#include "ifs/profiler.h"
#include <set>
#include <map>

namespace fibjs {

typedef std::set<int32_t> idset;

inline void buildIDSet(idset* seen, HeapSnapshot_base* snapshot, intptr_t& s)
{
    obj_ptr<NArray> nodes;
    int32_t _count;

    snapshot->get_nodes(nodes);
    _count = nodes->length();

    for (int32_t i = 0; i < _count; i++) {
        Variant v;
        obj_ptr<HeapGraphNode_base> cur;

        nodes->_indexed_getter(i, v);
        cur = HeapGraphNode_base::getInstance(v.object());

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
        : type(profiler_base::C_Node_Hidden)
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
            isolate->NewString(it->first)).IsJust();
        d->Set(context, isolate->NewString("size_bytes"),
            v8::Integer::New(isolate->m_isolate, (int32_t)it->second.size)).IsJust();
        d->Set(context, isolate->NewString("size"),
            isolate->NewString(niceSize(it->second.size))).IsJust();
        d->Set(context, isolate->NewString("+"),
            v8::Integer::New(isolate->m_isolate, (int32_t)it->second.added)).IsJust();
        d->Set(context, isolate->NewString("-"),
            v8::Integer::New(isolate->m_isolate, (int32_t)it->second.released)).IsJust();
        a->Set(context, a->Length(), d).IsJust();
    }

    return a;
}

result_t HeapSnapshot::diff(HeapSnapshot_base* before, HeapSnapshot_base* after,
    v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    intptr_t s, diffBytes;
    obj_ptr<NArray> nodes;
    int32_t _count;
    date_t d;

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

    v8::Local<v8::Object> b = v8::Object::New(isolate->m_isolate);
    before->get_nodes(nodes);
    _count = nodes->length();
    b->Set(context, isolate->NewString("nodes"),
        v8::Integer::New(isolate->m_isolate, _count)).IsJust();

    before->get_time(d);
    b->Set(context, isolate->NewString("time"), d.value(isolate->m_isolate)).IsJust();
    o->Set(context, isolate->NewString("before"), b).IsJust();

    v8::Local<v8::Object> a = v8::Object::New(isolate->m_isolate);
    after->get_nodes(nodes);
    _count = nodes->length();
    a->Set(context, isolate->NewString("nodes"),
        v8::Integer::New(isolate->m_isolate, _count)).IsJust();
    after->get_time(d);
    a->Set(context, isolate->NewString("time"), d.value(isolate->m_isolate)).IsJust();
    o->Set(context, isolate->NewString("after"), a).IsJust();

    idset beforeIDs, afterIDs;

    s = 0;
    buildIDSet(&beforeIDs, before, s);

    b->Set(context, isolate->NewString("size_bytes"),
        v8::Integer::New(isolate->m_isolate, (int32_t)s)).IsJust();
    b->Set(context, isolate->NewString("size"),
        isolate->NewString(niceSize(s))).IsJust();

    diffBytes = s;
    s = 0;
    buildIDSet(&afterIDs, after, s);

    a->Set(context, isolate->NewString("size_bytes"),
        v8::Integer::New(isolate->m_isolate, (int32_t)s)).IsJust();
    a->Set(context, isolate->NewString("size"),
        isolate->NewString(niceSize(s))).IsJust();

    diffBytes = s - diffBytes;

    v8::Local<v8::Object> c = v8::Object::New(isolate->m_isolate);
    c->Set(context, isolate->NewString("size_bytes"),
        v8::Integer::New(isolate->m_isolate, (int32_t)diffBytes)).IsJust();
    c->Set(context, isolate->NewString("size"),
        isolate->NewString(niceSize(diffBytes))).IsJust();
    o->Set(context, isolate->NewString("change"), c).IsJust();

    std::vector<int32_t> changedIDs;
    setDiff(beforeIDs, afterIDs, changedIDs);
    c->Set(context, isolate->NewString("freed_nodes"),
        v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size())).IsJust();

    changeset changes;

    for (size_t i = 0; i < changedIDs.size(); i++) {
        obj_ptr<HeapGraphNode_base> n;

        before->getNodeById(changedIDs[i], n);
        manageChange(changes, n, false);
    }

    changedIDs.clear();

    setDiff(afterIDs, beforeIDs, changedIDs);

    c->Set(context, isolate->NewString("allocated_nodes"),
        v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size())).IsJust();

    for (size_t i = 0; i < changedIDs.size(); i++) {
        obj_ptr<HeapGraphNode_base> n;

        after->getNodeById(changedIDs[i], n);
        manageChange(changes, n, true);
    }

    c->Set(context, isolate->NewString("details"),
        changesetToObject(isolate, changes)).IsJust();

    retVal = o;

    return 0;
}

result_t HeapSnapshot::diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal)
{
    return diff(before, this, retVal);
}
}
