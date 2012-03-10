//  lock-free fifo queue from
//  Michael, M. M. and Scott, M. L.,
//  "simple, fast and practical non-blocking and blocking concurrent queue algorithms"
//
//  implementation for c++
//
//  Copyright (C) 2008, 2009, 2010, 2011 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_FIFO_HPP_INCLUDED
#define BOOST_LOCKFREE_FIFO_HPP_INCLUDED

#include <memory>               /* std::auto_ptr */

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>

#include <boost/lockfree/detail/atomic.hpp>
#include <boost/lockfree/detail/tagged_ptr.hpp>
#include <boost/lockfree/detail/freelist.hpp>

namespace boost {
namespace lockfree {
namespace detail {

template <typename T, typename freelist_t, typename Alloc>
class fifo:
    boost::noncopyable
{
private:
#ifndef BOOST_DOXYGEN_INVOKED
    BOOST_STATIC_ASSERT(boost::is_pod<T>::value);

    struct BOOST_LOCKFREE_CACHELINE_ALIGNMENT node
    {
        typedef tagged_ptr<node> tagged_node_ptr;

        node(T const & v):
            data(v)
        {
            /* increment tag to avoid ABA problem */
            tagged_node_ptr old_next = next.load(memory_order_relaxed);
            tagged_node_ptr new_next (NULL, old_next.get_tag()+1);
            next.store(new_next, memory_order_release);
        }

        node (void):
            next(tagged_node_ptr(NULL, 0))
        {}

        atomic<tagged_node_ptr> next;
        T data;
    };

    typedef tagged_ptr<node> tagged_node_ptr;

    typedef typename Alloc::template rebind<node>::other node_allocator;

    typedef typename boost::mpl::if_<boost::is_same<freelist_t, caching_freelist_t>,
                                     detail::freelist_stack<node, true, node_allocator>,
                                     detail::freelist_stack<node, false, node_allocator>
                                     >::type pool_t;

    void initialize(void)
    {
        node * n = pool.construct();
        tagged_node_ptr dummy_node(n, 0);
        head_.store(dummy_node, memory_order_relaxed);
        tail_.store(dummy_node, memory_order_release);
    }
#endif

public:
    /**
     * \return true, if implementation is lock-free.
     *
     * \warning \b Warning: It only checks, if the fifo head node is lockfree. On most platforms, the whole implementation is
     *                      lockfree, if this is true. Using c++0x-style atomics, there is no possibility to provide a completely
     *                      accurate implementation, because one would need to test every internal node, which is impossible
     *                      if further nodes will be allocated from the operating system.
     * */
    bool is_lock_free (void) const
    {
        return head_.is_lock_free() && pool.is_lock_free();
    }

    //! Construct fifo.
    fifo(void)
    {
        pool.reserve_unsafe(1);
        initialize();
    }

    //! Construct fifo, allocate n nodes for the freelist.
    explicit fifo(std::size_t n)
    {
        pool.reserve_unsafe(n+1);
        initialize();
    }

    //! \copydoc boost::lockfree::stack::reserve
    void reserve(std::size_t n)
    {
        pool.reserve(n);
    }

    //! \copydoc boost::lockfree::stack::reserve_unsafe
    void reserve_unsafe(std::size_t n)
    {
        pool.reserve_unsafe(n);
    }

    /** Destroys fifo, free all nodes from freelist.
     * */
    ~fifo(void)
    {
        if (!empty()) {
            T dummy;
            while(dequeue_unsafe(dummy))
                ;
        }
        pool.destruct(head_.load(memory_order_relaxed).get_ptr());
    }

    /** Check if the ringbuffer is empty
     *
     * \warning Not thread-safe, use for debugging purposes only
     * */
    bool empty(void)
    {
        return head_.load().get_ptr() == tail_.load().get_ptr();
    }

    /** Enqueues object t to the fifo. Enqueueing may fail, if the freelist is not able to allocate a new fifo node.
     *
     * \returns true, if the enqueue operation is successful.
     *
     * \note Thread-safe and non-blocking
     * \warning \b Warning: May block if node needs to be allocated from the operating system
     * */
    bool enqueue(T const & t)
    {
        node * n = pool.construct(t);

        if (n == NULL)
            return false;

        for (;;) {
            tagged_node_ptr tail = tail_.load(memory_order_acquire);
            tagged_node_ptr next = tail->next.load(memory_order_acquire);
            node * next_ptr = next.get_ptr();

            tagged_node_ptr tail2 = tail_.load(memory_order_acquire);
            if (likely(tail == tail2)) {
                if (next_ptr == 0) {
                    if ( tail->next.compare_exchange_weak(next, tagged_node_ptr(n, next.get_tag() + 1)) ) {
                        tail_.compare_exchange_strong(tail, tagged_node_ptr(n, tail.get_tag() + 1));
                        return true;
                    }
                }
                else
                    tail_.compare_exchange_strong(tail, tagged_node_ptr(next_ptr, tail.get_tag() + 1));
            }
        }
    }

    /** Enqueues object t to the fifo. Enqueueing may fail, if the freelist is not able to allocate a new fifo node.
     *
     * \returns true, if the enqueue operation is successful.
     *
     * \note Not thread-safe
     * \warning \b Warning: May block if node needs to be allocated from the operating system
     * */
    bool enqueue_unsafe(T const & t)
    {
        node * n = pool.construct_unsafe(t);

        if (n == NULL)
            return false;

        for (;;)
        {
            tagged_node_ptr tail = tail_.load(memory_order_relaxed);
            tagged_node_ptr next = tail->next.load(memory_order_relaxed);
            node * next_ptr = next.get_ptr();

            if (next_ptr == 0) {
                tail->next.store(tagged_node_ptr(n, next.get_tag() + 1), memory_order_relaxed);
                tail_.store(tagged_node_ptr(n, tail.get_tag() + 1), memory_order_relaxed);
                return true;
            }
            else
                tail_.store(tagged_node_ptr(next_ptr, tail.get_tag() + 1), memory_order_relaxed);
        }
    }

    /** Dequeue object from fifo.
     *
     * if dequeue operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the dequeue operation is successful, false if fifo was empty.
     *
     * \note Thread-safe and non-blocking
     *
     * */
    bool dequeue (T & ret)
    {
        for (;;) {
            tagged_node_ptr head = head_.load(memory_order_acquire);
            tagged_node_ptr tail = tail_.load(memory_order_acquire);
            tagged_node_ptr next = head->next.load(memory_order_acquire);
            node * next_ptr = next.get_ptr();

            tagged_node_ptr head2 = head_.load(memory_order_acquire);
            if (likely(head == head2)) {
                if (head.get_ptr() == tail.get_ptr()) {
                    if (next_ptr == 0)
                        return false;
                    tail_.compare_exchange_strong(tail, tagged_node_ptr(next_ptr, tail.get_tag() + 1));
                } else {
                    if (next_ptr == 0)
                        /* this check is not part of the original algorithm as published by michael and scott
                         *
                         * however we reuse the tagged_ptr part for the and clear the next part during node
                         * allocation. we can observe a null-pointer here.
                         * */
                        continue;
                    ret = next_ptr->data;
                    if (head_.compare_exchange_weak(head, tagged_node_ptr(next_ptr, head.get_tag() + 1))) {
                        pool.destruct(head.get_ptr());
                        return true;
                    }
                }
            }
        }
    }

    /** Dequeue object from fifo.
     *
     * if dequeue operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the dequeue operation is successful, false if fifo was empty.
     *
     * \note Not thread-safe
     *
     * */
    bool dequeue_unsafe (T & ret)
    {
        for (;;) {
            tagged_node_ptr head = head_.load(memory_order_relaxed);
            tagged_node_ptr tail = tail_.load(memory_order_relaxed);
            tagged_node_ptr next = head->next.load(memory_order_relaxed);
            node * next_ptr = next.get_ptr();

            tagged_node_ptr head2 = head_.load(memory_order_relaxed);
            if (head.get_ptr() == tail.get_ptr()) {
                if (next_ptr == 0)
                    return false;
                tail_.store(tagged_node_ptr(next_ptr, tail.get_tag() + 1), memory_order_relaxed);
            } else {
                if (next_ptr == 0)
                    /* this check is not part of the original algorithm as published by michael and scott
                     *
                     * however we reuse the tagged_ptr part for the and clear the next part during node
                     * allocation. we can observe a null-pointer here.
                     * */
                    continue;
                ret = next_ptr->data;
                head_.store(tagged_node_ptr(next_ptr, head.get_tag() + 1), memory_order_relaxed);
                pool.destruct_unsafe(head.get_ptr());
                return true;
            }
        }
    }


private:
#ifndef BOOST_DOXYGEN_INVOKED
    atomic<tagged_node_ptr> head_;
    static const int padding_size = BOOST_LOCKFREE_CACHELINE_BYTES - sizeof(tagged_node_ptr);
    char padding1[padding_size];
    atomic<tagged_node_ptr> tail_;
    char padding2[padding_size];

    pool_t pool;
#endif
};

} /* namespace detail */

/** The fifo class provides a multi-writer/multi-reader fifo, enqueueing and dequeueing is lockfree,
 *  construction/destruction has to be synchronized. It uses a freelist for memory management,
 *  freed nodes are pushed to the freelist and not returned to the os before the fifo is destroyed.
 *
 *  The memory management of the fifo can be controlled via its freelist_t template argument. Two different
 *  freelists can be used. struct caching_freelist_t selects a caching freelist, which can allocate more nodes
 *  from the operating system, and struct static_freelist_t uses a fixed-sized freelist. With a fixed-sized
 *  freelist, the enqueue operation may fail, while with a caching freelist, the enqueue operation may block.
 *
 *  \b Limitation: The class T is required to have a trivial assignment operator.
 *
 * */
template <typename T,
          typename freelist_t = caching_freelist_t,
          typename Alloc = std::allocator<T>
         >
class fifo:
    public detail::fifo<T, freelist_t, Alloc>
{
    BOOST_STATIC_ASSERT(boost::has_trivial_assign<T>::value);

public:
    //! Construct fifo.
    fifo(void)
    {}

    //! Construct fifo, allocate n nodes for the freelist.
    explicit fifo(std::size_t n):
        detail::fifo<T, freelist_t, Alloc>(n)
    {}
};


/** Template specialization of the fifo class for pointer arguments, that supports dequeue operations to
 *  stl/boost-style smart pointers
 *
 * */
template <typename T,
          typename freelist_t,
          typename Alloc
         >
class fifo<T*, freelist_t, Alloc>:
    public detail::fifo<T*, freelist_t, Alloc>
{
#ifndef BOOST_DOXYGEN_INVOKED
    typedef detail::fifo<T*, freelist_t, Alloc> fifo_t;

    template <typename smart_ptr>
    bool dequeue_smart_ptr(smart_ptr & ptr)
    {
        T * result = 0;
        bool success = fifo_t::dequeue(result);

        if (success)
            ptr.reset(result);
        return success;
    }
#endif

public:
    //! Construct fifo.
    fifo(void)
    {}

    //! Construct fifo, allocate n nodes for the freelist.
    explicit fifo(std::size_t n):
        fifo_t(n)
    {}

    //! \copydoc detail::fifo::dequeue
    bool dequeue (T * & ret)
    {
        return fifo_t::dequeue(ret);
    }

    /** Dequeue object from fifo to std::auto_ptr
     *
     * if dequeue operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the dequeue operation is successful, false if fifo was empty.
     *
     * \note Thread-safe and non-blocking
     *
     * */
    bool dequeue (std::auto_ptr<T> & ret)
    {
        return dequeue_smart_ptr(ret);
    }

    /** Dequeue object from fifo to boost::scoped_ptr
     *
     * if dequeue operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the dequeue operation is successful, false if fifo was empty.
     *
     * \note Thread-safe and non-blocking
     *
     * */
    bool dequeue (boost::scoped_ptr<T> & ret)
    {
        BOOST_STATIC_ASSERT(sizeof(boost::scoped_ptr<T>) == sizeof(T*));
        return dequeue(reinterpret_cast<T*&>(ret));
    }

    /** Dequeue object from fifo to boost::shared_ptr
     *
     * if dequeue operation is successful, object is written to memory location denoted by ret.
     *
     * \returns true, if the dequeue operation is successful, false if fifo was empty.
     *
     * \note Thread-safe and non-blocking
     *
     * */
    bool dequeue (boost::shared_ptr<T> & ret)
    {
        return dequeue_smart_ptr(ret);
    }
};

} /* namespace lockfree */
} /* namespace boost */

#endif /* BOOST_LOCKFREE_FIFO_HPP_INCLUDED */
