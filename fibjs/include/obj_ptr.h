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

class obj_base
{
public:
    class weak_stub
    {
    public:
        weak_stub(obj_base *obj) :
            refs_(1), this_(obj)
        {
        }

        void Ref()
        {
            exlib::atom_inc(&refs_);
        }

        void Unref()
        {
            if (exlib::atom_dec(&refs_) == 0)
                delete this;
        }

        void clean()
        {
            exlib::atom_xchg(&this_, (obj_base *)NULL);
            Unref();
        }

        obj_base *obj()
        {
            return this_;
        }

    private:
        volatile int32_t refs_;
        obj_base *volatile this_;
    };

public:
    obj_base() :
        refs_(0), weak_(NULL)
    {
    }

    virtual ~obj_base()
    {
        if (weak_)
            weak_->clean();
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

    weak_stub *weak_ptr()
    {
        if (weak_)
            return weak_;

        weak_stub *pNew = new weak_stub(this);
        if (exlib::CompareAndSwap(&weak_, (weak_stub *)NULL, pNew))
            delete pNew;

        return weak_;
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
    weak_stub *volatile weak_;
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
        _assign(lp);
    }

    weak_ptr(const weak_ptr<T> &lp) :
        p(NULL)
    {
        _assign(lp);
    }

    ~weak_ptr()
    {
        Release();
    }

    T *operator=(T *lp)
    {
        return _assign(lp);
    }

    template<class Q>
    T *operator=(Q *lp)
    {
        return _assign(lp);
    }

    T *operator=(const weak_ptr<T> &lp)
    {
        return _assign(lp);
    }

    template<class Q>
    T *operator=(const weak_ptr<Q> &lp)
    {
        return _assign(lp);
    }

    operator T *() const
    {
        return p ? (T *)p->obj() : (T *)NULL;
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
        return (p == NULL) || (p->obj() == NULL);
    }

    bool operator==(T *pT) const
    {
        if (p == NULL)
            return pT == NULL;

        if (pT == NULL)
            return false;

        return (T *)p->obj() == pT;
    }

    T *operator->()
    {
        return p ? (T *)p->obj() : (T *)NULL;
    }

    void Release()
    {
        obj_base::weak_stub *p1 = exlib::atom_xchg(&p, (obj_base::weak_stub *)NULL);
        if (p1)
            p1->Unref();
    }

private:
    T *_assign(T *p2)
    {
        obj_base::weak_stub *p2_ = p2 ? p2->weak_ptr() : (obj_base::weak_stub *)NULL;
        if (p2_)
            p2_->Ref();

        obj_base::weak_stub *p1 = exlib::atom_xchg(&p, p2_);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    obj_base::weak_stub *volatile p;
};

template<class T>
class naked_ptr
{
public:
    naked_ptr() :
        p(NULL)
    {
    }

    naked_ptr(T *lp) :
        p(NULL)
    {
        operator=(lp);
    }

    ~naked_ptr()
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
