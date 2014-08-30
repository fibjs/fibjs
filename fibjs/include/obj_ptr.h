/*
 * obj_ptr.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include <exlib/include/utils.h>


#ifndef OBJ_PTR_H_
#define OBJ_PTR_H_

namespace fibjs
{

extern int32_t g_obj_refs;

class obj_base
{
public:
    obj_base() :
        refs_(0)
    {
        exlib::atom_inc(&g_obj_refs);
    }

    virtual ~obj_base()
    {
        exlib::atom_dec(&g_obj_refs);
    }

public:
    virtual void Ref()
    {
        internalRef();
    }

    virtual void Unref()
    {
        if (internalUnref() == 0)
            delete this;
    }

    void dispose()
    {
        if (refs_ == 0)
            delete this;
    }

protected:
    int32_t internalRef()
    {
        return exlib::atom_inc(&refs_);
    }

    int32_t internalUnref()
    {
        return exlib::atom_dec(&refs_);
    }

private:
    volatile int32_t refs_;
};

template<class T>
class obj_ptr
{
public:
    obj_ptr() :
        p(NULL)
    {
    }

    obj_ptr(T *lp) :
        p(NULL)
    {
        operator=(lp);
    }

    obj_ptr(const obj_ptr<T> &lp) :
        p(NULL)
    {
        operator=(lp);
    }

    template<class Q>
    obj_ptr(const obj_ptr<Q> &lp) :
        p(NULL)
    {
        operator=(lp);
    }

    ~obj_ptr()
    {
        Release();
    }

    T *operator=(T *lp)
    {
        if (lp != NULL)
            lp->Ref();

        return _attach(lp);
    }

    template<class Q>
    T *operator=(Q *lp)
    {
        if (lp != NULL)
            lp->Ref();

        return _attach(lp);
    }

    T *operator=(const obj_ptr<T> &lp)
    {
        return operator=((T *)lp);
    }

    template<class Q>
    T *operator=(const obj_ptr<Q> &lp)
    {
        return operator=((Q *)lp);
    }

    operator T *() const
    {
        return p;
    }

    T &operator*() const
    {
        return *p;
    }

    T **operator&()
    {
        return (T **)&p;
    }

    bool operator!() const
    {
        return (p == NULL);
    }

    bool operator==(T *pT) const
    {
        return p == pT;
    }

    T *operator->()
    {
        return p;
    }

    void Release()
    {
        _attach((T *)NULL);
    }

private:
    T *_attach(T *p2)
    {
        T *p1 = exlib::atom_xchg(&p, p2);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    T *volatile p;
};

template<class T>
class weak_ptr
{
public:
    weak_ptr() :
        p(NULL)
    {
    }

    weak_ptr(T *lp) :
        p(NULL)
    {
        operator=(lp);
    }

    ~weak_ptr()
    {
    }

    T *operator=(T *lp)
    {
        exlib::atom_xchg(&p, lp);
        return lp;
    }

    operator T *() const
    {
        return p;
    }

    T *operator->()
    {
        return p;
    }

    void Ref()
    {
        T *p1 = p;
        if (p1)
            p1->Ref();
    }

    void Unref()
    {
        T *p1 = p;
        if (p1)
            p1->Unref();
    }

private:
    T *volatile p;
};

}

#endif /* OBJ_PTR_H_ */
