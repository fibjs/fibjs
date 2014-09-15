/*
 * XmlParser.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "utils.h"

#define XML_STATIC
#include <expat/include/expat.h>
#include "XmlDocument.h"
#include <list>

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

namespace fibjs
{

class XmlParser
{
public:
    XmlParser(XmlDocument *document);
    ~XmlParser()
    {
        XML_ParserFree(m_parser);
    }

public:
    result_t parse(const char *source);

    void OnXmlDecl(const XML_Char *version, const XML_Char *encoding, int standalone);
    void OnStartElement(const XML_Char *name, const XML_Char **atts);
    void OnEndElement(const XML_Char *name);
    void OnCharacterData(const XML_Char *s, int len);
    void OnProcessingInstruction(const XML_Char *target, const XML_Char *data);
    void OnComment(const XML_Char *data);
    void OnStartCdataSection();
    void OnEndCdataSection();

    void OnStartDoctypeDecl(const XML_Char *doctypeName, const XML_Char *sysid,
                            const XML_Char *pubid, int has_internal_subset);

private:
    void leaveNode();
    void newNode(XmlNode_base *node, bool enter = false);

private:
    static void XMLCALL XmlDeclHandler(void *userData, const XML_Char *version,
                                       const XML_Char *encoding, int standalone)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnXmlDecl(version, encoding, standalone);
    }

    static void XMLCALL StartElementHandler(void *userData, const XML_Char *name,
                                            const XML_Char **atts)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartElement(name, atts);
    }

    static void XMLCALL EndElementHandler(void *userData, const XML_Char *name)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndElement(name);
    }

    static void XMLCALL CharacterDataHandler(void *userData, const XML_Char *s, int len)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnCharacterData(s, len);
    }

    static void XMLCALL ProcessingInstructionHandler(void *userData,
            const XML_Char *target, const XML_Char *data)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnProcessingInstruction(target, data);
    }

    static void XMLCALL CommentHandler(void *userData, const XML_Char *data)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnComment(data);
    }

    static void XMLCALL StartCdataSectionHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartCdataSection();
    }

    static void XMLCALL EndCdataSectionHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndCdataSection();
    }

    static void XMLCALL StartDoctypeDeclHandler(void *userData,
            const XML_Char *doctypeName, const XML_Char *sysid,
            const XML_Char *pubid, int has_internal_subset)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartDoctypeDecl(doctypeName, sysid, pubid, has_internal_subset);
    }

private:
    XML_Parser m_parser;
    obj_ptr<XmlDocument> m_document;
    obj_ptr<XmlNode_base> m_now;
    std::list<obj_ptr<XmlNode_base> > m_list;
};

} /* namespace fibjs */
#endif /* XMLPARSER_H_ */
