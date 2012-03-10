//  lock-free freelist
//
//  Copyright (C) 2008, 2009, 2011 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_FREELIST_HPP_INCLUDED
#define BOOST_LOCKFREE_FREELIST_HPP_INCLUDED

#include <boost/lockfree/detail/tagged_ptr.hpp>

#include <boost/lockfree/detail/atomic.hpp>
#include <boost/noncopyable.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_pod.hpp>

#include <algorithm>            /* for std::min */

namespace boost
{
namespace lockfree
{
namespace detail
{

struct freelist_node
{
    tagged_ptr<freelist_node> next;
};

template <typename T,
          bool allocate_may_allocate,
          typename Alloc = std::allocator<T>
         >
class freelist_stack:
    Alloc
{
    typedef tagged_ptr<freelist_node> tagged_node_ptr;

public:
    freelist_stack (std::size_t n = 0):
        pool_(tagged_node_ptr(NULL))
    {
        reserve_unsafe(n);
    }

    void reserve (std::size_t count)
    {
        for (std::size_t i = 0; i != count; ++i) {
            T * node = Alloc::allocate(1);
            deallocate(node);
        }
    }

    void reserve_unsafe (std::size_t count)
    {
        for (std::size_t i = 0; i != count; ++i) {
            T * node = Alloc::allocate(1);
            deallocate_unsafe(node);
        }
    }

    T * construct (void)
    {
        T * node = allocate();
        if (node)
            new(node) T();
        return node;
    }

    template <typename ArgumentType>
    T * construct (ArgumentType const & arg)
    {
        T * node = allocate();
        if (node)
            new(node) T(arg);
        return node;
    }

    T * construct_unsafe (void)
    {
        T * node = allocate_unsafe();
        if (node)
            new(node) T();
        return node;
    }

    template <typename ArgumentType>
    T * construct_unsafe (ArgumentType const & arg)
    {
        T * node = allocate_unsafe();
        if (node)
            new(node) T(arg);
        return node;
    }


    void destruct (T * n)
    {
        n->~T();
        deallocate(n);
    }

    void destruct_unsafe (T * n)
    {
        n->~T();
        deallocate_unsafe(n);
    }

    T * allocate (void)
    {
        tagged_node_ptr old_pool = pool_.load(memory_order_consume);

        for(;;) {
            if (!old_pool.get_ptr()) {
                if (allocate_may_allocate)
                    return Alloc::allocate(1);
                else
                    return 0;
            }

            freelist_node * new_pool_ptr = old_pool->next.get_ptr();
            tagged_node_ptr new_pool (new_pool_ptr, old_pool.get_tag() + 1);

            if (pool_.compare_exchange_weak(old_pool, new_pool)) {
                void * ptr = old_pool.get_ptr();
                return reinterpret_cast<T*>(ptr);
            }
        }
    }

    T * allocate_unsafe (void)
    {
        tagged_node_ptr old_pool = pool_.load(memory_order_relaxed);

        if (!old_pool.get_ptr()) {
            if (allocate_may_allocate)
                return Alloc::allocate(1);
            else
                return 0;
        }

        freelist_node * new_pool_ptr = old_pool->next.get_ptr();
        tagged_node_ptr new_pool (new_pool_ptr, old_pool.get_tag() + 1);

        pool_.store(new_pool, memory_order_relaxed);
        void * ptr = old_pool.get_ptr();
        return reinterpret_cast<T*>(ptr);
    }

    void deallocate (T * n)
    {
        void * node = n;
        tagged_node_ptr old_pool = pool_.load(memory_order_consume);
        freelist_node * new_pool_ptr = reinterpret_cast<freelist_node*>(node);

        for(;;) {
            tagged_node_ptr new_pool (new_pool_ptr, old_pool.get_tag());
            new_pool->next.set_ptr(old_pool.get_ptr());

            if (pool_.compare_exchange_weak(old_pool, new_pool))
                return;
        }
    }

    void deallocate_unsafe (T * n)
    {
        void * node = n;
        tagged_node_ptr old_pool = pool_.load(memory_order_relaxed);
        freelist_node * new_pool_ptr = reinterpret_cast<freelist_node*>(node);

        tagged_node_ptr new_pool (new_pool_ptr, old_pool.get_tag());
        new_pool->next.set_ptr(old_pool.get_ptr());

        pool_.store(new_pool, memory_order_relaxed);
    }

    ~freelist_stack(void)
    {
        tagged_node_ptr current (pool_);

        while (current) {
            freelist_node * current_ptr = current.get_ptr();
            if (current_ptr)
                current = current_ptr->next;
            Alloc::deallocate((T*)current_ptr, 1);
        }
    }

    bool is_lock_free(void) const
    {
        return pool_.is_lock_free();
    }

private:
    atomic<tagged_node_ptr> pool_;
};


} /* namespace detail */


struct caching_freelist_t {};
struct static_freelist_t {};



} /* namespace lockfree */
} /* namespace boost */

#endif /* BOOST_LOCKFREE_FREELIST_HPP_INCLUDED */
