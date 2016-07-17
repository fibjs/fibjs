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
#include <map>
#define XML_STATIC
#include <expat/include/expat.h>

namespace fibjs
{

void XmlParser::newNode(XmlNode_base *node, bool enter)
{
    obj_ptr<XmlNode_base> out;
    m_now->appendChild(node, out);

    if (enter)
    {
        m_now = node;
        m_list.push_back(m_now);
    }
}

void XmlParser::leaveNode()
{
    m_list.pop_back();
    m_now = m_list.back();
}

void XmlParser::OnXmlDecl(const XML_Char *version, const XML_Char *encoding, int32_t standalone)
{
    m_document->setDecl(version, encoding, standalone);
}

void XmlParser::OnStartDoctypeDecl(const XML_Char *doctypeName, const XML_Char *sysid,
                                   const XML_Char *pubid, int32_t has_internal_subset)
{
    obj_ptr<XmlDocumentType> doctype = new XmlDocumentType(m_document, doctypeName,
            sysid ? sysid : "", pubid ? pubid : "");
    newNode(doctype);
}

void XmlParser::OnStartElement(const XML_Char *name, const XML_Char **atts)
{
    const XML_Char **p = atts;
    std::map<exlib::string, exlib::string> nss;
    exlib::string def_ns;
    bool has_def = false;

    while (p[0] && p[1])
    {
        const XML_Char *ns = p[0];

        if (!qstrcmp(ns, "xmlns", 5))
        {
            if (ns[5] == ':')
                nss.insert(std::pair<exlib::string, exlib::string>(ns + 6, p[1]));
            else if (!ns[5])
            {
                def_ns = p[1];
                has_def = true;
            }
        }
        p += 2;
    }

    obj_ptr<XmlElement> el;
    const char *str = qstrchr(name, ':');

    if (str)
    {
        exlib::string prefix(name, str - name);
        exlib::string qname(str + 1);
        std::map<exlib::string, exlib::string>::iterator it;

        it = nss.find(prefix);
        if (it != nss.end())
            def_ns = it->second;
        else
            m_now->lookupNamespaceURI(prefix.c_str(), def_ns);
    }
    else if (!has_def)
    {
        int32_t type;
        m_now->get_nodeType(type);
        if (type == xml_base::_ELEMENT_NODE)
            ((XmlElement *)(XmlNode_base *)m_now)->get_defaultNamespace(def_ns);
    }

    if (!def_ns.empty())
        el = new XmlElement(m_document, def_ns.c_str(), name, m_isXml);
    else
        el = new XmlElement(m_document, name, m_isXml);

    newNode(el, true);

    while (atts[0] && atts[1])
    {
        name = atts[0];

        str = qstrchr(name, ':');
        if (str && str[1])
        {
            exlib::string ns(name, str - name);
            exlib::string qname(str + 1);
            std::map<exlib::string, exlib::string>::iterator it;

            it = nss.find(ns);
            if (it != nss.end())
                def_ns = it->second;
            else
                m_now->lookupNamespaceURI(ns.c_str(), def_ns);
        }
        else
            def_ns.clear();

        if (!def_ns.empty())
            el->setAttributeNS(def_ns.c_str(), name, atts[1]);
        else
            el->setAttribute(name, atts[1]);

        atts += 2;
    }
}

void XmlParser::OnEndElement(const XML_Char *name)
{
    leaveNode();
}

void XmlParser::OnComment(const XML_Char *data)
{
    obj_ptr<XmlComment> comment = new XmlComment(m_document, data);
    newNode(comment);
}

void XmlParser::OnProcessingInstruction(const XML_Char *target, const XML_Char *data)
{
    obj_ptr<XmlProcessingInstruction> pi = new XmlProcessingInstruction(m_document, target, data);
    newNode(pi);
}

void XmlParser::OnCharacterData(const XML_Char *s, int32_t len)
{
    exlib::string data(s, len);
    int32_t type;

    m_now->get_nodeType(type);
    if (type == xml_base::_CDATA_SECTION_NODE)
        ((XmlCDATASection_base *)(XmlNode_base *)m_now)->appendData(data.c_str());
    else
    {
        obj_ptr<XmlNode_base> last;
        m_now->get_lastChild(last);

        if (last)
        {
            last->get_nodeType(type);
            if (type == xml_base::_TEXT_NODE)
            {
                ((XmlText_base *)(XmlNode_base *)last)->appendData(data.c_str());
                return;
            }
        }

        obj_ptr<XmlText_base> text = new XmlText(m_document, data.c_str());
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

result_t XmlParser::parse(XmlDocument *doc, const char *source)
{
    XmlParser parser(doc, true);

    parser.m_now = doc;
    parser.m_list.push_back(doc);

    XML_Parser xml_parser = XML_ParserCreate(NULL);

    XML_SetParamEntityParsing(xml_parser, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    XML_SetUserData(xml_parser, &parser);

    XML_SetXmlDeclHandler(xml_parser, XmlDeclHandler);
    XML_SetElementHandler(xml_parser, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(xml_parser, CharacterDataHandler);
    XML_SetProcessingInstructionHandler(xml_parser, ProcessingInstructionHandler);
    XML_SetCommentHandler(xml_parser, CommentHandler);
    XML_SetCdataSectionHandler(xml_parser, StartCdataSectionHandler, EndCdataSectionHandler);
    XML_SetStartDoctypeDeclHandler(xml_parser, StartDoctypeDeclHandler);

    if (XML_Parse(xml_parser, source, (int32_t)qstrlen(source), true) != XML_STATUS_OK)
    {
        char msg[128];
        sprintf(msg, "XmlParser: error on line %lu at colum %lu: %s", XML_GetCurrentLineNumber(xml_parser),
                XML_GetCurrentColumnNumber(xml_parser) + 1,
                XML_ErrorString(XML_GetErrorCode(xml_parser)));

        XML_ParserFree(xml_parser);
        return CHECK_ERROR(Runtime::setError(msg));
    }

    XML_ParserFree(xml_parser);

    return 0;
}

} /* namespace fibjs */
