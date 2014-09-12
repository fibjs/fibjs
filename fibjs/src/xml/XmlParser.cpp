/*
 * XmlParser.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "XmlParser.h"
#include "Runtime.h"

namespace fibjs
{

XmlParser::XmlParser()
{
    m_parser = XML_ParserCreate(NULL);

    XML_SetParamEntityParsing(m_parser, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    XML_SetUserData(m_parser, this);

    XML_SetElementDeclHandler(m_parser, ElementDeclHandler);
    XML_SetAttlistDeclHandler(m_parser, AttlistDeclHandler);
    XML_SetXmlDeclHandler(m_parser, XmlDeclHandler);
    XML_SetEntityDeclHandler(m_parser, EntityDeclHandler);
    XML_SetElementHandler(m_parser, StartElementHandler, EndElementHandler);
    XML_SetCharacterDataHandler(m_parser, CharacterDataHandler);
    XML_SetProcessingInstructionHandler(m_parser, ProcessingInstructionHandler);
    XML_SetCommentHandler(m_parser, CommentHandler);
    XML_SetCdataSectionHandler(m_parser, StartCdataSectionHandler, EndCdataSectionHandler);
    XML_SetDefaultHandler(m_parser, DefaultHandler);
    XML_SetDoctypeDeclHandler(m_parser, StartDoctypeDeclHandler, EndDoctypeDeclHandler);
    XML_SetUnparsedEntityDeclHandler(m_parser, UnparsedEntityDeclHandler);
    XML_SetNotationDeclHandler(m_parser, NotationDeclHandler);
    XML_SetNamespaceDeclHandler(m_parser, StartNamespaceDeclHandler, EndNamespaceDeclHandler);
    XML_SetNotStandaloneHandler(m_parser, NotStandaloneHandler);
    XML_SetSkippedEntityHandler(m_parser, SkippedEntityHandler);
    //XML_SetExternalEntityRefHandler(m_parser, ExternalEntityRefHandler);
    //XML_SetUnknownEncodingHandler(m_parser, UnknownEncodingHandler);

    XML_UseParserAsHandlerArg(m_parser);
}

result_t XmlParser::parse(const char *source)
{
    if (XML_Parse(m_parser, source, (int)qstrlen(source), true) != XML_STATUS_OK)
    {
        char msg[128];
        sprintf(msg, "error on line %lu at colum %lu: %s", XML_GetCurrentLineNumber(m_parser),
                XML_GetCurrentColumnNumber(m_parser) + 1,
                XML_ErrorString(XML_GetErrorCode(m_parser)));

        return Runtime::setError(msg);
    }

    return 0;
}

} /* namespace fibjs */
