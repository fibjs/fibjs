/*
 * XmlParser.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "utils.h"
#define XML_STATIC
#include <expat/include/expat.h>

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

namespace fibjs
{

class XmlParser
{
public:
    XmlParser();
    ~XmlParser()
    {
        XML_ParserFree(m_parser);
    }

public:
    result_t parse(const char *source);

    void OnElementDecl(const XML_Char *name, XML_Content *model)
    {
        puts("OnElementDecl");
    }

    void OnAttlistDecl(const XML_Char *elname, const XML_Char *attname, const XML_Char *att_type,
                       const XML_Char *dflt, int isrequired)
    {
        puts("OnAttlistDecl");
    }

    void OnXmlDecl(const XML_Char *version, const XML_Char *encoding, int standalone)
    {
        puts("OnXmlDecl");
    }

    void OnStartElement(const XML_Char *name, const XML_Char **atts)
    {
        puts("------OnStartElement");
        puts(name);
        while (*atts)
        {
            puts(*atts);
            atts ++;
        }
    }

    void OnEndElement(const XML_Char *name)
    {
        puts("-------OnEndElement");
    }

    void OnCharacterData(const XML_Char *s, int len)
    {
        puts("OnCharacterData");
        puts(std::string(s, len).c_str());
    }

    void OnProcessingInstruction(const XML_Char *target, const XML_Char *data)
    {
        puts("OnProcessingInstruction");
    }

    void OnComment(const XML_Char *data)
    {
        puts("OnComment");
    }

    void OnStartCdataSection()
    {
        puts("OnStartCdataSection");
    }

    void OnEndCdataSection()
    {
        puts("OnEndCdataSection");
    }

    void OnDefault(const XML_Char *s, int len)
    {
        puts("OnDefault");
    }

    void OnStartDoctypeDecl(const XML_Char *doctypeName, const XML_Char *sysid,
                            const XML_Char *pubid, int has_internal_subset)
    {
        puts("OnStartDoctypeDecl");
    }

    void OnEndDoctypeDecl()
    {
        puts("OnEndDoctypeDecl");
    }

    void OnEntityDecl(const XML_Char *entityName, int is_parameter_entity, const XML_Char *value,
                      int value_length, const XML_Char *base, const XML_Char *systemId,
                      const XML_Char *publicId, const XML_Char *notationName)
    {
        puts("OnEntityDecl");
    }

    void OnUnparsedEntityDecl(const XML_Char *entityName, const XML_Char *base,
                              const XML_Char *systemId, const XML_Char *publicId,
                              const XML_Char *notationName)
    {
        puts("OnUnparsedEntityDecl");
    }

    void OnNotationDecl(const XML_Char *notationName, const XML_Char *base, const XML_Char *systemId,
                        const XML_Char *publicId)
    {
        puts("OnNotationDecl");
    }

    void OnStartNamespaceDecl(const XML_Char *prefix, const XML_Char *uri)
    {
        puts("OnStartNamespaceDecl");
    }

    void OnEndNamespaceDecl(const XML_Char *prefix)
    {
        puts("OnEndNamespaceDecl");
    }

    int OnNotStandalone()
    {
        puts("OnNotStandalone");
        return 0;
    }

    void OnSkippedEntity(const XML_Char *entityName, int is_parameter_entity)
    {
        puts("OnSkippedEntity");
    }

private:
    static void XMLCALL ElementDeclHandler(void *userData, const XML_Char *name,
                                           XML_Content *model)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnElementDecl(name, model);
    }

    static void XMLCALL AttlistDeclHandler(void *userData, const XML_Char *elname,
                                           const XML_Char *attname, const XML_Char *att_type,
                                           const XML_Char *dflt, int isrequired)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnAttlistDecl(elname, attname, att_type, dflt, isrequired);
    }


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

    static void XMLCALL DefaultHandler(void *userData, const XML_Char *s, int len)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnDefault(s, len);
    }

    static void XMLCALL StartDoctypeDeclHandler(void *userData,
            const XML_Char *doctypeName, const XML_Char *sysid,
            const XML_Char *pubid, int has_internal_subset)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartDoctypeDecl(doctypeName, sysid, pubid, has_internal_subset);
    }

    static void XMLCALL EndDoctypeDeclHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndDoctypeDecl();
    }

    static void XMLCALL EntityDeclHandler(void *userData, const XML_Char *entityName,
                                          int is_parameter_entity, const XML_Char *value,
                                          int value_length, const XML_Char *base,
                                          const XML_Char *systemId, const XML_Char *publicId,
                                          const XML_Char *notationName)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEntityDecl(entityName, is_parameter_entity, value,
                            value_length, base, systemId, publicId, notationName);
    }

    static void XMLCALL UnparsedEntityDeclHandler(void *userData, const XML_Char *entityName,
            const XML_Char *base, const XML_Char *systemId,
            const XML_Char *publicId, const XML_Char *notationName)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnUnparsedEntityDecl(entityName, base, systemId, publicId, notationName);
    }

    static void XMLCALL NotationDeclHandler(void *userData, const XML_Char *notationName,
                                            const XML_Char *base, const XML_Char *systemId,
                                            const XML_Char *publicId)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnNotationDecl(notationName, base,  systemId, publicId);
    }

    static void XMLCALL StartNamespaceDeclHandler(void *userData, const XML_Char *prefix,
            const XML_Char *uri)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnStartNamespaceDecl(prefix, uri);
    }

    static void XMLCALL EndNamespaceDeclHandler(void *userData, const XML_Char *prefix)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnEndNamespaceDecl(prefix);
    }

    static int XMLCALL NotStandaloneHandler(void *userData)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        return pThis->OnNotStandalone();
    }

    static void XMLCALL SkippedEntityHandler(void *userData, const XML_Char *entityName,
            int is_parameter_entity)
    {
        XmlParser *pThis = static_cast <XmlParser *>(userData);
        pThis->OnSkippedEntity(entityName, is_parameter_entity);
    }

private:
    XML_Parser m_parser;
};

} /* namespace fibjs */
#endif /* XMLPARSER_H_ */
