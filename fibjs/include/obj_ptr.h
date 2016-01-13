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
            refs_.inc();
        }

        void Unref()
        {
            if (refs_.dec() == 0)
                delete this;
        }

        void clean()
        {
            this_ = (obj_base *)NULL;
            Unref();
        }

        obj_base *obj()
        {
            return this_;
        }

    private:
        exlib::atomic refs_;
        exlib::atomic_ptr<obj_base> this_;
    };

public:
    obj_base() : gc_(false)
    {
    }

    virtual ~obj_base()
    {
        if (weak_)
            weak_.value()->clean();
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

    void dispose(bool gc)
    {
        if (refs_ == 0)
        {
            gc_ = gc;
            delete this;
        }
    }

    weak_stub *get_stub()
    {
        if (weak_)
            return weak_;

        weak_stub *pNew = new weak_stub(this);
        if (weak_.CompareAndSwap((weak_stub *)NULL, pNew))
            delete pNew;

        return weak_;
    }

protected:
    intptr_t internalRef()
    {
        return refs_.inc();
    }

    intptr_t internalUnref()
    {
        return refs_.dec();
    }

    intptr_t refs()
    {
        return refs_;
    }

    bool in_gc()
    {
        return gc_;
    }

private:
    exlib::atomic refs_;
    exlib::atomic_ptr<weak_stub> weak_;
    bool gc_;
};

template<class T>
class obj_ptr
{
public:
    obj_ptr()
    {
    }

    obj_ptr(T *lp)
    {
        operator=(lp);
    }

    obj_ptr(const obj_ptr<T> &lp)
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
        T *p1 = p.xchg(p2);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    exlib::atomic_ptr<T> p;
};

template<class T>
class weak_ptr
{
public:
    weak_ptr()
    {
    }

    weak_ptr(T *lp)
    {
        _assign(lp);
    }

    weak_ptr(const weak_ptr<T> &lp)
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
        return p ? (T *)p.value()->obj() : (T *)NULL;
    }

    T &operator*() const
    {
        return *p;
    }

    bool operator!() const
    {
        return (p == NULL) || (p.value()->obj() == NULL);
    }

    bool operator==(T *pT) const
    {
        if (p == NULL)
            return pT == NULL;

        if (pT == NULL)
            return false;

        return (T *)p.value()->obj() == pT;
    }

    T *operator->()
    {
        return p ? (T *)p.value()->obj() : (T *)NULL;
    }

    void Release()
    {
        obj_base::weak_stub *p1 = p.xchg((obj_base::weak_stub *)NULL);
        if (p1)
            p1->Unref();
    }

private:
    T *_assign(T *p2)
    {
        obj_base::weak_stub *p2_ = p2 ? p2->get_stub() : (obj_base::weak_stub *)NULL;
        if (p2_)
            p2_->Ref();

        obj_base::weak_stub *p1 = p.xchg(p2_);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    exlib::atomic_ptr<obj_base::weak_stub> p;
};

template<class T>
class naked_ptr
{
public:
    naked_ptr()
    {
    }

    naked_ptr(T *lp)
    {
        operator=(lp);
    }

    ~naked_ptr()
    {
    }

    T *operator=(T *lp)
    {
        p = lp;
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
    exlib::atomic_ptr<T> p;
};

}

#endif /* OBJ_PTR_H_ */
