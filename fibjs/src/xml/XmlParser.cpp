/*
 * XmlParser.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "XmlParser.h"
#include "XmlElement.h"
#include "XmlComment.h"
#include "XmlDocumentType.h"
#include "XmlProcessingInstruction.h"
#include "XmlText.h"
#include "XmlCDATASection.h"
#include "Runtime.h"

namespace fibjs
{

XmlParser::XmlParser(XmlDocument *document)
{
    m_document = document;

    m_parser = XML_ParserCreate(NULL);

    XML_SetParamEntityParsing(m_parser, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    XML_SetUserData(m_parser, this);

    XML_SetXmlDeclHandler(m_parser, XmlDeclHandler);
    XML_SetElementHandler(m_parser, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(m_parser, CharacterDataHandler);
    XML_SetProcessingInstructionHandler(m_parser, ProcessingInstructionHandler);
    XML_SetCommentHandler(m_parser, CommentHandler);
    XML_SetCdataSectionHandler(m_parser, StartCdataSectionHandler, EndCdataSectionHandler);
    XML_SetStartDoctypeDeclHandler(m_parser, StartDoctypeDeclHandler);
}

void XmlParser::newNode(XmlNode_base *node, bool enter)
{
    obj_ptr<XmlNode_base> out;

    m_now->appendChild(node, out);

    if (enter)
    {
        m_now = node;
        m_list.push_back(node);
    }
}

void XmlParser::leaveNode()
{
    m_list.pop_back();
    m_now = m_list.back();
}

void XmlParser::OnXmlDecl(const XML_Char *version, const XML_Char *encoding, int standalone)
{
    m_document->setDecl(version, encoding, standalone);
}

void XmlParser::OnStartDoctypeDecl(const XML_Char *doctypeName, const XML_Char *sysid,
                                   const XML_Char *pubid, int has_internal_subset)
{
    obj_ptr<XmlDocumentType> doctype = new XmlDocumentType(m_document, doctypeName,
            sysid ? sysid : "", pubid ? pubid : "");
    newNode(doctype);
}

void XmlParser::OnStartElement(const XML_Char *name, const XML_Char **atts)
{
    obj_ptr<XmlElement> el = new XmlElement(m_document, name);
    newNode(el, true);

    while (atts[0] && atts[1])
    {
        el->setAttribute(atts[0], atts[1]);
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

void XmlParser::OnCharacterData(const XML_Char *s, int len)
{
    std::string data(s, len);
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

result_t XmlParser::parse(const char *source)
{
    m_now = m_document;
    m_list.push_back(m_now);

    if (XML_Parse(m_parser, source, (int)qstrlen(source), true) != XML_STATUS_OK)
    {
        char msg[128];
        sprintf(msg, "error on line %lu at colum %lu: %s", XML_GetCurrentLineNumber(m_parser),
                XML_GetCurrentColumnNumber(m_parser) + 1,
                XML_ErrorString(XML_GetErrorCode(m_parser)));

        return CHECK_ERROR(Runtime::setError(msg));
    }

    return 0;
}

} /* namespace fibjs */
