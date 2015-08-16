/*
 * HeapDiff.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "HeapSnapshot.h"
#include "ifs/HeapGraphEdge.h"
#include "ifs/profiler.h"
#include <set>
#include <map>

namespace fibjs
{

typedef std::set<int32_t> idset;

inline void buildIDSet(idset* seen, HeapSnapshot_base* snapshot, intptr_t& s)
{
	obj_ptr<List_base> nodes;
	int32_t _count;

	snapshot->get_nodes(nodes);
	nodes->get_length(_count);

	for (int32_t i = 0; i < _count; i++)
	{
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

void setDiff(idset a, idset b, std::vector<int32_t> &c)
{
	for (idset::iterator i = a.begin(); i != a.end(); i++)
		if (b.find(*i) == b.end())
			c.push_back(*i);
}

class example
{
public:
	int32_t type;
	std::string name;
	std::string value;
	std::string heap_value;
	intptr_t self_size;
	intptr_t retained_size;
	intptr_t retainers;

	example() : type(profiler_base::_Node_Hidden),
		self_size(0), retained_size(0), retainers(0)
	{};
};

class change
{
public:
	intptr_t size;
	intptr_t added;
	intptr_t released;
	std::vector<example> examples;

	change() : size(0), added(0), released(0) { }
};

typedef std::map<std::string, change>changeset;

inline void manageChange(changeset& changes, HeapGraphNode_base* node,
                         bool added)
{
	std::string type;
	int32_t _type;

	node->get_type(_type);
	switch (_type) {
	case profiler_base::_Node_Array:
		type.assign("Array");
		break;
	case profiler_base::_Node_String:
		type.assign("String");
		break;
	case profiler_base::_Node_Object:
		node->get_name(type);
		break;
	case profiler_base::_Node_Code:
		type.assign("Code");
		break;
	case profiler_base::_Node_Closure:
	{
		node->get_name(type);
		if (!type.empty())
			type.append("(Closure)");
		else
			type.assign("Closure");
		break;
	}
	case profiler_base::_Node_RegExp:
		type.assign("RegExp");
		break;
	case profiler_base::_Node_HeapNumber:
		type.assign("Number");
		break;
	case profiler_base::_Node_Native:
		type.assign("Native");
		break;
	case profiler_base::_Node_Synthetic :
		type.assign("Synthetic");
		break;
	case profiler_base::_Node_ConsString :
		type.assign("ConsString");
		break;
	case profiler_base::_Node_SlicedString :
		type.assign("SlicedString");
		break;
	default :
		type.assign("Hidden");
	}

	if (changes.find(type) == changes.end())
		changes[type] = change();

	changeset::iterator i = changes.find(type);

	int32_t _size;
	node->get_shallowSize(_size);

	i->second.size += _size * (added ? 1 : -1);
	if (added) i->second.added++;
	else i->second.released++;

	return;
}

static int32_t compare (const void * a, const void * b)
{
	return (int32_t)((*(changeset::iterator*)b)->second.size -
	                 (*(changeset::iterator*)a)->second.size);
}

inline v8::Local<v8::Value> changesetToObject(Isolate* isolate, changeset& changes)
{
	v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate);
	std::vector<changeset::iterator> its;
	int32_t n = 0;

	its.resize(changes.size());
	for (changeset::iterator i = changes.begin(); i != changes.end(); i++)
		its[n ++] = i;

	qsort(&its[0], its.size(), sizeof(changeset::iterator), compare);

	for (int32_t i = 0; i < n; i++) {
		changeset::iterator it = its[i];
		v8::Local<v8::Object> d = v8::Object::New(isolate->m_isolate);
		d->Set(v8::String::NewFromUtf8(isolate->m_isolate, "type"),
		       v8::String::NewFromUtf8(isolate->m_isolate, it->first.c_str()));
		d->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size_bytes"),
		       v8::Integer::New(isolate->m_isolate, (int32_t)it->second.size));
		d->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size"),
		       v8::String::NewFromUtf8(isolate->m_isolate, niceSize(it->second.size).c_str()));
		d->Set(v8::String::NewFromUtf8(isolate->m_isolate, "+"),
		       v8::Integer::New(isolate->m_isolate, (int32_t)it->second.added));
		d->Set(v8::String::NewFromUtf8(isolate->m_isolate, "-"),
		       v8::Integer::New(isolate->m_isolate, (int32_t)it->second.released));
		a->Set(a->Length(), d);
	}

	return a;
}

result_t HeapSnapshot::diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal)
{
	obj_ptr<HeapSnapshot> old_snap = (HeapSnapshot*)before;

	Isolate* isolate = Isolate::now();
	intptr_t s, diffBytes;
	obj_ptr<List_base> nodes;
	int32_t _count;

	v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

	v8::Local<v8::Object> b = v8::Object::New(isolate->m_isolate);
	old_snap->get_nodes(nodes);
	nodes->get_length(_count);
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "nodes"),
	       v8::Integer::New(isolate->m_isolate, _count));
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "time"), old_snap->m_time);
	o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "before"), b);

	v8::Local<v8::Object> a = v8::Object::New(isolate->m_isolate);
	get_nodes(nodes);
	nodes->get_length(_count);
	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "nodes"),
	       v8::Integer::New(isolate->m_isolate, _count));
	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "time"), m_time);
	o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "after"), a);

	idset beforeIDs, afterIDs;

	s = 0;
	buildIDSet(&beforeIDs, old_snap, s);

	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size_bytes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)s));
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size"),
	       v8::String::NewFromUtf8(isolate->m_isolate, niceSize(s).c_str()));

	diffBytes = s;
	s = 0;
	buildIDSet(&afterIDs, this, s);

	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size_bytes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)s));
	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size"),
	       v8::String::NewFromUtf8(isolate->m_isolate, niceSize(s).c_str()));

	diffBytes = s - diffBytes;

	v8::Local<v8::Object> c = v8::Object::New(isolate->m_isolate);
	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size_bytes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)diffBytes));
	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size"),
	       v8::String::NewFromUtf8(isolate->m_isolate, niceSize(diffBytes).c_str()));
	o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "change"), c);

	std::vector<int32_t> changedIDs;
	setDiff(beforeIDs, afterIDs, changedIDs);
	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "freed_nodes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size()));

	changeset changes;

	for (size_t i = 0; i < changedIDs.size(); i++) {
		obj_ptr<HeapGraphNode_base> n;

		old_snap->getNodeById(changedIDs[i], n);
		manageChange(changes, n, false);
	}

	changedIDs.clear();

	setDiff(afterIDs, beforeIDs, changedIDs);

	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "allocated_nodes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size()));

	for (size_t i = 0; i < changedIDs.size(); i++) {
		obj_ptr<HeapGraphNode_base> n;

		getNodeById(changedIDs[i], n);
		manageChange(changes, n, true);
	}

	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "details"),
	       changesetToObject(isolate, changes));

	retVal = o;

	return 0;
}

}
