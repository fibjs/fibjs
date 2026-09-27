/*
 * XmlParser.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "object.h"
#include "XmlParser.h"
#include "XmlElement.h"
#include "XmlComment.h"
#include "XmlDocumentType.h"
#include "XmlProcessingInstruction.h"
#include "XmlText.h"
#include "XmlCDATASection.h"
#include "Runtime.h"
#include <unordered_map>
#define XML_STATIC
#include <expat/include/expat.h>

namespace fibjs {

void XmlParser::newNode(XmlNode_base* node, bool enter)
{
    if (!countNodes(1))
        return;

    obj_ptr<XmlNode_base> out;
    m_now->appendChild(node, out);

    if (enter) {
        m_now = node;
        m_list.push_back(m_now);
    }
}

// " at line N" when expat is parsing (it tracks the position), empty for the
// HTML tree builder, which has no line information.
exlib::string XmlParser::location()
{
    char buf[64];

    if (!m_xml_parser)
        return exlib::string();

    snprintf(buf, sizeof(buf), " at line %lu", XML_GetCurrentLineNumber((XML_Parser)m_xml_parser));
    return exlib::string(buf);
}

void XmlParser::abort(exlib::string msg)
{
    if (m_aborted)
        return;

    m_aborted = true;
    m_abort_msg = msg;

    // Stop expat from feeding more callbacks; XML_Parse() then returns
    // XML_ERROR_ABORTED and parse() reports m_abort_msg instead.  The HTML tree
    // builder has no such switch: it checks aborted() and stops descending.
    if (m_xml_parser)
        XML_StopParser((XML_Parser)m_xml_parser, XML_FALSE);
}

bool XmlParser::countNodes(int64_t count)
{
    if (m_limits.max_node_count <= 0)
        return true;

    m_node_count += count;

    if (m_node_count > m_limits.max_node_count) {
        char msg[192];

        snprintf(msg, sizeof(msg), "XmlParser: too many nodes, maxNodeCount is %lld%s",
            (long long)m_limits.max_node_count, location().c_str());
        abort(msg);

        return false;
    }

    return true;
}

void XmlParser::leaveNode()
{
    m_list.pop_back();
    m_now = m_list.back();
}

void XmlParser::OnXmlDecl(const XML_Char* version, const XML_Char* encoding, int32_t standalone)
{
    m_document->setDecl(version, encoding, standalone);
}

void XmlParser::OnStartDoctypeDecl(const XML_Char* doctypeName, const XML_Char* sysid,
    const XML_Char* pubid, int32_t has_internal_subset)
{
    obj_ptr<XmlDocumentType> doctype = new XmlDocumentType(m_document, doctypeName,
        sysid ? sysid : "", pubid ? pubid : "");
    newNode(doctype);
}

void XmlParser::OnStartElement(const XML_Char* name, const XML_Char** atts)
{
    const XML_Char** p = atts;
    std::unordered_map<exlib::string, exlib::string> nss;
    exlib::string def_ns;
    bool has_def = false;

    // m_list holds the open nodes with the document at the bottom, so its size
    // is the depth this element is about to have (the document element is 1).
    if (m_limits.max_element_depth > 0 && (int32_t)m_list.size() > m_limits.max_element_depth) {
        char msg[192];

        snprintf(msg, sizeof(msg), "XmlParser: document is nested too deeply, maxElementDepth is %d%s",
            m_limits.max_element_depth, location().c_str());
        abort(msg);

        return;
    }

    while (p[0] && p[1]) {
        const XML_Char* ns = p[0];

        if (!qstrcmp(ns, "xmlns", 5)) {
            if (ns[5] == ':')
                nss.insert(std::pair<exlib::string, exlib::string>(ns + 6, p[1]));
            else if (!ns[5]) {
                def_ns = p[1];
                has_def = true;
            }
        }
        p += 2;
    }

    obj_ptr<XmlElement> el;
    const char* str = qstrchr(name, ':');

    if (str) {
        exlib::string prefix(name, str - name);
        exlib::string qname(str + 1);
        std::unordered_map<exlib::string, exlib::string>::iterator it;

        it = nss.find(prefix);
        if (it != nss.end())
            def_ns = it->second;
        else
            m_now->lookupNamespaceURI(prefix, def_ns);
    } else if (!has_def) {
        int32_t type;
        m_now->get_nodeType(type);
        if (type == xml_base::C_ELEMENT_NODE)
            m_now.As<XmlElement>()->get_defaultNamespace(def_ns);
    }

    if (!def_ns.empty())
        el = new XmlElement(m_document, def_ns, name, m_isXml);
    else
        el = new XmlElement(m_document, name, m_isXml);

    newNode(el, true);

    if (m_aborted)
        return;

    while (atts[0] && atts[1]) {
        name = atts[0];

        // attributes are nodes too, and a document can carry millions of them
        if (!countNodes(1))
            return;

        str = qstrchr(name, ':');
        if (str && str[1]) {
            exlib::string ns(name, str - name);
            exlib::string qname(str + 1);
            std::unordered_map<exlib::string, exlib::string>::iterator it;

            it = nss.find(ns);
            if (it != nss.end())
                def_ns = it->second;
            else
                m_now->lookupNamespaceURI(ns, def_ns);
        } else
            def_ns.clear();

        if (!def_ns.empty())
            el->setAttributeNS(def_ns, name, atts[1]);
        else
            el->setAttribute(name, atts[1]);

        atts += 2;
    }
}

void XmlParser::OnEndElement(const XML_Char* name)
{
    leaveNode();
}

void XmlParser::OnComment(const XML_Char* data)
{
    obj_ptr<XmlComment> comment = new XmlComment(m_document, data);
    newNode(comment);
}

void XmlParser::OnProcessingInstruction(const XML_Char* target, const XML_Char* data)
{
    obj_ptr<XmlProcessingInstruction> pi = new XmlProcessingInstruction(m_document, target, data);
    newNode(pi);
}

void XmlParser::OnCharacterData(const XML_Char* s, int32_t len)
{
    exlib::string data(s, len);
    int32_t type;

    m_now->get_nodeType(type);
    if (type == xml_base::C_CDATA_SECTION_NODE)
        m_now.As<XmlCDATASection_base>()->appendData(data);
    else {
        obj_ptr<XmlNode_base> last;
        m_now->get_lastChild(last);

        if (last) {
            last->get_nodeType(type);
            if (type == xml_base::C_TEXT_NODE) {
                last.As<XmlText_base>()->appendData(data);
                return;
            }
        }

        obj_ptr<XmlText_base> text = new XmlText(m_document, data);
        newNode(text);
    }
}

void XmlParser::OnStartCdataSection()
{
    obj_ptr<XmlCDATASection> cs = new XmlCDATASection(m_document);
    newNode(cs, true);
}

void XmlParser::OnEndCdataSection()
{
    leaveNode();
}

result_t XmlParser::parse(XmlDocument* doc, exlib::string source, const XmlParseLimits& limits)
{
    XmlParser parser(doc, true, limits);

    parser.m_now = doc;
    parser.m_list.push_back(doc);

    XML_Parser xml_parser = XML_ParserCreate(NULL);
    if (!xml_parser)
        return CHECK_ERROR(Runtime::setError("XmlParser: unable to create Expat parser."));

    parser.m_xml_parser = (XML_ParserStruct*)xml_parser;

    XML_SetParamEntityParsing(xml_parser, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    XML_SetUserData(xml_parser, &parser);

    XML_SetXmlDeclHandler(xml_parser, XmlDeclHandler);
    XML_SetElementHandler(xml_parser, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(xml_parser, CharacterDataHandler);
    XML_SetProcessingInstructionHandler(xml_parser, ProcessingInstructionHandler);
    XML_SetCommentHandler(xml_parser, CommentHandler);
    XML_SetCdataSectionHandler(xml_parser, StartCdataSectionHandler, EndCdataSectionHandler);
    XML_SetStartDoctypeDeclHandler(xml_parser, StartDoctypeDeclHandler);

    if (XML_Parse(xml_parser, source.c_str(), (int32_t)source.length(), true) != XML_STATUS_OK) {
        char msg[128];

        // read the position and the error before freeing the parser
        snprintf(msg, sizeof(msg), "XmlParser: error on line %lu at column %lu: %s", XML_GetCurrentLineNumber(xml_parser),
            XML_GetCurrentColumnNumber(xml_parser) + 1,
            XML_ErrorString(XML_GetErrorCode(xml_parser)));

        XML_ParserFree(xml_parser);
        parser.m_xml_parser = NULL;

        // A limit aborted the parse; report the limit, not expat's "aborted"
        // error code.
        if (parser.m_aborted)
            return CHECK_ERROR(Runtime::setError(parser.m_abort_msg));

        return CHECK_ERROR(Runtime::setError(msg));
    }

    XML_ParserFree(xml_parser);

    return 0;
}

} /* namespace fibjs */
