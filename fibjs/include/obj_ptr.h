/*
 * obj_ptr.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include <exlib/utils.h>


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
    int32_t refs_;
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

        return Attach(lp);
    }

    template<class Q>
    T *operator=(Q *lp)
    {
        if (lp != NULL)
            lp->Ref();

        return Attach(lp);
    }

    T *operator=(const obj_ptr<T> &lp)
    {
        return operator=(lp.p);
    }

    template<class Q>
    T *operator=(const obj_ptr<Q> &lp)
    {
        return operator=(lp.p);
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
        return &p;
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
        Attach((T *)NULL);
    }

    T *Attach(T *p2)
    {
        T *p1 = exlib::atom_xchg(&p, p2);
        if (p1)
            p1->Unref();

        return p2;
    }

    T *Detach()
    {
        return exlib::atom_xchg(&p, (T *)NULL);
    }

    T *p;
};

}

#endif /* OBJ_PTR_H_ */
