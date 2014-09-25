/*
 * XmlParser.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "utils.h"

#include "XmlDocument.h"
#include <list>

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

namespace fibjs
{

class XmlParser
{
public:
    XmlParser(XmlDocument *document, bool isXml) :
        m_document(document), m_isXml(isXml)
    {}

public:
    static result_t parse(XmlDocument *doc, const char *source);
    static result_t parseHtml(XmlDocument *doc, const char *source);

    void OnXmlDecl(const char *version, const char *encoding, int standalone);
    void OnStartElement(const char *name, const char **atts);
    void OnEndElement(const char *name);
    void OnCharacterData(const char *s, int len);
    void OnProcessingInstruction(const char *target, const char *data);
    void OnComment(const char *data);
    void OnStartCdataSection();
    void OnEndCdataSection();

    void OnStartDoctypeDecl(const char *doctypeName, const char *sysid,
                            const char *pubid, int has_internal_subset);

private:
    void leaveNode();
    void newNode(XmlNode_base *node, bool enter = false);

private:
    static void XmlDeclHandler(void *userData, const char *version,
                               const char *encoding, int standalone)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnXmlDecl(version, encoding, standalone);
    }

    static void StartElementHandler(void *userData, const char *name,
                                    const char **atts)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartElement(name, atts);
    }

    static void EndElementHandler(void *userData, const char *name)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndElement(name);
    }

    static void CharacterDataHandler(void *userData, const char *s, int len)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnCharacterData(s, len);
    }

    static void ProcessingInstructionHandler(void *userData,
            const char *target, const char *data)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnProcessingInstruction(target, data);
    }

    static void CommentHandler(void *userData, const char *data)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnComment(data);
    }

    static void StartCdataSectionHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartCdataSection();
    }

    static void EndCdataSectionHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndCdataSection();
    }

    static void StartDoctypeDeclHandler(void *userData,
                                        const char *doctypeName, const char *sysid,
                                        const char *pubid, int has_internal_subset)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartDoctypeDecl(doctypeName, sysid, pubid, has_internal_subset);
    }

private:
    obj_ptr<XmlDocument> m_document;
    obj_ptr<XmlNode_base> m_now;
    std::list<obj_ptr<XmlNode_base> > m_list;
    bool m_isXml;
};

} /* namespace fibjs */
#endif /* XMLPARSER_H_ */
