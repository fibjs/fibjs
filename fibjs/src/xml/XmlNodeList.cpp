/*
 * XmlNodeList.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "XmlNodeList.h"
#include "XmlNodeImpl.h"
#include "XmlElement.h"
#include "XmlDocument.h"
#include "ifs/XmlText.h"
#include <string.h>
#include "StringBuffer.h"
#include "Iterator.h"
#include "SimpleObject.h"

namespace fibjs {

result_t XmlNodeList::get_length(int32_t& retVal)
{
    retVal = (int32_t)m_childs.size();
    return 0;
}

result_t XmlNodeList::item(int32_t index, obj_ptr<XmlNode_base>& retVal)
{
    if (index < 0 || index >= (int32_t)m_childs.size()) {
        // clear the out parameter: callers may loop on it -- see T16
        retVal = NULL;
        return CALL_RETURN_NULL;
    }
    retVal = m_childs[index]->m_node;
    return 0;
}

result_t XmlNodeList::_indexed_getter(uint32_t index, obj_ptr<XmlNode_base>& retVal)
{
    if (index >= m_childs.size()) {
        retVal = NULL;
        return CALL_RETURN_NULL;
    }

    retVal = m_childs[index]->m_node;
    return 0;
}

result_t XmlNodeList::toString(exlib::string& retVal)
{
    StringBuffer strs;

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    if (sz == 0) {
        retVal.clear();
        return 0;
    } else if (sz == 1)
        return m_childs[0]->m_node->toString(retVal);

    for (i = 0; i < sz; i++) {
        exlib::string str;

        m_childs[i]->m_node->toString(str);
        strs.append(str);
    }

    retVal = strs.str();

    return 0;
}

result_t XmlNodeList::toXmlString(exlib::string& retVal)
{
    StringBuffer strs;

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    if (sz == 0) {
        retVal.clear();
        return 0;
    }

    for (i = 0; i < sz; i++) {
        exlib::string str;

        if (m_childs[i]->m_type == xml_base::C_ELEMENT_NODE) {
            ((XmlElement*)m_childs[i]->m_node)->toXmlString(str);
        } else {
            m_childs[i]->m_node->toString(str);
        }

        strs.append(str);
    }

    retVal = strs.str();

    return 0;
}

void XmlNodeList::appendRef(XmlNodeImpl* newChild)
{
    m_childs.push_back(newChild);
    newChild->m_node->Ref();
    m_holdsRefs = true;
}

void XmlNodeList::detachChilds()
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i++)
        m_childs[i]->clearParent();

    m_childs.resize(0);
    m_holdsRefs = false;
}

// Collect the nodes below this list that are *exclusively* owned by their
// parent (strong reference count == 1: no JS wrapper, no query result, no
// document reference).  Those are the nodes that will be deleted together with
// the parent, so their subtrees have to be flattened before the parent goes
// away.  Descending stops at shared nodes -- they survive and keep their
// subtree intact.  The result is in pre-order (parent before children).
void XmlNodeList::collectExclusive(std::vector<XmlNodeImpl*>& out)
{
    struct Frame {
        XmlNodeImpl* node;
        size_t index;
    };

    std::vector<Frame> stack;
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i++) {
        XmlNodeImpl* child = m_childs[i];
        if (child->m_node->refCount() != 1)
            continue;

        out.push_back(child);
        stack.push_back(Frame { child, 0 });

        while (!stack.empty()) {
            Frame& frame = stack.back();
            std::vector<XmlNodeImpl*>& childs = frame.node->m_childs->m_childs;

            if (frame.index >= childs.size()) {
                stack.pop_back();
                continue;
            }

            XmlNodeImpl* grandChild = childs[frame.index++];
            if (grandChild->m_node->refCount() != 1)
                continue;

            out.push_back(grandChild);
            stack.push_back(Frame { grandChild, 0 });
        }
    }
}

void XmlNodeList::removeAll()
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    if (sz > 0) {
        if (m_holdsRefs) {
            // result list: drop the references taken by appendRef()
            for (i = 0; i < sz; i++)
                m_childs[i]->m_node->Unref();
        } else if (m_this) {
            // Structural child list: detach every child.  A child that is only
            // owned by this parent gets deleted, and its destructor would then
            // detach (and delete) its own children recursively -- deep
            // documents (>=10k levels) overflow the C++ stack and kill the
            // process.  Flatten the exclusively owned part of the subtree first
            // (deepest nodes first), so by the time a node is deleted its
            // children have already been detached and the destructor chain
            // stays O(1) deep.
            std::vector<XmlNodeImpl*> exclusive;
            collectExclusive(exclusive);

            for (size_t k = exclusive.size(); k > 0; k--)
                exclusive[k - 1]->m_childs->detachChilds();

            detachChilds();
            return;
        }
    }

    m_childs.resize(0);
    m_holdsRefs = false;
}

void XmlNodeList::clean()
{
    removeAll();
    m_this = NULL;
}

result_t XmlNodeList::firstChild(obj_ptr<XmlNode_base>& retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    if (!sz)
        return CALL_RETURN_NULL;

    retVal = m_childs[0]->m_node;
    return 0;
}

result_t XmlNodeList::lastChild(obj_ptr<XmlNode_base>& retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    if (!sz)
        return CALL_RETURN_NULL;

    retVal = m_childs[sz - 1]->m_node;
    return 0;
}

XmlNodeImpl* XmlNodeList::checkChild(XmlNode_base* child)
{
    static char s_child_rules[12][12] = {
        //  E  A  T  CS ER EN PI C  D  DT DF
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // NONE
        { 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1 }, // ELEMENT_NODE
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, // ATTRIBUTE_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // TEXT_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // CDATA_SECTION_NODE
        { 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0 }, // ENTITY_REFERENCE_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // ENTITY_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // PROCESSING_INSTRUCTION_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // COMMENT_NODE
        { 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1 }, // DOCUMENT_NODE
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // DOCUMENT_TYPE_NODE
        { 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0 }  // DOCUMENT_FRAGMENT_NODE
    };

    XmlNodeImpl* node = XmlNodeImpl::fromNode(child);
    if (!node)
        return NULL;

    if (node->m_type >= 12 || m_this->m_type >= 12)
        return NULL;

    if (!s_child_rules[m_this->m_type][node->m_type])
        return NULL;

    return node;
}

bool XmlNodeList::checkNew(XmlNodeImpl* child)
{
    XmlNodeImpl* pThis = m_this;

    while (pThis) {
        if (pThis == child)
            return false;

        pThis = pThis->m_parent;
    }

    if (child->m_parent) {
        obj_ptr<XmlNode_base> tmp;
        child->m_parent->m_node->removeChild(child->m_node, tmp);
    }

    return true;
}

result_t XmlNodeList::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    if (!m_this)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    XmlNodeImpl* pNew = checkChild(newChild);
    XmlNodeImpl* pRef = checkChild(refChild);
    if (!pNew || !pRef)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (pRef->m_parent != m_this)
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The node before which the new node is to be inserted is not a child of this node."));

    // Handle DocumentFragment: insert all its children instead of itself
    if (pNew->m_type == xml_base::C_DOCUMENT_FRAGMENT_NODE) {
        XmlNodeList* fragChilds = pNew->m_childs;
        int32_t idx = pRef->m_index;

        while (fragChilds->m_childs.size() > 0) {
            XmlNodeImpl* child = fragChilds->m_childs[0];

            // Hold a reference to prevent premature destruction during removeChild
            obj_ptr<XmlNode_base> childRef = child->m_node;

            obj_ptr<XmlNode_base> tmp;
            fragChilds->removeChild(child->m_node, tmp);

            int32_t sz = (int32_t)m_childs.size();
            m_childs.resize(sz + 1);

            for (int32_t i = sz; i > idx; i--) {
                XmlNodeImpl* pTmp = m_childs[i - 1];
                m_childs[i] = pTmp;
                pTmp->m_index++;
            }

            child->setParent(m_this, idx);
            m_childs[idx] = child;
            idx++;
        }
        retVal = newChild;
        return 0;
    }

    if (pNew == pRef) {
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The new child element contains the parent."));

    int32_t sz = (int32_t)m_childs.size();
    int32_t idx = pRef->m_index;
    int32_t i;

    m_childs.resize(sz + 1);

    for (i = sz; i > idx; i--) {
        XmlNodeImpl* pTmp = m_childs[i - 1];
        m_childs[i] = pTmp;
        pTmp->m_index++;
    }

    pNew->setParent(m_this, idx);
    m_childs[idx] = pNew;

    retVal = newChild;
    return 0;
}

result_t XmlNodeList::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    if (!m_this)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    XmlNodeImpl* pNew = checkChild(newChild);
    XmlNodeImpl* pRef = checkChild(refChild);
    if (!pNew || !pRef)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (pRef->m_parent != m_this)
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The node after which the new node is to be inserted is not a child of this node."));

    // Handle DocumentFragment: insert all its children instead of itself
    if (pNew->m_type == xml_base::C_DOCUMENT_FRAGMENT_NODE) {
        XmlNodeList* fragChilds = pNew->m_childs;
        int32_t idx = pRef->m_index + 1;

        while (fragChilds->m_childs.size() > 0) {
            XmlNodeImpl* child = fragChilds->m_childs[0];

            // Hold a reference to prevent premature destruction during removeChild
            obj_ptr<XmlNode_base> childRef = child->m_node;

            obj_ptr<XmlNode_base> tmp;
            fragChilds->removeChild(child->m_node, tmp);

            int32_t sz = (int32_t)m_childs.size();
            m_childs.resize(sz + 1);

            for (int32_t i = sz; i > idx; i--) {
                XmlNodeImpl* pTmp = m_childs[i - 1];
                m_childs[i] = pTmp;
                pTmp->m_index++;
            }

            child->setParent(m_this, idx);
            m_childs[idx] = child;
            idx++;
        }
        retVal = newChild;
        return 0;
    }

    if (pNew == pRef) {
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The new child element contains the parent."));

    int32_t sz = (int32_t)m_childs.size();
    int32_t idx = pRef->m_index + 1;
    int32_t i;

    m_childs.resize(sz + 1);

    for (i = sz; i > idx; i--) {
        XmlNodeImpl* pTmp = m_childs[i - 1];
        m_childs[i] = pTmp;
        pTmp->m_index++;
    }

    pNew->setParent(m_this, idx);
    m_childs[idx] = pNew;

    retVal = newChild;
    return 0;
}

result_t XmlNodeList::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    if (!m_this)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    XmlNodeImpl* pNew = checkChild(newChild);
    XmlNodeImpl* pOld = checkChild(oldChild);
    if (!pNew || !pOld)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (pOld->m_parent != m_this)
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The node to be replaced is not a child of this node."));

    // Handle DocumentFragment: replace with all its children
    if (pNew->m_type == xml_base::C_DOCUMENT_FRAGMENT_NODE) {
        XmlNodeList* fragChilds = pNew->m_childs;
        int32_t idx = pOld->m_index;

        // Remove the old child first
        int32_t sz = (int32_t)m_childs.size();
        for (int32_t i = idx; i < sz - 1; i++) {
            XmlNodeImpl* pTmp = m_childs[i + 1];
            m_childs[i] = pTmp;
            pTmp->m_index--;
        }
        m_childs.resize(sz - 1);
        pOld->clearParent();

        // Insert all fragment children at the position
        while (fragChilds->m_childs.size() > 0) {
            XmlNodeImpl* child = fragChilds->m_childs[0];

            // Hold a reference to prevent premature destruction during removeChild
            obj_ptr<XmlNode_base> childRef = child->m_node;

            obj_ptr<XmlNode_base> tmp;
            fragChilds->removeChild(child->m_node, tmp);

            sz = (int32_t)m_childs.size();
            m_childs.resize(sz + 1);

            for (int32_t i = sz; i > idx; i--) {
                XmlNodeImpl* pTmp = m_childs[i - 1];
                m_childs[i] = pTmp;
                pTmp->m_index++;
            }

            child->setParent(m_this, idx);
            m_childs[idx] = child;
            idx++;
        }
        retVal = oldChild;
        return 0;
    }

    if (pNew == pOld) {
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The new child element contains the parent."));

    int32_t idx = pOld->m_index;

    pOld->clearParent();

    pNew->setParent(m_this, idx);
    m_childs[idx] = pNew;

    retVal = oldChild;
    return 0;
}

result_t XmlNodeList::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    if (!m_this)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    XmlNodeImpl* pOld = checkChild(oldChild);
    if (!pOld)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (pOld->m_parent != m_this)
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The node to be removed is not a child of this node."));

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = pOld->m_index; i < sz - 1; i++) {
        XmlNodeImpl* pTmp = m_childs[i + 1];
        m_childs[i] = pTmp;
        pTmp->m_index--;
    }
    m_childs.resize(sz - 1);

    pOld->clearParent();

    retVal = oldChild;
    return 0;
}

result_t XmlNodeList::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    if (!m_this)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    XmlNodeImpl* pNew = checkChild(newChild);
    if (!pNew)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    // Handle DocumentFragment: append all its children instead of itself
    if (pNew->m_type == xml_base::C_DOCUMENT_FRAGMENT_NODE) {
        XmlNodeList* fragChilds = pNew->m_childs;
        while (fragChilds->m_childs.size() > 0) {
            XmlNodeImpl* child = fragChilds->m_childs[0];

            // Hold a reference to prevent premature destruction during removeChild
            obj_ptr<XmlNode_base> childRef = child->m_node;

            obj_ptr<XmlNode_base> tmp;
            fragChilds->removeChild(child->m_node, tmp);

            child->setParent(m_this, (int32_t)m_childs.size());
            m_childs.push_back(child);
        }
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return CHECK_ERROR(Runtime::setError("XmlNodeList: The new child element contains the parent."));

    pNew->setParent(m_this, (int32_t)m_childs.size());
    m_childs.push_back(pNew);

    retVal = newChild;
    return 0;
}

result_t XmlNodeList::hasChildNodes(bool& retVal)
{
    retVal = !!m_childs.size();
    return 0;
}

result_t XmlNodeList::get_children(obj_ptr<XmlNodeList_base>& retVal)
{
    // children is a hot property (read in loops, and every read used to rebuild
    // the list): cache the element-only view and invalidate it through the
    // document query epoch, which every structural change bumps.
    XmlDocument* doc = m_this ? m_this->document() : NULL;

    if (doc && m_children && m_childrenEpoch == doc->m_queryEpoch) {
        retVal = m_children;
        return 0;
    }

    obj_ptr<XmlNodeList> children = new XmlNodeList(NULL);

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i++) {
        XmlNodeImpl* child = m_childs[i];
        if (child->m_type == xml_base::C_ELEMENT_NODE)
            children->appendRef(child);
    }

    retVal = children;

    if (doc) {
        m_children = children;
        m_childrenEpoch = doc->m_queryEpoch;
    }

    return 0;
}

result_t XmlNodeList::find_element(int32_t base, int32_t step, obj_ptr<XmlNode_base>& retVal)
{
    for (base += step; base >= 0 && base < (int32_t)m_childs.size(); base += step) {
        XmlNodeImpl* child = m_childs[base];
        if (child->m_type == xml_base::C_ELEMENT_NODE) {
            retVal = child->m_node;
            return 0;
        }
    }

    retVal = NULL;
    return CALL_RETURN_NULL;
}

// Deep-clone every child of this list into `to`, without recursion.
//
// The previous form called child->cloneNode(true) for each entry, and
// cloneNode() called back into this function for the children of that clone:
// one pair of C++ frames per tree level, which overflowed the native stack
// (~4000 levels) and killed the process instead of raising a JS error.  Here a
// child is cloned shallow (cloneNode(false), which still copies attributes and
// other per-node state) and appended, and its own children are processed by
// this loop -- same tree, same document order, O(depth) stack frames.
result_t XmlNodeList::cloneChilds(XmlNode_base* to)
{
    struct Frame {
        std::vector<XmlNodeImpl*>* childs;
        XmlNode_base* dst;
        size_t index;
    };

    std::vector<Frame> stack;
    stack.push_back(Frame { &m_childs, to, 0 });

    while (!stack.empty()) {
        Frame& frame = stack.back();

        if (frame.index >= frame.childs->size()) {
            stack.pop_back();
            continue;
        }

        XmlNodeImpl* child = (*frame.childs)[frame.index++];
        obj_ptr<XmlNode_base> clone;
        obj_ptr<XmlNode_base> out;
        result_t hr;

        hr = child->m_node->cloneNode(false, clone);
        if (hr < 0)
            return hr;

        hr = frame.dst->appendChild(clone, out);
        if (hr < 0)
            return hr;

        if (child->m_childs->hasChildNodes())
            stack.push_back(Frame { &child->m_childs->m_childs, clone, 0 });
    }

    return 0;
}

// Synchronous iterator over an XmlNodeList.
//
// The generic Iterator drives an AsyncCall round trip on every step: next()
// returns CALL_E_NOSYNC in the sync state, AsyncCall::check_result() then
// leaves the JS scope, calls invoke() (a second virtual dispatch into next())
// and waits on an Event -- even though the value is produced inline.  An
// XmlNodeList is an in-memory snapshot, so next() can produce the value
// directly and return 0: AsyncCall::check_result() takes the `else` branch and
// never touches the event, and the async/promise variant resolves from
// AsyncCallBack::check_result() the same way.  That removes the LeaveJsScope,
// the Event set/wait, one virtual dispatch and the std::function indirection
// from every iteration step.
//
// GC safety (must not be broken): this iterator keeps the list alive (m_list),
// and a result list owns a strong reference to every node it holds
// (XmlNodeList::appendRef), so the node read below cannot be freed by a GC
// running inside wrap() -- the wrapper allocation is the only GC point in this
// path, and at that point the node is already referenced by the list.  The raw
// read and the Variant assignment that takes the strong reference are adjacent
// statements with no GC point in between; never reorder them or drop the
// list-held reference model without redoing that analysis.
class XmlNodeListIterator : public Iterator_base {
public:
    enum Kind {
        kValues,
        kKeys,
        kEntries
    };

public:
    XmlNodeListIterator(XmlNodeList* list, Kind kind)
        : m_list(list)
        , m_kind(kind)
        , m_index(0)
        , m_done(false)
    {
    }

public:
    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t next(obj_ptr<NextType>& retVal, AsyncHandle ac)
    {
        retVal = new NextType();

        if (m_done || m_index >= m_list->m_childs.size()) {
            m_done = true;
            retVal->done = true;
            return 0;
        }

        size_t index = m_index++;
        retVal->done = false;

        switch (m_kind) {
        case kKeys:
            retVal->value = (int32_t)index;
            break;
        case kEntries: {
            obj_ptr<NArray> array = new NArray();
            array->append((int32_t)index);
            array->append(m_list->m_childs[index]->m_node);
            retVal->value = array;
            break;
        }
        default:
            retVal->value = m_list->m_childs[index]->m_node;
            break;
        }

        return 0;
    }

    virtual result_t _return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
    {
        m_done = true;
        retVal = new ReturnType();
        retVal->done = true;
        return 0;
    }

private:
    obj_ptr<XmlNodeList> m_list;
    Kind m_kind;
    size_t m_index;
    bool m_done;
};

result_t XmlNodeList::symbol_iterator(obj_ptr<Iterator_base>& retVal)
{
    return values(retVal);
}

result_t XmlNodeList::keys(obj_ptr<Iterator_base>& retVal)
{
    retVal = new XmlNodeListIterator(this, XmlNodeListIterator::kKeys);
    return 0;
}

result_t XmlNodeList::values(obj_ptr<Iterator_base>& retVal)
{
    retVal = new XmlNodeListIterator(this, XmlNodeListIterator::kValues);
    return 0;
}

result_t XmlNodeList::entries(obj_ptr<Iterator_base>& retVal)
{
    retVal = new XmlNodeListIterator(this, XmlNodeListIterator::kEntries);
    return 0;
}

// Merge adjacent text nodes and drop the empty ones in one child list.
static void normalizeChilds(XmlNodeList* list)
{
    size_t i = 0;

    while (i < list->m_childs.size()) {
        XmlNodeImpl* child = list->m_childs[i];

        if (child->m_type != xml_base::C_TEXT_NODE) {
            i++;
            continue;
        }

        XmlText_base* txt = (XmlText_base*)child->m_node;

        while (i + 1 < list->m_childs.size()) {
            XmlNodeImpl* next = list->m_childs[i + 1];

            if (next->m_type != xml_base::C_TEXT_NODE)
                break;

            exlib::string val;
            next->m_node->get_nodeValue(val);
            txt->appendData(val);

            obj_ptr<XmlNode_base> out;
            list->removeChild(next->m_node, out);
        }

        exlib::string val;
        child->m_node->get_nodeValue(val);
        if (val.empty()) {
            obj_ptr<XmlNode_base> out;
            list->removeChild(child->m_node, out);
            // the next sibling is at the same index now
            continue;
        }

        i++;
    }
}

result_t XmlNodeList::normalize()
{
    if (!m_this) {
        // A result list has no parent to merge into -- removeChild() rejects it
        // -- so only the subtrees of its entries are normalized.
        for (size_t i = 0; i < m_childs.size(); i++)
            walkTree(m_childs[i], false, WALK_ALL_NODES, [](XmlNodeImpl* node) -> int32_t {
                normalizeChilds(node->m_childs);
                return WALK_CONTINUE;
            });

        return 0;
    }

    // Normalize every child list of the subtree, the root's included.  The walk
    // is iterative (XmlTreeWalk.h): the recursive form -- child->normalize() per
    // level -- overflowed the native stack on deep documents.  A callback runs
    // before the walker starts iterating that node's children, so removing text
    // nodes from the list it just normalized cannot invalidate a cursor.
    walkTree(m_this, true, WALK_ALL_NODES, [](XmlNodeImpl* node) -> int32_t {
        normalizeChilds(node->m_childs);
        return WALK_CONTINUE;
    });

    return 0;
}

result_t XmlNodeList::forEach(v8::Local<v8::Function> callback)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    int32_t sz = (int32_t)m_childs.size();

    for (int32_t i = 0; i < sz; i++) {
        v8::Local<v8::Value> args[3];
        args[0] = m_childs[i]->m_node->wrap();
        args[1] = v8::Int32::New(isolate->m_isolate, i);
        args[2] = wrap();

        v8::Local<v8::Value> result = callback->Call(context, v8::Undefined(isolate->m_isolate), 3, args).FromMaybe(v8::Local<v8::Value>());
        if (result.IsEmpty())
            return CALL_E_JAVASCRIPT;
    }

    return 0;
}
}
