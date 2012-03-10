//  Copyright (C) 2008, 2009, 2010, 2011 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_STACK_HPP_INCLUDED
#define BOOST_LOCKFREE_STACK_HPP_INCLUDED

#include <boost/checked_delete.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <boost/lockfree/detail/atomic.hpp>
#include <boost/lockfree/detail/freelist.hpp>
#include <boost/lockfree/detail/tagged_ptr.hpp>


namespace boost {
namespace lockfree {

/** The stack class provides a multi-writer/multi-reader stack, pushing and poping is lockfree,
 *  construction/destruction has to be synchronized. It uses a freelist for memory management,
 *  freed nodes are pushed to the freelist and not returned to the os before the stack is destroyed.
 *
 *  The memory management of the stack can be controlled via its freelist_t template argument. Two different
 *  freelists can be used. struct caching_freelist_t selects a caching freelist, which can allocate more nodes
 *  from the operating system, and struct static_freelist_t uses a fixed-sized freelist. With a fixed-sized
 *  freelist, the push operation may fail, while with a caching freelist, the push operation may block.
 *
 *  \b Limitation: The class T is required to have a trivial assignment operator.
 * */
template <typename T,
          typename freelist_t = caching_freelist_t,
          typename Alloc = std::allocator<T>
          >
class stack:
    boost::noncopyable
{
private:
    BOOST_STATIC_ASSERT(boost::has_trivial_assign<T>::value);

#ifndef BOOST_DOXYGEN_INVOKED
    struct node
    {
        typedef detail::tagged_ptr<node> tagged_node_ptr;

        node(T const & v):
            v(v)
        {}

        tagged_node_ptr next;
        T v;
    };
#endif

    typedef detail::tagged_ptr<node> tagged_node_ptr;

    typedef typename Alloc::template rebind<node>::other node_allocator;

    typedef typename boost::mpl::if_<boost::is_same<freelist_t, caching_freelist_t>,
                                     detail::freelist_stack<node, true, node_allocator>,
                                     detail::freelist_stack<node, false, node_allocator>
                                     >::type pool_t;

public:
    /**
     * \return true, if implementation is lock-free.
     * */
    bool is_lock_free (void) const
    {
        return tos.is_lock_free() && pool.is_lock_free();
    }

    //! Construct stack.
    stack(void):
        tos(tagged_node_ptr(NULL, 0))
    {}

    //! Construct stack, allocate n nodes for the freelist
    explicit stack(std::size_t n):
        tos(tagged_node_ptr(NULL, 0))
    {
        pool.reserve_unsafe(n);
    }

    //! Allocate n nodes for freelist
    void reserve(std::size_t n)
    {
        pool.reserve(n);
    }

    /** \copydoc boost::lockfree::stack::reserve
     *
     *  \note not thread-safe
     *
     * */
    void reserve_unsafe(std::size_t n)
    {
        pool.reserve_unsafe(n);
    }

    /** Destroys stack, free all nodes from freelist.
     *
     *  \note not thread-safe
     *
     * */
    ~stack(void)
    {
        if (!empty()) {
            T dummy;
            while(pop_unsafe(dummy))
                ;
        }
    }

    /** Pushes object t to the fifo. May fail, if the freelist is not able to allocate a new fifo node.
     *
     * \returns true, if the push operation is successful.
     *
     * \note Thread-safe and non-blocking
     * \warning \b Warning: May block if node needs to be allocated from the operating system
     * */
    bool push(T const & v)
    {
        node * newnode = pool.construct(v);

        if (newnode == 0)
            return false;

        tagged_node_ptr old_tos = tos.load(detail::memory_order_relaxed);

        for (;;) {
            tagged_node_ptr new_tos (newnode, old_tos.get_tag());
            newnode->next.set_ptr(old_tos.get_ptr());

            if (tos.compare_exchange_weak(old_tos, new_tos))
                return true;
        }
    }

    /** Pushes object t to the fifo. May fail, if the freelist is not able to allocate a new fifo node.
     *
     * \returns true, if the push operation is successful.
     *
     * \note Not thread-safe
     * \warning \b Warning: May block if node needs to be allocated from the operating system
     * */
    bool push_unsafe(T const & v)
    {
        node * newnode = pool.construct_unsafe(v);

        if (newnode == 0)
            return false;

        tagged_node_ptr old_tos = tos.load(detail::memory_order_relaxed);

        tagged_node_ptr new_tos (newnode, old_tos.get_tag());
        newnode->next.set_ptr(old_tos.get_ptr());

        tos.store(new_tos, memory_order_relaxed);
        return true;
    }


    /** Pops object from stack.
     *
     * If pop operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the pop operation is successful, false if stack was empty.
     *
     * \note Thread-safe and non-blocking
     *
     * */
    bool pop(T & ret)
    {
        tagged_node_ptr old_tos = tos.load(detail::memory_order_consume);

        for (;;) {
            if (!old_tos.get_ptr())
                return false;

            node * new_tos_ptr = old_tos->next.get_ptr();
            tagged_node_ptr new_tos(new_tos_ptr, old_tos.get_tag() + 1);

            if (tos.compare_exchange_weak(old_tos, new_tos)) {
                ret = old_tos->v;
                pool.destruct(old_tos.get_ptr());
                return true;
            }
        }
    }

    /** Pops object from stack.
     *
     * If pop operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the pop operation is successful, false if stack was empty.
     *
     * \note Not thread-safe
     *
     * */
    bool pop_unsafe(T & ret)
    {
        tagged_node_ptr old_tos = tos.load(detail::memory_order_relaxed);

        if (!old_tos.get_ptr())
            return false;

        node * new_tos_ptr = old_tos->next.get_ptr();
        tagged_node_ptr new_tos(new_tos_ptr, old_tos.get_tag() + 1);

        tos.store(new_tos, memory_order_relaxed);
        ret = old_tos->v;
        pool.destruct_unsafe(old_tos.get_ptr());
        return true;
    }

    /**
     * \return true, if stack is empty.
     *
     * \warning The state of the stack can be modified by other threads
     *
     * \note While this function is thread-safe, it only guarantees that at some point during the execution of the function the
     *       stack has been empty
     * */
    bool empty(void) const
    {
        return tos.load().get_ptr() == NULL;
    }

private:
#ifndef BOOST_DOXYGEN_INVOKED
    detail::atomic<tagged_node_ptr> tos;

    static const int padding_size = BOOST_LOCKFREE_CACHELINE_BYTES - sizeof(tagged_node_ptr);
    char padding[padding_size];

    pool_t pool;
#endif
};

} /* namespace lockfree */
} /* namespace boost */

#endif /* BOOST_LOCKFREE_STACK_HPP_INCLUDED */
