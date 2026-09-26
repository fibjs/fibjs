/*
 * XmlNodeList.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlNodeList.h"
#include "ifs/XmlDocument.h"
#include "QuickArray.h"
#include "Iterator.h"

namespace fibjs {

class XmlNodeImpl;

class XmlNodeList : public XmlNodeList_base {
public:
    XmlNodeList(XmlNodeImpl* pThis)
        : m_this(pThis)
    {
    }

    ~XmlNodeList()
    {
        clean();
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);
    result_t toXmlString(exlib::string& retVal);

public:
    // XmlNodeList_base
    virtual result_t get_length(int32_t& retVal);
    virtual result_t item(int32_t index, obj_ptr<XmlNode_base>& retVal);
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlNode_base>& retVal);
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal);
    virtual result_t forEach(v8::Local<v8::Function> callback);
    virtual result_t keys(obj_ptr<Iterator_base>& retVal);
    virtual result_t values(obj_ptr<Iterator_base>& retVal);
    virtual result_t entries(obj_ptr<Iterator_base>& retVal);

public:
    void clean();
    void removeAll();
    // Detach every child of this list without recursing into their subtrees
    void detachChilds();

    result_t firstChild(obj_ptr<XmlNode_base>& retVal);
    result_t lastChild(obj_ptr<XmlNode_base>& retVal);
    result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal);
    result_t hasChildNodes(bool& retVal);

public:
    result_t get_children(obj_ptr<XmlNodeList_base>& retVal);
    result_t find_element(int32_t base, int32_t step, obj_ptr<XmlNode_base>& retVal);

    void appendChild(XmlNodeImpl* newChild)
    {
        m_childs.push_back(newChild);
    }

    // Append to a *result* list (m_this == NULL): the list owns a strong
    // reference to every node it holds, so a node that has been detached from
    // its document stays alive as long as the list is reachable.  This is what
    // makes it safe to hand out the raw m_node pointer in the traversal paths
    // (indexed getter / iterator): the receiver keeps the list alive for the
    // whole call, and the list keeps the nodes alive, so a GC that runs inside
    // wrap() (which allocates the JS wrapper) cannot free the node.
    void appendRef(XmlNodeImpl* newChild);

    bool hasChildNodes()
    {
        return !!m_childs.size();
    }

    result_t cloneChilds(XmlNode_base* to);
    result_t normalize();

private:
    XmlNodeImpl* checkChild(XmlNode_base* child);
    bool checkNew(XmlNodeImpl* child);
    // Collect the exclusively owned part of the subtree (reference count == 1),
    // in pre-order; see removeAll()
    void collectExclusive(std::vector<XmlNodeImpl*>& out);

public:
    XmlNodeImpl* m_this;
    std::vector<XmlNodeImpl*> m_childs;
    // true for result lists (m_this == NULL) built with appendRef(): every
    // entry holds a reference that removeAll()/clean() must release
    bool m_holdsRefs = false;

private:
    // Cached element-only view for get_children() (element.children): the
    // property is read in loops and used to rebuild a list on every access.
    // Invalidated by the owning document's query epoch, which any structural
    // change bumps (same mechanism as the document query indexes).
    obj_ptr<XmlNodeList> m_children;
    uint64_t m_childrenEpoch = 0;
};

} /* namespace fibjs */
