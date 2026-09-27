/*
 * XmlDocument.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlDocument.h"
#include "XmlNodeMixin.h"
#include "XmlParseLimits.h"
#include <unordered_map>
#include <vector>

namespace fibjs {

class XmlElement;

class XmlDocument : public XmlNodeMixin<XmlDocument, XmlDocument_base> {
public:
    XmlDocument(bool isXml)
        : XmlNodeMixin<XmlDocument, XmlDocument_base>(NULL, this, xml_base::C_DOCUMENT_NODE)
        , m_isXml(isXml)
        , m_standalone(-1)
    {
        m_document = this;
    }

    XmlDocument(const XmlDocument& from)
        : XmlNodeMixin<XmlDocument, XmlDocument_base>(NULL, this, xml_base::C_DOCUMENT_NODE)
        , m_isXml(from.m_isXml)
        , m_version(from.m_version)
        , m_encoding(from.m_encoding)
        , m_standalone(from.m_standalone)
    {
        m_document = this;
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base - custom implementations
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);
    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);

public:
    // XmlDocument_base
    virtual result_t load(exlib::string source, v8::Local<v8::Object> options = v8::Local<v8::Object>());
    virtual result_t load(Buffer_base* source, v8::Local<v8::Object> options = v8::Local<v8::Object>());
    virtual result_t get_inputEncoding(exlib::string& retVal);
    virtual result_t get_xmlStandalone(bool& retVal);
    virtual result_t set_xmlStandalone(bool newVal);
    virtual result_t get_xmlVersion(exlib::string& retVal);
    virtual result_t set_xmlVersion(exlib::string newVal);
    virtual result_t get_doctype(obj_ptr<XmlDocumentType_base>& retVal);
    virtual result_t get_head(obj_ptr<XmlElement_base>& retVal);
    virtual result_t get_title(exlib::string& retVal);
    virtual result_t get_body(obj_ptr<XmlElement_base>& retVal);
    virtual result_t get_documentElement(obj_ptr<XmlElement_base>& retVal);
    virtual result_t getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal);
    virtual result_t getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t createElement(exlib::string tagName, obj_ptr<XmlElement_base>& retVal);
    virtual result_t createElementNS(exlib::string namespaceURI, exlib::string qualifiedName, obj_ptr<XmlElement_base>& retVal);
    virtual result_t createTextNode(exlib::string data, obj_ptr<XmlText_base>& retVal);
    virtual result_t createComment(exlib::string data, obj_ptr<XmlComment_base>& retVal);
    virtual result_t createCDATASection(exlib::string data, obj_ptr<XmlCDATASection_base>& retVal);
    virtual result_t createProcessingInstruction(exlib::string target, exlib::string data, obj_ptr<XmlProcessingInstruction_base>& retVal);
    virtual result_t createDocumentFragment(obj_ptr<XmlDocumentFragment_base>& retVal);
    virtual result_t importNode(XmlNode_base* importedNode, bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t adoptNode(XmlNode_base* adoptedNode, obj_ptr<XmlNode_base>& retVal);
    virtual result_t querySelector(exlib::string selectors, obj_ptr<XmlElement_base>& retVal);
    virtual result_t querySelectorAll(exlib::string selectors, obj_ptr<XmlNodeList_base>& retVal);

public:
    void setDecl(const char* version, const char* encoding, int32_t standalone)
    {
        m_version = version ? version : "";
        m_encoding = encoding ? encoding : "";
        m_standalone = standalone;
    }

    void create_root();

    // Parse with explicit limits (XmlParseLimits.h).  The IDL-facing overloads
    // above read them from their options argument; these are what the parser
    // and the callers inside the module use.
    result_t load(exlib::string source, const XmlParseLimits& limits);
    result_t load(Buffer_base* source, const XmlParseLimits& limits);

public:
    // ------------------------------------------------------------------
    // Lazy document-level query indexes (remediation plan B3 / T8-T10)
    //
    // Document-level getElementsByTagName(NS) / getElementById used to walk
    // the whole tree on every call.  The indexes below are built on the first
    // such query and rebuilt lazily whenever the tree changes: every mutation
    // funnels through XmlNodeImpl::setParent()/clearParent() (structure) or
    // XmlNamedNodeMap / XmlAttr::set_value() (attributes), and all of them
    // call bumpQueryEpoch().
    //
    // The indexes store raw pointers on purpose (no Ref): a node can only
    // disappear together with an epoch bump, so an index can never outlive its
    // nodes -- but it MUST be discarded (epoch mismatch) before it is used
    // again.  Never read one of these maps without checking
    // `epoch == m_queryEpoch`.
    // ------------------------------------------------------------------
    uint64_t m_queryEpoch = 1;

    void bumpQueryEpoch()
    {
        m_queryEpoch++;
    }

    // tagName -> elements in document order (key already normalised: HTML mode
    // stores the upper-case form, XML mode the exact one)
    struct TagIndex {
        uint64_t epoch = 0; // 0 = never built
        std::unordered_map<exlib::string, std::vector<XmlElement*>> byTag;
    };

    // "namespaceURI \x1f localName" -> elements in document order
    struct NsIndex {
        uint64_t epoch = 0;
        std::unordered_map<exlib::string, std::vector<XmlElement*>> byNsLocal;
    };

    // id attribute -> first element in document order
    struct IdIndex {
        uint64_t epoch = 0;
        std::unordered_map<exlib::string, XmlElement*> byId;
    };

    // class token -> elements in document order
    struct ClassIndex {
        uint64_t epoch = 0;
        std::unordered_map<exlib::string, std::vector<XmlElement*>> byClass;
    };

    TagIndex m_tagIndex;
    NsIndex m_nsIndex;
    IdIndex m_idIndex;
    ClassIndex m_classIndex;

    // Build the requested index if it is stale (one walk builds them all)
    void buildTagIndex();
    void buildNsIndex();
    void buildIdIndex();
    void buildClassIndex();

    // document element (NULL when the document has none); used by the query
    // fast path to tell document-level queries from element-level ones
    XmlElement* documentElement()
    {
        return m_element.As<XmlElement>();
    }

    // Escape hatch (decision D6): FIBJS_DOM_INDEX=0 turns the indexes off so a
    // missed invalidation can be worked around without a rebuild
    static bool useQueryIndex();

private:
    void buildQueryIndexes(bool wantTag, bool wantNs, bool wantId, bool wantClass);
    void addClassTokens(exlib::string& className, XmlElement* el);

private:
    result_t checkNode(XmlNode_base* newChild);

private:
    bool m_isXml;
    obj_ptr<XmlDocumentType_base> m_doctype;
    obj_ptr<XmlElement_base> m_element;
    exlib::string m_version;
    exlib::string m_encoding;
    int32_t m_standalone;
};

} /* namespace fibjs */
