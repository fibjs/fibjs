/*
 * XmlNodeImpl.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "XmlNodeImpl.h"
#include "XmlDocument.h"
#include "XmlDocumentType.h"
#include "XmlDocumentFragment.h"
#include "XmlElement.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"

namespace fibjs {

const char* XmlNodeImpl::s_nss[][2] = {
    { "xml", "http://www.w3.org/XML/1998/namespace" },
    { "xmlns", "http://www.w3.org/2000/xmlns/" },
    { NULL, NULL }
};

XmlNodeImpl* XmlNodeImpl::fromNode(XmlNode_base* pNode)
{
    int32_t type;

    pNode->get_nodeType(type);

    switch (type) {
    case xml_base::C_ELEMENT_NODE:
        return (XmlElement*)pNode;
    case xml_base::C_TEXT_NODE:
        return (XmlText*)pNode;
    case xml_base::C_CDATA_SECTION_NODE:
        return (XmlCDATASection*)pNode;
    case xml_base::C_PROCESSING_INSTRUCTION_NODE:
        return (XmlProcessingInstruction*)pNode;
    case xml_base::C_COMMENT_NODE:
        return (XmlComment*)pNode;
    case xml_base::C_DOCUMENT_NODE:
        return (XmlDocument*)pNode;
    case xml_base::C_DOCUMENT_TYPE_NODE:
        return (XmlDocumentType*)pNode;
    case xml_base::C_DOCUMENT_FRAGMENT_NODE:
        return (XmlDocumentFragment*)pNode;
    }

    return NULL;
}

XmlDocument* XmlNodeImpl::document()
{
    // m_document is a weak_ptr: it yields NULL once the document is gone.
    // XmlDocument is the only XmlDocument_base implementation in this module
    // (same assumption as m_element.As<XmlElement>() in XmlDocument.cpp).
    XmlDocument_base* doc = m_document;

    return doc ? static_cast<XmlDocument*>(doc) : NULL;
}

void XmlNodeImpl::bumpQueryEpoch()
{
    XmlDocument* doc = document();

    if (doc)
        doc->bumpQueryEpoch();
}

// Serialize a node (an element subtree, or a single leaf node) without
// recursion, through the walker in XmlTreeWalk.h.
//
// Element nesting was the only unbounded recursion in the serializer: the
// previous form called child->toString() from XmlElement::toString(), i.e. 2+
// C++ frames per tree level, and a 1200 level document killed the process with
// SIGBUS instead of raising a JS error.  The walker keeps the exact same
// output -- a node is opened before its children, children in vector order,
// closed after them -- with an explicit stack of one frame per level.
//
// xmlMode selects the XML rules of XmlElement::toXmlString() over the legacy
// form of XmlElement::toString(); the two differ only in how a childless HTML
// element is closed.
void XmlNodeImpl::serializeTo(exlib::string& retVal, bool xmlMode)
{
    // Leaf nodes write their markup with an assignment (retVal = ...), not an
    // append, so they must not be handed the accumulator directly.
    exlib::string strChild;

    walkTree(this, true, WALK_ALL_NODES,
        [&](XmlNodeImpl* node) -> int32_t {
            if (node->m_type != xml_base::C_ELEMENT_NODE) {
                // Every non-element type writes its complete markup in
                // toString() and has no children to descend into.  A document
                // or a document fragment forwards to its child list, which
                // re-enters this serializer per child.
                strChild.clear();
                node->m_node->toString(strChild);
                retVal.append(strChild);

                return WALK_SKIP;
            }

            XmlElement* el = (XmlElement*)node->m_node;

            el->writeOpen(retVal, xmlMode);

            // writeOpen() already closed a childless element, so only elements
            // with children need the closing tag from the leave callback.
            return node->m_childs->hasChildNodes() ? WALK_CONTINUE : WALK_SKIP;
        },
        [&](XmlNodeImpl* node) {
            ((XmlElement*)node->m_node)->writeClose(retVal);
        });
}
}
