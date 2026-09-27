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
#include "XmlTreeWalk.h"

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif

#include <gumbo/include/gumbo.h>

namespace fibjs {

inline void buildText(XmlParser& parser, GumboText& text)
{
    parser.OnCharacterData(text.text, (int32_t)qstrlen(text.text));
}

inline void buildCData(XmlParser& parser, GumboText& text)
{
    parser.OnStartCdataSection();
    parser.OnCharacterData(text.text, (int32_t)qstrlen(text.text));
    parser.OnEndCdataSection();
}

inline void buildCommen(XmlParser& parser, GumboText& text)
{
    // fix gumbo cdata parse error
    if (!qstricmp(text.original_text.data, "<![CDATA[", 9)) {
        int32_t len = (int32_t)qstrlen(text.text) - 1;

        if (text.text[len - 1] == ']') {
            len--;
            if (text.text[len - 1] == ']')
                len--;
        }

        parser.OnStartCdataSection();
        parser.OnCharacterData(text.text + 7, (int32_t)qstrlen(text.text) - 7);
        parser.OnEndCdataSection();
    } else
        parser.OnComment(text.text);
}

// Children of a gumbo node, in the shape the walker takes (a contiguous array
// of pointers).  Only documents and elements have children.
inline NodeSeq<GumboNode> gumboChilds(GumboNode* node)
{
    GumboVector* childs = NULL;

    if (node->type == GUMBO_NODE_DOCUMENT)
        childs = &node->v.document.children;
    else if (node->type == GUMBO_NODE_ELEMENT)
        childs = &node->v.element.children;

    if (!childs || !childs->data)
        return NodeSeq<GumboNode> { NULL, 0 };

    return NodeSeq<GumboNode> { (GumboNode* const*)childs->data, childs->length };
}

// Convert one gumbo node and its subtree into DOM nodes.
//
// This used to recurse (buildElement -> buildChilds -> buildElement), i.e. two
// C++ frames per HTML nesting level: a document nested 20000 levels deep killed
// the process with SIGBUS instead of raising a JS error.  It goes through the
// core walker of XmlTreeWalk.h now -- the tree agnostic half, since the source
// is a gumbo tree and not an XmlNodeImpl one.  Element start/end map onto the
// enter/leave callbacks, so the nesting is unwound by the walker's stack.
inline void buildNode(XmlParser& parser, GumboNode* node)
{
    walkTreeSeq<GumboNode>(node, true, gumboChilds,
        [&](GumboNode* n) -> int32_t {
            switch (n->type) {
            case GUMBO_NODE_ELEMENT: {
                const char* tag = gumbo_normalized_tagname(n->v.element.tag);
                std::vector<const char*> attrs;
                int32_t i;

                attrs.resize(n->v.element.attributes.length * 2 + 2);
                for (i = 0; i < (int32_t)n->v.element.attributes.length; i++) {
                    GumboAttribute* attr = (GumboAttribute*)n->v.element.attributes.data[i];
                    attrs[i * 2] = attr->name;
                    attrs[i * 2 + 1] = attr->value;
                }

                attrs[i * 2] = NULL;
                attrs[i * 2 + 1] = NULL;

                parser.OnStartElement(tag, attrs.data());

                return parser.aborted() ? WALK_STOP : WALK_CONTINUE;
            }
            case GUMBO_NODE_TEXT:
            case GUMBO_NODE_WHITESPACE:
                buildText(parser, n->v.text);
                break;
            case GUMBO_NODE_CDATA:
                buildCData(parser, n->v.text);
                break;
            case GUMBO_NODE_COMMENT:
                buildCommen(parser, n->v.text);
                break;
            default:
                break;
            }

            return WALK_SKIP;
        },
        [&](GumboNode* n) {
            parser.OnEndElement(gumbo_normalized_tagname(n->v.element.tag));
        });
}

inline void buildDocument(XmlParser& parser, GumboDocument& document)
{
    if (document.has_doctype)
        parser.OnStartDoctypeDecl(document.name, document.system_identifier,
            document.public_identifier, 0);

    GumboNode** nodes = (GumboNode**)document.children.data;

    for (int32_t i = 0; i < (int32_t)document.children.length; i++)
        buildNode(parser, nodes[i]);
}

result_t XmlParser::parseHtml(XmlDocument* doc, exlib::string source, const XmlParseLimits& limits)
{
    XmlParser parser(doc, false, limits);
    GumboOptions options = kGumboDefaultOptions;

    parser.m_now = doc;
    parser.m_list.push_back(doc);

    options.max_tree_depth = limits.max_element_depth > 0 ? limits.max_element_depth : 0;
    options.max_tree_nodes = limits.max_node_count > 0 ? (size_t)limits.max_node_count : 0;

    GumboOutput* output = gumbo_parse_with_options(&options, source.c_str(), source.length());
    if (!output || !output->document) {
        if (output)
            gumbo_destroy_output(&options, output);
        return CHECK_ERROR(Runtime::setError("XmlParser: unable to create gumbo parse tree."));
    }

    if (output->status == GUMBO_STATUS_DEPTH_LIMIT) {
        gumbo_destroy_output(&options, output);
        return CHECK_ERROR(Runtime::setError("XmlParser: document is nested too deeply, maxElementDepth is %d", limits.max_element_depth));
    }

    if (output->status == GUMBO_STATUS_NODE_LIMIT) {
        gumbo_destroy_output(&options, output);
        return CHECK_ERROR(Runtime::setError("XmlParser: too many nodes, maxNodeCount is %lld", (long long)limits.max_node_count));
    }

    // The tree builder stops descending as soon as a limit trips, but gumbo has
    // already parsed the whole source: the limits bound the DOM that is built,
    // not the parse buffer.
    buildDocument(parser, output->document->v.document);
    gumbo_destroy_output(&options, output);

    if (parser.m_aborted)
        return CHECK_ERROR(Runtime::setError(parser.m_abort_msg));

    return 0;
}

} /* namespace fibjs */
