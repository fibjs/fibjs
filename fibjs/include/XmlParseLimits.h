/*
 * XmlParseLimits.h
 *
 * Resource limits enforced while parsing xml/html (xml.parse / DOMParser /
 * XmlDocument.load).
 *
 * Why the defaults are not "unlimited": a document is untrusted input, and the
 * DOM it produces is ~700 bytes per node (measured: an empty element with its
 * attribute map), so a few hundred KB of markup can allocate hundreds of MB.
 * The depth limit additionally keeps every operation on the resulting tree
 * inside the range where the native stack is safe -- the deepest traversal in
 * the module is the serializer at ~1200 levels (see XmlTreeWalk.h).
 *
 * A limit of 0 (or a negative value) disables that limit.
 *
 * HTML note: the vendored gumbo parse is already iterative, and its teardown is
 * flattened too (destroy_node in gumbo_parser.c now keeps an explicit stack),
 * so html obeys maxElementDepth like xml does.  A build against an older
 * prebuilt vender dist may still carry the recursive gumbo teardown; the
 * default (1000) stays well below that historical ceiling either way.
 */

#pragma once

#include "utils.h"

namespace fibjs {

enum {
    // xml.parse()'s defaults when the caller passes no options.
    XML_DEFAULT_MAX_ELEMENT_DEPTH = 1000,
    XML_DEFAULT_MAX_NODE_COUNT = 1000000
};

struct XmlParseLimits {
    XmlParseLimits()
        : max_element_depth(XML_DEFAULT_MAX_ELEMENT_DEPTH)
        , max_node_count(XML_DEFAULT_MAX_NODE_COUNT)
    {
    }

    // Maximum element nesting depth; the document element is depth 1.
    int32_t max_element_depth;
    // Maximum number of nodes the parser may create: elements, attributes,
    // text nodes, comments, CDATA sections, processing instructions and the
    // document type.
    int64_t max_node_count;
};

// Read { maxElementDepth, maxNodeCount } from a JS options object.  Missing
// properties keep their default, a non-number is rejected, 0 or a negative
// value disables the limit.  `options` may be empty (no options passed).
result_t getParseLimits(v8::Local<v8::Object> options, XmlParseLimits& limits);

} /* namespace fibjs */
