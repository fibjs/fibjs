/*
 * QuickArray.h
 *
 *  Created on: Jul 30, 2012
 *      Author: lion
 */

#include <stdlib.h>

#ifndef QUICKARRAY_H_
#define QUICKARRAY_H_

namespace fibjs
{

template<typename T>
class QuickArray
{
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
        if (n > 0)
        {
            size_t c1, c2;
            size_t i, s;

            s = m_size + n;

            c1 = (s + BlockSize() - 1) / BlockSize();
            c2 = (m_size + BlockSize() - 1) / BlockSize();

            if (c1 > c2)
            {
                if (c2 > 0)
                    m_p = (T **) realloc(m_p, sizeof(T *) * c1);
                else
                    m_p = (T **) malloc(sizeof(T *) * c1);

                for (i = c2; i < c1; i++)
                    m_p[i] = (T *) malloc(sizeof(T) * BlockSize());
            }
        }
    }

public:
    QuickArray()
    {
        m_size = 0;
        m_p = 0;
    }

    QuickArray(const QuickArray<T> &rhs)
    {
        m_size = 0;
        m_p = 0;
        append(rhs);
    }

    ~QuickArray()
    {
        resize(0);
    }

    QuickArray<T> &operator=(const QuickArray<T> &rhs)
    {
        resize(0);
        append(rhs);

        return *this;
    }

    size_t resize(size_t s)
    {
        if (s < m_size)
        {
            size_t c1, c2;
            size_t i;

            c1 = (s + BlockSize() - 1) / BlockSize();
            c2 = (m_size + BlockSize() - 1) / BlockSize();

            size_t p1 = s / BlockSize(), p2 = s % BlockSize();
            T *ptr = m_p[p1];

            for (i = s; i < m_size; i++)
            {
                ptr[p2].~T();
                if (i < m_size - 1 && ++p2 == BlockSize())
                {
                    p1++;
                    p2 = 0;
                    ptr = m_p[p1];
                }
            }

            if (c1 < c2)
            {
                for (i = c1; i < c2; i++)
                    free(m_p[i]);

                if (c1 > 0)
                    m_p = (T **) realloc(m_p, sizeof(T *) * c1);
                else
                {
                    free(m_p);
                    m_p = NULL;
                }
            }
        }
        else if (s > m_size)
        {
            _grow(s - m_size);

            size_t p1 = m_size / BlockSize(), p2 = m_size % BlockSize();
            T *ptr = m_p[p1];

            for (size_t i = m_size; i < s; i++)
            {
                new (&ptr[p2]) T();
                if (i < s - 1 && ++p2 == BlockSize())
                {
                    p1++;
                    p2 = 0;
                    ptr = m_p[p1];
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

    T &operator[](size_t i)
    {
        if (i >= m_size)
            resize(i + 1);

        return m_p[i / BlockSize()][i % BlockSize()];
    }

    void pop()
    {
        resize(m_size - 1);
    }

    T &alloc()
    {
        return operator[](m_size);
    }

    template<typename V>
    void append(const V *rhs, size_t n)
    {
        if (n > 0)
        {
            size_t s = m_size + n;
            _grow(n);

            size_t pos = 0, p1 = m_size / BlockSize(), p2 = m_size
                                 % BlockSize();
            T *ptr = m_p[p1];
            for (size_t i = m_size; i < s; i++)
            {
                new (&ptr[p2]) T(rhs[pos]);
                pos++;

                if (i < s - 1 && ++p2 == BlockSize())
                {
                    p1++;
                    p2 = 0;
                    ptr = m_p[p1];
                }
            }

            m_size = s;
        }
    }

    template<typename V>
    void append(const V &t)
    {
        append(&t, 1);
    }

    void append(const QuickArray<T> &rhs, size_t pos = 0, size_t n = -1)
    {
        if (n < 0 || n > rhs.m_size - pos)
            n = rhs.m_size - pos;

        if (n > 0)
        {
            size_t s = m_size + n;
            _grow(n);

            size_t p1 = m_size / BlockSize(), p2 = m_size % BlockSize();
            size_t p11 = pos / BlockSize(), p21 = pos % BlockSize();

            T *ptr = m_p[p1], *ptr1 = rhs.m_p[p11];

            for (size_t i = m_size; i < s; i++)
            {
                new (&ptr[p2]) T(ptr1[p21]);

                if (i < s - 1)
                {
                    if (++p21 == BlockSize())
                    {
                        p11++;
                        p21 = 0;
                        ptr1 = rhs.m_p[p11];
                    }

                    if (++p2 == BlockSize())
                    {
                        p1++;
                        p2 = 0;
                        ptr = m_p[p1];
                    }
                }
            }

            m_size = s;
        }
    }

    void clear()
    {
        resize(0);
    }

private:
    T **m_p;
    size_t m_size;
};

}

#endif /* QUICKARRAY_H_ */
