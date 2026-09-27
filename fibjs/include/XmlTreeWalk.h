/*
 * XmlTreeWalk.h
 *
 * Iterative tree traversal: the single home for "walk a subtree" in the XML
 * module.
 *
 * Why this exists
 * ---------------
 * A recursive traversal costs one or two C++ frames per tree level, so the
 * depth of the document bounds the depth of the native stack.  Deep documents
 * are cheap to build (a few hundred KB of markup) and are routinely produced by
 * generated or hostile input, so every recursive helper in this module was a
 * process kill waiting to happen.  The depths at which a release build died
 * with SIGBUS/SIGSEGV -- no JS exception, no output at all -- were:
 *
 *   ~1200  serializer (XmlElement::toString / toXmlString)
 *   ~2500  element queries (getElementsByTagName, querySelectorAll, ...)
 *   ~2700  parser (namespace lookup per level)
 *   ~4000  cloneNode(true)
 *   ~20000 normalize / isEqualNode / :has() / the HTML tree builder
 *
 * All of those are now iterative.  New code must not reintroduce the pattern:
 *
 *   RULE: if it walks a subtree, it uses walkTree() / walkTreeSeq() below.
 *         A helper that calls itself (or a sibling) once per tree level is a
 *         process kill; `test/xml_test.js` -> "deep documents" walks 20000
 *         levels through every public traversal and will catch it.
 *
 * How the walkers work
 * --------------------
 * The stack holds one frame per *level* (node + cursor into its child array),
 * not one entry per pending node, so:
 *   - a deep document costs O(depth) frames,
 *   - a wide one (millions of siblings) costs nothing extra,
 *   - the walk is document order: a node before its children, children in
 *     vector order, and with leave() a node is closed after its children.
 *
 * Two traversals cannot use these walkers and keep their own explicit stack
 * instead -- both are documented at their definition:
 *   - XmlNodeImpl::isEqualNode() compares two subtrees, i.e. it walks a pair of
 *     trees in lockstep;
 *   - XmlNodeList::cloneChilds() walks a source tree while building a second
 *     one, so each frame carries a source *and* a destination node.
 */

#pragma once

#include "XmlNodeImpl.h"

namespace fibjs {

// What the walker does after enter() returned.
enum {
    // Visit the children of this node, then continue with its next sibling.
    WALK_CONTINUE,
    // Do not visit the children of this node, continue with its next sibling.
    WALK_SKIP,
    // Do not advance: the callback changed or removed the child at the cursor,
    // so the same slot has to be examined again (used by normalize()).
    WALK_REPEAT,
    // End the walk.
    WALK_STOP
};

// Which nodes enter()/leave() see.
enum {
    WALK_ALL_NODES,
    WALK_ELEMENTS
};

// A read-only view over a child sequence: any contiguous array of node
// pointers.  std::vector<XmlNodeImpl*> (the DOM) and GumboVector (the HTML
// tree) both satisfy it, which is what lets the core walker serve both trees.
template <typename NodeT>
struct NodeSeq {
    NodeT* const* data;
    size_t size;
};

// ---------------------------------------------------------------------------
// Core walker (tree agnostic): pre-order over any tree whose nodes expose a
// contiguous child array through `seqOf`.
//
//   seqOf(node)  -> NodeSeq<NodeT>   children of node (empty for leaves)
//   enter(node)  -> one of the WALK_* actions above
//   leave(node)  -> optional, runs after the children of a node whose enter()
//                   returned WALK_CONTINUE (i.e. only for nodes that were
//                   actually descended into)
//   includeSelf  -> call enter() for `root` itself, or only for its descendants
//
// seqOf() is re-evaluated on every step instead of being cached in the frame:
// a callback is allowed to mutate the child array it is looking at (normalize()
// merges and removes text nodes), and a cached pointer would go stale as soon
// as the vector reallocates.  Mutating the list of the node currently being
// entered is safe; mutating an ancestor's list while the walk is inside it is
// not (the cursor would skip or repeat entries).
// ---------------------------------------------------------------------------
template <typename NodeT, typename SeqFn, typename EnterFn, typename LeaveFn>
void walkTreeSeq(NodeT* root, bool includeSelf, SeqFn&& seqOf, EnterFn&& enter, LeaveFn&& leave)
{
    struct Frame {
        NodeT* node;
        size_t index;
        bool close;
    };

    std::vector<Frame> stack;

    if (includeSelf) {
        int32_t action = enter(root);
        if (action != WALK_CONTINUE)
            return;

        stack.push_back(Frame { root, 0, true });
    } else
        stack.push_back(Frame { root, 0, false });

    while (!stack.empty()) {
        Frame& frame = stack.back();
        NodeSeq<NodeT> seq = seqOf(frame.node);

        if (frame.index >= seq.size) {
            if (frame.close)
                leave(frame.node);

            stack.pop_back();
            continue;
        }

        NodeT* child = seq.data[frame.index];
        int32_t action = enter(child);

        if (action == WALK_STOP)
            return;

        if (action == WALK_REPEAT)
            continue;

        frame.index++;

        if (action == WALK_CONTINUE)
            stack.push_back(Frame { child, 0, true });
    }
}

template <typename NodeT, typename SeqFn, typename EnterFn>
void walkTreeSeq(NodeT* root, bool includeSelf, SeqFn&& seqOf, EnterFn&& enter)
{
    walkTreeSeq(root, includeSelf, seqOf, enter, [](NodeT*) {});
}

// ---------------------------------------------------------------------------
// DOM walkers
// ---------------------------------------------------------------------------

// Pre-order walk over the subtree of `root`, with an explicit stack.
template <typename EnterFn, typename LeaveFn>
void walkTree(XmlNodeImpl* root, bool includeSelf, int32_t filter, EnterFn&& enter, LeaveFn&& leave)
{
    walkTreeSeq<XmlNodeImpl>(root, includeSelf,
        [](XmlNodeImpl* node) -> NodeSeq<XmlNodeImpl> {
            std::vector<XmlNodeImpl*>& childs = node->m_childs->m_childs;
            return NodeSeq<XmlNodeImpl> { childs.data(), childs.size() };
        },
        [&](XmlNodeImpl* node) -> int32_t {
            if (filter == WALK_ELEMENTS && node->m_type != xml_base::C_ELEMENT_NODE)
                return WALK_SKIP;

            return enter(node);
        },
        leave);
}

template <typename EnterFn>
void walkTree(XmlNodeImpl* root, bool includeSelf, int32_t filter, EnterFn&& enter)
{
    walkTree(root, includeSelf, filter, enter, [](XmlNodeImpl*) {});
}

// Element-only pre-order walk, the shape the query code uses: fn returns true
// to stop the walk early.
template <typename Fn>
void walkElements(XmlNodeImpl* root, bool includeSelf, Fn&& fn)
{
    walkTree(root, includeSelf, WALK_ELEMENTS, [&](XmlNodeImpl* node) -> int32_t {
        return fn(node) ? WALK_STOP : WALK_CONTINUE;
    });
}

} /* namespace fibjs */
