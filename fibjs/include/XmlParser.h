/*
 * XmlParser.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#pragma once

#include "utils.h"

#include "XmlDocument.h"
#include "XmlParseLimits.h"
#include <list>

// expat's parser handle is an opaque pointer: XML_ParserStruct is only
// completed in XmlParser.cpp, so the header stays free of the expat API.
struct XML_ParserStruct;

namespace fibjs {

class XmlParser {
public:
    XmlParser(XmlDocument* document, bool isXml, const XmlParseLimits& limits)
        : m_document(document)
        , m_isXml(isXml)
        , m_limits(limits)
        , m_node_count(0)
        , m_aborted(false)
        , m_xml_parser(NULL)
    {
    }

public:
    static result_t parse(XmlDocument* doc, exlib::string source, const XmlParseLimits& limits);
    static result_t parseHtml(XmlDocument* doc, exlib::string source, const XmlParseLimits& limits);

    // True once a limit stopped the parse.  The HTML tree builder polls this to
    // stop descending; the expat path is stopped through XML_StopParser().
    bool aborted()
    {
        return m_aborted;
    }

    void OnXmlDecl(const char* version, const char* encoding, int32_t standalone);
    void OnStartElement(const char* name, const char** atts);
    void OnEndElement(const char* name);
    void OnCharacterData(const char* s, int32_t len);
    void OnProcessingInstruction(const char* target, const char* data);
    void OnComment(const char* data);
    void OnStartCdataSection();
    void OnEndCdataSection();

    void OnStartDoctypeDecl(const char* doctypeName, const char* sysid,
        const char* pubid, int32_t has_internal_subset);

private:
    void leaveNode();
    void newNode(XmlNode_base* node, bool enter = false);

    // Stop the parse with `msg` as the error.  Idempotent: the first limit that
    // trips wins, and the expat parser is aborted so it stops feeding the
    // callbacks (the tree builder checks aborted() instead).
    void abort(exlib::string msg);
    // Account for `count` new nodes; false when maxNodeCount is exceeded.
    bool countNodes(int64_t count);
    // " at line N" when the source is being parsed by expat, empty otherwise.
    exlib::string location();

private:
    static void XmlDeclHandler(void* userData, const char* version,
        const char* encoding, int32_t standalone)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnXmlDecl(version, encoding, standalone);
    }

    static void StartElementHandler(void* userData, const char* name,
        const char** atts)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnStartElement(name, atts);
    }

    static void EndElementHandler(void* userData, const char* name)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnEndElement(name);
    }

    static void CharacterDataHandler(void* userData, const char* s, int32_t len)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnCharacterData(s, len);
    }

    static void ProcessingInstructionHandler(void* userData,
        const char* target, const char* data)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnProcessingInstruction(target, data);
    }

    static void CommentHandler(void* userData, const char* data)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnComment(data);
    }

    static void StartCdataSectionHandler(void* userData)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnStartCdataSection();
    }

    static void EndCdataSectionHandler(void* userData)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnEndCdataSection();
    }

    static void StartDoctypeDeclHandler(void* userData,
        const char* doctypeName, const char* sysid,
        const char* pubid, int32_t has_internal_subset)
    {
        XmlParser* pThis = static_cast<XmlParser*>(userData);
        pThis->OnStartDoctypeDecl(doctypeName, sysid, pubid, has_internal_subset);
    }

private:
    obj_ptr<XmlDocument> m_document;
    obj_ptr<XmlNode_base> m_now;
    std::list<obj_ptr<XmlNode_base>> m_list;
    bool m_isXml;

    XmlParseLimits m_limits;
    int64_t m_node_count;
    bool m_aborted;
    exlib::string m_abort_msg;
    XML_ParserStruct* m_xml_parser;
};

} /* namespace fibjs */
