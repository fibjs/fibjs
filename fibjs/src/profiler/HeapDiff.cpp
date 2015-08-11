/*
 * HeapDiff.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: lion
 */

#include "HeapSnapshot.h"
#include <set>

namespace fibjs
{

inline std::string handleToStr(const v8::Local<v8::Value>& str)
{
	v8::String::Utf8Value utfString(str->ToString());
	return *utfString;
}

typedef std::set<v8::SnapshotObjectId> idset;

inline void buildIDSet(idset* seen,
                       const v8::HeapGraphNode* cur, intptr_t& s)
{
	if (seen->find(cur->GetId()) != seen->end())
		return;

	if (cur->GetType() == v8::HeapGraphNode::kObject &&
	        handleToStr(cur->GetName()).compare("HeapSnapshot") == 0)
		return;

	s += cur->GetShallowSize();

	seen->insert(cur->GetId());

	for (int i = 0; i < cur->GetChildrenCount(); i++)
		buildIDSet(seen, cur->GetChild(i)->GetToNode(), s);
}

void setDiff(idset a, idset b, std::vector<v8::SnapshotObjectId> &c)
{
	for (idset::iterator i = a.begin(); i != a.end(); i++)
		if (b.find(*i) == b.end()) c.push_back(*i);
}

class example
{
public:
	v8::HeapGraphEdge::Type context;
	v8::HeapGraphNode::Type type;
	std::string name;
	std::string value;
	std::string heap_value;
	intptr_t self_size;
	intptr_t retained_size;
	intptr_t retainers;

	example() : context(v8::HeapGraphEdge::kHidden),
		type(v8::HeapGraphNode::kHidden),
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

inline void manageChange(changeset& changes, const v8::HeapGraphNode * node,
                         bool added)
{
	std::string type;

	switch (node->GetType()) {
	case v8::HeapGraphNode::kArray:
		type.assign("Array");
		break;
	case v8::HeapGraphNode::kString:
		type.assign("String");
		break;
	case v8::HeapGraphNode::kObject:
		type.assign(handleToStr(node->GetName()));
		break;
	case v8::HeapGraphNode::kCode:
		type.assign("Code");
		break;
	case v8::HeapGraphNode::kClosure:
	{
		type = handleToStr(node->GetName());
		if (!type.empty())
			type.append("(Closure)");
		else
			type.assign("Closure");
		break;
	}
	case v8::HeapGraphNode::kRegExp:
		type.assign("RegExp");
		break;
	case v8::HeapGraphNode::kHeapNumber:
		type.assign("Number");
		break;
	case v8::HeapGraphNode::kNative:
		type.assign("Native");
		break;
	case v8::HeapGraphNode::kSynthetic :
		type.assign("Synthetic");
		break;
	case v8::HeapGraphNode::kConsString :
		type.assign("ConsString");
		break;
	case v8::HeapGraphNode::kSlicedString :
		type.assign("SlicedString");
		break;
	default :
		type.assign("Hidden");
	}

	if (changes.find(type) == changes.end())
		changes[type] = change();

	changeset::iterator i = changes.find(type);

	i->second.size += node->GetShallowSize() * (added ? 1 : -1);
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

	v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

	v8::Local<v8::Object> b = v8::Object::New(isolate->m_isolate);
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "nodes"),
	       v8::Integer::New(isolate->m_isolate, old_snap->m_snapshot->GetNodesCount()));
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "time"), old_snap->m_time);
	o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "before"), b);

	v8::Local<v8::Object> a = v8::Object::New(isolate->m_isolate);
	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "nodes"),
	       v8::Integer::New(isolate->m_isolate, m_snapshot->GetNodesCount()));
	a->Set(v8::String::NewFromUtf8(isolate->m_isolate, "time"), m_time);
	o->Set(v8::String::NewFromUtf8(isolate->m_isolate, "after"), a);

	idset beforeIDs, afterIDs;
	s = 0;
	buildIDSet(&beforeIDs, old_snap->m_snapshot->GetRoot(), s);
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size_bytes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)s));
	b->Set(v8::String::NewFromUtf8(isolate->m_isolate, "size"),
	       v8::String::NewFromUtf8(isolate->m_isolate, niceSize(s).c_str()));

	diffBytes = s;
	s = 0;
	buildIDSet(&afterIDs, m_snapshot->GetRoot(), s);
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

	std::vector<v8::SnapshotObjectId> changedIDs;
	setDiff(beforeIDs, afterIDs, changedIDs);
	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "freed_nodes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size()));

	changeset changes;

	for (size_t i = 0; i < changedIDs.size(); i++) {
		const v8::HeapGraphNode * n = old_snap->m_snapshot->GetNodeById(changedIDs[i]);
		manageChange(changes, n, false);
	}

	changedIDs.clear();

	setDiff(afterIDs, beforeIDs, changedIDs);

	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "allocated_nodes"),
	       v8::Integer::New(isolate->m_isolate, (int32_t)changedIDs.size()));

	for (size_t i = 0; i < changedIDs.size(); i++) {
		const v8::HeapGraphNode * n = m_snapshot->GetNodeById(changedIDs[i]);
		manageChange(changes, n, true);
	}

	c->Set(v8::String::NewFromUtf8(isolate->m_isolate, "details"),
	       changesetToObject(isolate, changes));

	retVal = o;

	return 0;
}

}
