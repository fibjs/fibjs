/*
 * XmlDocument.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlAttr.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"
#include "XmlDocumentFragment.h"
#include "XmlParser.h"
#include "encoding_conv.h"
#include <cmath>

namespace fibjs {

DECLARE_MODULE(xml);

result_t XmlDocument_base::_new(exlib::string type, obj_ptr<XmlDocument_base>& retVal,
    v8::Local<v8::Object> This)
{
    bool isXml = type == "text/xml";

    if (!isXml && (type != "text/html"))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "Invalid MIME type: '%s', expected 'text/xml' or 'text/html'.", type.c_str()));

    obj_ptr<XmlDocument> doc = new XmlDocument(isXml);
    retVal = doc;

    if (!isXml)
        doc->create_root();

    return 0;
}

result_t getParseLimits(v8::Local<v8::Object> options, XmlParseLimits& limits)
{
    limits = XmlParseLimits();

    if (options.IsEmpty() || !options->IsObject())
        return 0;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // One numeric limit: a missing/null property keeps its default, a
    // non-number is rejected and a non-finite value (Infinity) disables the
    // limit, as 0 and negative values do.
    auto read = [&](const char* name, double& out) -> result_t {
        v8::Local<v8::Value> v;

        if (!options->Get(context, isolate->NewString(name)).ToLocal(&v) || v->IsUndefined() || v->IsNull())
            return 0;

        if (!v->IsNumber())
            return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "xml: option '%s' must be a number.", name));

        double d = v->NumberValue(context).FromMaybe(0);

        out = std::isfinite(d) ? d : 0;

        return 0;
    };

    double depth = limits.max_element_depth;
    double nodes = (double)limits.max_node_count;
    result_t hr;

    hr = read("maxElementDepth", depth);
    if (hr < 0)
        return hr;

    hr = read("maxNodeCount", nodes);
    if (hr < 0)
        return hr;

    limits.max_element_depth = (int32_t)depth;
    limits.max_node_count = (int64_t)nodes;

    return 0;
}

result_t xml_base::parse(exlib::string source, exlib::string type, v8::Local<v8::Object> options,
    obj_ptr<XmlDocument_base>& retVal)
{
    bool isXml = type == "text/xml";

    if (!isXml && (type != "text/html"))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "Invalid MIME type: '%s', expected 'text/xml' or 'text/html'.", type.c_str()));

    XmlParseLimits limits;
    result_t hr = getParseLimits(options, limits);
    if (hr < 0)
        return hr;

    // XmlDocument is the only XmlDocument_base implementation in this module
    // (same assumption as XmlNodeImpl::document()).
    obj_ptr<XmlDocument> doc = new XmlDocument(isXml);
    retVal = doc;

    return doc->load(source, limits);
}

result_t xml_base::parse(Buffer_base* source, exlib::string type, v8::Local<v8::Object> options,
    obj_ptr<XmlDocument_base>& retVal)
{
    bool isXml = type == "text/xml";

    if (!isXml && (type != "text/html"))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "Invalid MIME type: '%s', expected 'text/xml' or 'text/html'.", type.c_str()));

    XmlParseLimits limits;
    result_t hr = getParseLimits(options, limits);
    if (hr < 0)
        return hr;

    obj_ptr<XmlDocument> doc = new XmlDocument(isXml);
    retVal = doc;

    return doc->load(source, limits);
}

result_t xml_base::serialize(XmlNode_base* node, exlib::string& retVal)
{
    return node->toString(retVal);
}

void XmlDocument::create_root()
{
    obj_ptr<XmlNode_base> tmp;

    // Create HTML root element
    m_element = new XmlElement(this, "html", false);
    appendChild(m_element, tmp);

    // Create HEAD element
    obj_ptr<XmlElement> head = new XmlElement(this, "head", false);
    m_element->appendChild(head, tmp);

    // Create BODY element
    obj_ptr<XmlElement> body = new XmlElement(this, "body", false);
    m_element->appendChild(body, tmp);
}

result_t XmlDocument::get_nodeName(exlib::string& retVal)
{
    retVal = "#document";
    return 0;
}

result_t XmlDocument::get_nodeValue(exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::set_nodeValue(exlib::string newVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlDocument::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlDocument::checkNode(XmlNode_base* newChild)
{
    int32_t type;
    newChild->get_nodeType(type);

    if (type == xml_base::C_ELEMENT_NODE) {
        if (m_element) {
            if (m_element != newChild)
                return Runtime::setError("XmlDocument: The document node contains only one element node.");
        } else
            m_element = (XmlElement_base*)newChild;
    } else if (type == xml_base::C_DOCUMENT_TYPE_NODE) {
        if (m_doctype) {
            if (m_doctype != newChild)
                return Runtime::setError("XmlDocument: The document node contains only one doctype node.");
        } else
            m_doctype = (XmlDocumentType_base*)newChild;
    }

    return 0;
}

result_t XmlDocument::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    if (globalPrefix(namespaceURI, retVal))
        return 0;

    if (!m_element)
        return CALL_RETURN_NULL;

    return m_element->lookupPrefix(namespaceURI, retVal);
}

result_t XmlDocument::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    if (globalNamespaceURI(prefix, retVal))
        return 0;

    if (!m_element)
        return CALL_RETURN_NULL;

    return m_element->lookupNamespaceURI(prefix, retVal);
}

result_t XmlDocument::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlDocument::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlDocument::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    if (newChild == oldChild) {
        retVal = oldChild;
        return 0;
    }

    if (oldChild == m_element)
        m_element.Release();
    else if (oldChild == m_doctype)
        m_doctype.Release();

    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlDocument::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    if (oldChild == m_element)
        m_element.Release();
    else if (oldChild == m_doctype)
        m_doctype.Release();

    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlDocument::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->appendChild(newChild, retVal);
}

result_t XmlDocument::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlDocument> doc = new XmlDocument(*this);
    return XmlNodeImpl::cloneNode(doc, deep, retVal);
}

result_t XmlDocument::load(exlib::string source, v8::Local<v8::Object> options)
{
    XmlParseLimits limits;
    result_t hr = getParseLimits(options, limits);
    if (hr < 0)
        return hr;

    return load(source, limits);
}

result_t XmlDocument::load(exlib::string source, const XmlParseLimits& limits)
{
    if (m_isXml)
        return XmlParser::parse(this, source, limits);

    m_childs->removeAll();
    m_element.Release();

    return XmlParser::parseHtml(this, source, limits);
}

result_t XmlDocument::load(Buffer_base* source, v8::Local<v8::Object> options)
{
    XmlParseLimits limits;
    result_t hr = getParseLimits(options, limits);
    if (hr < 0)
        return hr;

    return load(source, limits);
}

result_t XmlDocument::load(Buffer_base* source, const XmlParseLimits& limits)
{
    exlib::string strBuf;
    result_t hr;

    source->toString(strBuf);

    if (!m_isXml) {
        _parser p(strBuf);
        const char* ptr;

        while ((ptr = qstristr(p.now(), "<meta")) != NULL && qisspace(ptr[5])) {
            bool bContentType = false;
            exlib::string content;

            p.pos = (int32_t)(ptr - p.string + 5);
            while (true) {
                exlib::string key, value;

                p.skipSpace();
                p.getWord(key, '=', '>');

                if (key.empty())
                    break;

                if (p.want('=')) {
                    if (p.want('\"')) {
                        p.getString(value, '\"', '>');
                        p.want('\"');
                    } else
                        p.getWord(value, '>');
                }

                if (!qstricmp(key.c_str(), "charset")) {
                    m_encoding = value;
                    break;
                } else if (!qstricmp(key.c_str(), "content"))
                    content = value;
                else if (!qstricmp(key.c_str(), "http-equiv") && !qstricmp(value.c_str(), "Content-Type"))
                    bContentType = true;
            }

            if (bContentType && !content.empty()) {
                _parser p1(content);

                while (true) {
                    exlib::string key, value;

                    p1.skipSpace();
                    p1.getWord(key, ';', '=');

                    if (key.empty())
                        break;

                    if (p1.want('='))
                        p1.getWord(value, ';');

                    p1.want(';');

                    if (!qstricmp(key.c_str(), "charset")) {
                        m_encoding = value;
                        break;
                    }
                }
            }

            if (!m_encoding.empty())
                break;
        }

        if (!m_encoding.empty()) {
            encoding_conv conv(m_encoding);
            conv.decode(strBuf, strBuf);
        }
    }

    hr = load(strBuf, limits);
    if (hr < 0)
        return hr;

    return 0;
}

result_t XmlDocument::get_doctype(obj_ptr<XmlDocumentType_base>& retVal)
{
    if (!m_doctype)
        return CALL_RETURN_NULL;

    retVal = m_doctype;
    return 0;
}

result_t XmlDocument::get_documentElement(obj_ptr<XmlElement_base>& retVal)
{
    if (!m_element)
        return CALL_RETURN_NULL;

    retVal = m_element;
    return 0;
}

result_t XmlDocument::createElement(exlib::string tagName, obj_ptr<XmlElement_base>& retVal)
{
    retVal = new XmlElement(this, tagName, m_isXml);
    return 0;
}

result_t XmlDocument::createElementNS(exlib::string namespaceURI, exlib::string qualifiedName,
    obj_ptr<XmlElement_base>& retVal)
{
    retVal = new XmlElement(this, namespaceURI, qualifiedName, m_isXml);
    return 0;
}

result_t XmlDocument::createTextNode(exlib::string data, obj_ptr<XmlText_base>& retVal)
{
    retVal = new XmlText(this, data);
    return 0;
}

result_t XmlDocument::createComment(exlib::string data, obj_ptr<XmlComment_base>& retVal)
{
    retVal = new XmlComment(this, data);
    return 0;
}

result_t XmlDocument::createCDATASection(exlib::string data, obj_ptr<XmlCDATASection_base>& retVal)
{
    retVal = new XmlCDATASection(this, data);
    return 0;
}

result_t XmlDocument::createProcessingInstruction(exlib::string target, exlib::string data,
    obj_ptr<XmlProcessingInstruction_base>& retVal)
{
    retVal = new XmlProcessingInstruction(this, target, data);
    return 0;
}

result_t XmlDocument::createDocumentFragment(obj_ptr<XmlDocumentFragment_base>& retVal)
{
    retVal = new XmlDocumentFragment(this);
    return 0;
}

result_t XmlDocument::importNode(XmlNode_base* importedNode, bool deep, obj_ptr<XmlNode_base>& retVal)
{
    if (!importedNode)
        return CALL_E_INVALIDARG;

    int32_t type;
    importedNode->get_nodeType(type);

    // Cannot import Document nodes
    if (type == xml_base::C_DOCUMENT_NODE)
        return Runtime::setError("XmlDocument: Cannot import a document node.");

    // Clone the node (which creates a copy with this document as owner)
    obj_ptr<XmlNode_base> clonedNode;
    result_t hr = importedNode->cloneNode(deep, clonedNode);
    if (hr < 0)
        return hr;

    // Set the owner document of the cloned node to this document
    XmlNodeImpl* impl = XmlNodeImpl::fromNode(clonedNode);
    if (impl)
        impl->setDocument(this);

    retVal = clonedNode;
    return 0;
}

result_t XmlDocument::adoptNode(XmlNode_base* adoptedNode, obj_ptr<XmlNode_base>& retVal)
{
    if (!adoptedNode)
        return CALL_E_INVALIDARG;

    int32_t type;
    adoptedNode->get_nodeType(type);

    // Cannot adopt Document nodes
    if (type == xml_base::C_DOCUMENT_NODE)
        return Runtime::setError("XmlDocument: Cannot adopt a document node.");

    // Remove the node from its current parent (if any)
    XmlNodeImpl* impl = XmlNodeImpl::fromNode(adoptedNode);
    if (impl) {
        // Remove from parent if attached
        obj_ptr<XmlNode_base> removed;
        impl->remove(removed);

        // Set the owner document to this document
        impl->setDocument(this);
    }

    retVal = adoptedNode;
    return 0;
}

result_t XmlDocument::get_head(obj_ptr<XmlElement_base>& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement* pEl = m_element.As<XmlElement>();
    if (pEl)
        return pEl->getFirstElementsByTagName("head", retVal);

    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_title(exlib::string& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement* pEl = m_element.As<XmlElement>();
    if (pEl) {
        obj_ptr<XmlElement_base> title;
        if (pEl->getFirstElementsByTagName("title", title) == CALL_RETURN_NULL)
            return 0;

        return title->get_textContent(retVal);
    }

    return 0;
}

result_t XmlDocument::get_body(obj_ptr<XmlElement_base>& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement* pEl = m_element.As<XmlElement>();
    if (pEl)
        return pEl->getFirstElementsByTagName("body", retVal);

    return CALL_RETURN_NULL;
}

result_t XmlDocument::getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    XmlElement* pEl = m_element.As<XmlElement>();

    if (pEl) {
        // '*' has no key to look up, and the index can be disabled: walk
        if (*tagName.c_str() == '*' || !useQueryIndex())
            pEl->getElementsByTagNameFromThis(tagName, ret);
        else {
            if (m_tagIndex.epoch != m_queryEpoch)
                buildTagIndex();

            // HTML documents store the upper-case tag name and compare
            // case-insensitively (exactly like the walk does)
            exlib::string key(tagName);
            if (!m_isXml)
                exlib::qstrupr(key);

            auto it = m_tagIndex.byTag.find(key);
            if (it != m_tagIndex.byTag.end()) {
                std::vector<XmlElement*>& els = it->second;
                for (size_t i = 0; i < els.size(); i++)
                    ret->appendRef(els[i]);
            }
        }
    }

    retVal = ret;
    return 0;
}

// index key for the (namespaceURI, localName) index
static void makeNsLocalKey(exlib::string& key, const exlib::string& ns, const exlib::string& local)
{
    key.assign(ns);
    key.push_back('\x1f');
    key.append(local);
}

// match an index key against a (possibly wildcarded) query
static bool nsLocalKeyMatch(const exlib::string& key, const exlib::string& ns, const exlib::string& local)
{
    size_t pos = key.find('\x1f');
    if (pos == exlib::string::npos)
        return false;

    if (*ns.c_str() != '*') {
        exlib::string kNs = key.substr(0, pos);
        if (kNs != ns)
            return false;
    }

    if (*local.c_str() != '*') {
        exlib::string kLocal = key.substr(pos + 1);
        if (kLocal != local)
            return false;
    }

    return true;
}

result_t XmlDocument::getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName,
    obj_ptr<XmlNodeList_base>& retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    XmlElement* pEl = m_element.As<XmlElement>();

    if (pEl) {
        bool wildNs = *namespaceURI.c_str() == '*';
        bool wildLocal = *localName.c_str() == '*';

        if (!useQueryIndex() || (wildNs && wildLocal))
            pEl->getElementsByTagNameNSFromThis(namespaceURI, localName, ret);
        else {
            if (m_nsIndex.epoch != m_queryEpoch)
                buildNsIndex();

            std::vector<XmlElement*>* hit = NULL;

            if (!wildNs && !wildLocal) {
                exlib::string key;
                makeNsLocalKey(key, namespaceURI, localName);

                auto it = m_nsIndex.byNsLocal.find(key);
                if (it != m_nsIndex.byNsLocal.end())
                    hit = &it->second;
            } else {
                // wildcard: the key set is small (one entry per distinct
                // (ns, local) pair), so scan it.  A single matching key is a
                // direct hit; several would need a document-order merge, so
                // fall back to the walk instead.
                size_t matches = 0;

                for (auto& kv : m_nsIndex.byNsLocal) {
                    if (nsLocalKeyMatch(kv.first, namespaceURI, localName)) {
                        matches++;
                        hit = &kv.second;
                        if (matches > 1)
                            break;
                    }
                }

                if (matches > 1) {
                    hit = NULL;
                    pEl->getElementsByTagNameNSFromThis(namespaceURI, localName, ret);
                }
            }

            if (hit) {
                for (size_t i = 0; i < hit->size(); i++)
                    ret->appendRef((*hit)[i]);
            }
        }
    }

    retVal = ret;
    return 0;
}

result_t XmlDocument::getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal)
{
    if (id.empty())
        return CHECK_ERROR(CALL_RETURN_NULL);

    XmlElement* pEl = m_element.As<XmlElement>();
    if (!pEl)
        return CHECK_ERROR(CALL_RETURN_NULL);

    if (!useQueryIndex())
        return pEl->getElementByIdFromThis(id, retVal);

    if (m_idIndex.epoch != m_queryEpoch)
        buildIdIndex();

    auto it = m_idIndex.byId.find(id);
    if (it != m_idIndex.byId.end()) {
        retVal = it->second;
        return 0;
    }

    return CHECK_ERROR(CALL_RETURN_NULL);
}

bool XmlDocument::useQueryIndex()
{
    // Escape hatch (decision D6): FIBJS_DOM_INDEX=0 disables the indexes, so a
    // missed invalidation can be worked around without rebuilding.
    static int32_t s_use = -1;

    if (s_use < 0) {
        const char* env = getenv("FIBJS_DOM_INDEX");
        s_use = (env && *env == '0') ? 0 : 1;
    }

    return s_use != 0;
}

void XmlDocument::buildTagIndex()
{
    buildQueryIndexes(true, false, false, false);
}

void XmlDocument::buildNsIndex()
{
    buildQueryIndexes(false, true, false, false);
}

void XmlDocument::buildIdIndex()
{
    buildQueryIndexes(false, false, true, false);
}

void XmlDocument::buildClassIndex()
{
    buildQueryIndexes(false, false, false, true);
}

// true when the same token already appeared earlier in the class attribute
// (a value like "a b a" must add one entry, not two)
static bool classTokenSeenBefore(const char* s, size_t start, size_t tokenLen)
{
    size_t i = 0;

    while (i < start) {
        while (i < start && qisspace(s[i]))
            i++;

        size_t t0 = i;
        while (i < start && !qisspace(s[i]))
            i++;

        if (i - t0 == tokenLen && tokenLen > 0 && memcmp(s + t0, s + start, tokenLen) == 0)
            return true;
    }

    return false;
}

void XmlDocument::addClassTokens(exlib::string& className, XmlElement* el)
{
    const char* s = className.c_str();
    size_t len = className.length();
    size_t i = 0;

    while (i < len) {
        while (i < len && qisspace(s[i]))
            i++;

        size_t start = i;
        while (i < len && !qisspace(s[i]))
            i++;

        size_t tokenLen = i - start;
        if (tokenLen == 0)
            break;

        if (classTokenSeenBefore(s, start, tokenLen))
            continue;

        m_classIndex.byClass[exlib::string(s + start, tokenLen)].push_back(el);
    }
}

void XmlDocument::buildQueryIndexes(bool wantTag, bool wantNs, bool wantId, bool wantClass)
{
    if (wantTag)
        m_tagIndex.byTag.clear();
    if (wantNs)
        m_nsIndex.byNsLocal.clear();
    if (wantId)
        m_idIndex.byId.clear();
    if (wantClass)
        m_classIndex.byClass.clear();

    XmlElement* pEl = m_element.As<XmlElement>();

    if (pEl) {
        exlib::string key;

        walkElements(pEl, true, [&](XmlNodeImpl* node) -> bool {
            XmlElement* el = static_cast<XmlElement*>(node);

            if (wantTag)
                m_tagIndex.byTag[el->tagNameRef()].push_back(el);

            if (wantNs) {
                makeNsLocalKey(key, el->namespaceURIRef(), el->localNameRef());
                m_nsIndex.byNsLocal[key].push_back(el);
            }

            if (wantId) {
                exlib::string id;

                el->get_id(id);
                // first element in document order wins (same as the walk)
                if (!id.empty() && m_idIndex.byId.find(id) == m_idIndex.byId.end())
                    m_idIndex.byId[id] = el;
            }

            if (wantClass) {
                exlib::string cls;

                el->get_className(cls);
                if (!cls.empty())
                    addClassTokens(cls, el);
            }

            return false;
        });
    }

    if (wantTag)
        m_tagIndex.epoch = m_queryEpoch;
    if (wantNs)
        m_nsIndex.epoch = m_queryEpoch;
    if (wantId)
        m_idIndex.epoch = m_queryEpoch;
    if (wantClass)
        m_classIndex.epoch = m_queryEpoch;
}

result_t XmlDocument::getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    XmlElement* pEl = m_element.As<XmlElement>();

    if (pEl) {
        std::vector<exlib::string> classNames;

        XmlElement::parseClassNames(className, classNames);

        if (classNames.size() > 0) {
            // a single class token is served by the class index; several tokens
            // would need an intersection in document order, so walk instead
            if (!useQueryIndex() || classNames.size() != 1)
                pEl->getElementsByClassNameFromThis(classNames, ret);
            else {
                if (m_classIndex.epoch != m_queryEpoch)
                    buildClassIndex();

                auto it = m_classIndex.byClass.find(classNames[0]);
                if (it != m_classIndex.byClass.end()) {
                    std::vector<XmlElement*>& els = it->second;
                    for (size_t i = 0; i < els.size(); i++)
                        ret->appendRef(els[i]);
                }
            }
        }
    }

    retVal = ret;
    return 0;
}

result_t XmlDocument::querySelector(exlib::string selectors, obj_ptr<XmlElement_base>& retVal)
{
    if (!m_element)
        return CALL_RETURN_NULL;

    XmlElement* pEl = m_element.As<XmlElement>();
    if (!pEl)
        return CALL_RETURN_NULL;

    // document-level query: the document element itself is a match candidate
    return pEl->querySelectorImpl(selectors, true, retVal);
}

result_t XmlDocument::querySelectorAll(exlib::string selectors, obj_ptr<XmlNodeList_base>& retVal)
{
    if (!m_element)
        return CALL_RETURN_NULL;

    XmlElement* pEl = m_element.As<XmlElement>();
    if (!pEl)
        return CALL_RETURN_NULL;

    // document-level query: the document element itself is a match candidate
    return pEl->querySelectorAllImpl(selectors, true, retVal);
}

result_t XmlDocument::get_inputEncoding(exlib::string& retVal)
{
    if (m_encoding.empty())
        return CALL_RETURN_NULL;

    retVal = m_encoding;
    return 0;
}

result_t XmlDocument::get_xmlStandalone(bool& retVal)
{
    if (m_standalone < 0)
        return CALL_RETURN_NULL;

    retVal = m_standalone == 1;
    return 0;
}

result_t XmlDocument::set_xmlStandalone(bool newVal)
{
    m_standalone = newVal ? 1 : 0;
    return 0;
}

result_t XmlDocument::get_xmlVersion(exlib::string& retVal)
{
    if (m_version.empty())
        retVal = "1.0";
    else
        retVal = m_version;
    return 0;
}

result_t XmlDocument::set_xmlVersion(exlib::string newVal)
{
    m_version = newVal;
    return 0;
}

result_t XmlDocument::toString(exlib::string& retVal)
{
    exlib::string strChilds;
    m_childs->toString(strChilds);

    if (!m_version.empty()) {
        retVal = "<?xml version=\"";
        retVal.append(m_version);

        if (!m_encoding.empty()) {
            retVal.append("\" encoding=\"");
            retVal.append(m_encoding);
        }

        if (m_standalone >= 0)
            retVal.append(m_standalone ? "\" standalone=\"yes\"?>" : "\" standalone=\"no\"?>");
        else
            retVal.append("\"?>");

        retVal.append(strChilds);
    } else
        retVal = strChilds;

    return 0;
}
}
