/*
 * QuickArray.h
 *
 *  Created on: Jul 30, 2012
 *      Author: lion
 */

#pragma once

#include <stdlib.h>

namespace fibjs {

template <typename T>
class QuickArray {
private:
    inline size_t BlockSize() const
    {
        if (sizeof(T) <= 16)
            return 256 / sizeof(T);
        else if (sizeof(T) <= 32)
            return 512 / sizeof(T);
        else if (sizeof(T) <= 64)
            return 1024 / sizeof(T);
        else if (sizeof(T) <= 128)
            return 2048 / sizeof(T);
        else if (sizeof(T) <= 2048)
            return 4096 / sizeof(T);
        else
            return 1;
    }

    inline void _grow(size_t n)
    {
        if (n > 0) {
            size_t c1, c2;
            size_t i, s;

            s = m_size + n;

            c1 = (s + BlockSize() - 1) / BlockSize();
            c2 = m_array.size();

            if (c1 > c2) {
                m_array.resize(c1);
                for (i = c2; i < c1; i++)
                    m_array[i] = (T*)new char[sizeof(T) * BlockSize()];
            }
        }
    }

public:
    QuickArray()
    {
        m_size = 0;
    }

    QuickArray(const QuickArray<T>& rhs)
    {
        m_size = 0;
        append(rhs);
    }

    ~QuickArray()
    {
        resize(0);
    }

    QuickArray<T>& operator=(const QuickArray<T>& rhs)
    {
        resize(0);
        append(rhs);

        return *this;
    }

    size_t resize(size_t s)
    {
        if (s < m_size) {
            size_t c1, c2;
            size_t i;

            c1 = (s + BlockSize() - 1) / BlockSize();
            c2 = m_array.size();

            size_t p1 = s / BlockSize(), p2 = s % BlockSize();
            T* ptr = m_array[p1];

            for (i = s; i < m_size; i++) {
                ptr[p2++].~T();
                if (i < m_size - 1 && p2 == BlockSize()) {
                    p1++;
                    ptr = m_array[p1];
                    p2 = 0;
                }
            }

            if (c1 < c2) {
                for (i = c1; i < c2; i++)
                    delete[]((char*)m_array[i]);
                m_array.resize(c1);
            }
        } else if (s > m_size) {
            _grow(s - m_size);

            size_t p1 = m_size / BlockSize(), p2 = m_size % BlockSize();
            T* ptr = m_array[p1];

            for (size_t i = m_size; i < s; i++) {
                new (&ptr[p2++]) T();
                if (i < s - 1 && p2 == BlockSize()) {
                    p1++;
                    ptr = m_array[p1];
                    p2 = 0;
                }
            }
        }

        m_size = s;

        return m_size;
    }

    size_t size() const
    {
        return m_size;
    }

    T& operator[](size_t i)
    {
        if (i >= m_size)
            resize(i + 1);

        return m_array[i / BlockSize()][i % BlockSize()];
    }

    void pop()
    {
        resize(m_size - 1);
    }

    T& alloc()
    {
        return operator[](m_size);
    }

    void append(const QuickArray<T>& rhs)
    {
        for (size_t i = 0; i < rhs.m_size; i++)
            append(rhs.m_array[i / BlockSize()][i % BlockSize()]);
    }

    template <typename V>
    void append(const V* rhs, size_t n)
    {
        if (n > 0) {
            size_t s = m_size + n;
            _grow(n);

            size_t pos = 0, p1 = m_size / BlockSize(),
                   p2 = m_size % BlockSize();
            T* ptr = m_array[p1];

            for (size_t i = m_size; i < s; i++) {
                new (&ptr[p2++]) T(rhs[pos++]);

                if (i < s - 1 && p2 == BlockSize()) {
                    p1++;
                    ptr = m_array[p1];
                    p2 = 0;
                }
            }

            m_size = s;
        }
    }

    template <typename V>
    void append(const V& t)
    {
        append(&t, 1);
    }

    void clear()
    {
        resize(0);
    }

private:
    std::vector<T*> m_array;
    size_t m_size;
};
}
