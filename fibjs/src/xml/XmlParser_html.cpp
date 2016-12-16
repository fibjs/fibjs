/*
 * XmlParser_html.cpp
 *
 *  Created on: Sep 22, 2014
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

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif

#include <gumbo/include/gumbo.h>

namespace fibjs
{

inline void buildChilds(XmlParser &parser, GumboVector &childs);

inline void buildElement(XmlParser &parser, GumboElement &el)
{
    const char *tag = gumbo_normalized_tagname(el.tag);
    std::vector<const char *> attrs;
    int32_t i;

    attrs.resize(el.attributes.length * 2 + 2);
    for (i = 0; i < (int32_t)el.attributes.length; i ++)
    {
        GumboAttribute *attr = (GumboAttribute *)el.attributes.data[i];
        attrs[i * 2] = attr->name;
        attrs[i * 2 + 1] = attr->value;
    }

    attrs[i * 2] = NULL;
    attrs[i * 2 + 1] = NULL;

    parser.OnStartElement(tag, attrs.data());
    if (el.tag == GUMBO_TAG_SCRIPT || el.tag == GUMBO_TAG_STYLE)
    {
        if (el.children.length > 0)
            ((GumboNode **)el.children.data)[0]->type = GUMBO_NODE_COMMENT;
    }

    buildChilds(parser, el.children);
    parser.OnEndElement(tag);
}

inline void buildText(XmlParser &parser, GumboText &text)
{
    parser.OnCharacterData(text.text, (int32_t)qstrlen(text.text));
}

inline void buildCData(XmlParser &parser, GumboText &text)
{
    parser.OnStartCdataSection();
    parser.OnCharacterData(text.text, (int32_t)qstrlen(text.text));
    parser.OnEndCdataSection();
}

inline void buildCommen(XmlParser &parser, GumboText &text)
{
    // fix gumbo cdata parse error
    if (!qstricmp(text.original_text.data, "<![CDATA[", 9))
    {
        int32_t len = (int32_t)qstrlen(text.text) - 1;

        if (text.text[len - 1] == ']')
        {
            len --;
            if (text.text[len - 1] == ']')
                len --;
        }

        parser.OnStartCdataSection();
        parser.OnCharacterData(text.text + 7, (int32_t)qstrlen(text.text) - 7);
        parser.OnEndCdataSection();
    }
    else
        parser.OnComment(text.text);
}

inline void buildChilds(XmlParser &parser, GumboVector &childs)
{
    int32_t i;
    GumboNode **nodes = (GumboNode **)childs.data;

    for (i = 0; i < (int32_t)childs.length; i ++)
        switch (nodes[i]->type)
        {
        case GUMBO_NODE_ELEMENT:
            buildElement(parser, nodes[i]->v.element);
            break;
        case GUMBO_NODE_TEXT:
        case GUMBO_NODE_WHITESPACE:
            buildText(parser, nodes[i]->v.text);
            break;
        case GUMBO_NODE_CDATA:
            buildCData(parser, nodes[i]->v.text);
            break;
        case GUMBO_NODE_COMMENT:
            buildCommen(parser, nodes[i]->v.text);
            break;
        default:
            break;
        }
}

inline void buildDocument(XmlParser &parser, GumboDocument &document)
{
    if (document.has_doctype)
        parser.OnStartDoctypeDecl(document.name, document.system_identifier,
                                  document.public_identifier, 0);

    buildChilds(parser, document.children);
}

result_t XmlParser::parseHtml(XmlDocument *doc, exlib::string source)
{
    XmlParser parser(doc, false);

    parser.m_now = doc;
    parser.m_list.push_back(doc);

    GumboOutput *output = gumbo_parse(source.c_str());
    buildDocument(parser, output->document->v.document);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return 0;
}

} /* namespace fibjs */
