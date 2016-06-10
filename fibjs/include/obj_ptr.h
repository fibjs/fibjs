/*
 * obj_ptr.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include <exlib/include/utils.h>
#include <atomic>

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
            refs_++;
        }

        void Unref()
        {
            if (--refs_ == 0)
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
        std::atomic_intptr_t refs_;
        std::atomic<obj_base*> this_;
    };

public:
    obj_base() : refs_(0), weak_(NULL)
    {}

    virtual ~obj_base()
    {
        weak_stub* p = weak_.load();

        if (p)
            p->clean();
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

    weak_stub *get_stub()
    {
        weak_stub* p = weak_.load();

        if (p)
            return p;

        weak_stub *pNew = new weak_stub(this);
        weak_stub * tst = NULL;
        if (!weak_.compare_exchange_strong(tst, pNew))
            delete pNew;

        return weak_;
    }

protected:
    intptr_t internalRef()
    {
        return ++refs_;
    }

    intptr_t internalUnref()
    {
        return --refs_;
    }

    intptr_t refs()
    {
        return refs_;
    }

private:
    std::atomic_intptr_t refs_;
    std::atomic<weak_stub*> weak_;
};

template<class T>
class obj_ptr
{
public:
    obj_ptr() : p(NULL)
    {
    }

    obj_ptr(T *lp) : p(NULL)
    {
        operator=(lp);
    }

    obj_ptr(const obj_ptr<T> &lp) : p(NULL)
    {
        operator=(lp);
    }

    template<class Q>
    obj_ptr(const obj_ptr<Q> &lp) : p(NULL)
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
        return (p == (T*)NULL);
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
        T *p1 = p.exchange(p2);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    std::atomic<T*> p;
};

template<class T>
class weak_ptr
{
public:
    weak_ptr() : p(NULL)
    {
    }

    weak_ptr(T *lp) : p(NULL)
    {
        _assign(lp);
    }

    weak_ptr(const weak_ptr<T> &lp) : p(NULL)
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
        obj_base::weak_stub* p1 = p.load();
        return p1 ? (T *)p1->obj() : (T *)NULL;
    }

    T &operator*() const
    {
        return *p;
    }

    bool operator!() const
    {
        obj_base::weak_stub* p1 = p.load();
        return (p1 == NULL) || (p1->obj() == NULL);
    }

    bool operator==(T *pT) const
    {
        obj_base::weak_stub* p1 = p.load();

        if (p1 == NULL)
            return pT == NULL;

        if (pT == NULL)
            return false;

        return (T *)p1->obj() == pT;
    }

    T *operator->()
    {
        obj_base::weak_stub* p1 = p.load();
        return p1 ? (T *)p1->obj() : (T *)NULL;
    }

    void Release()
    {
        obj_base::weak_stub *p1 = p.exchange((obj_base::weak_stub *)NULL);
        if (p1)
            p1->Unref();
    }

private:
    T *_assign(T *p2)
    {
        obj_base::weak_stub *p2_ = p2 ? p2->get_stub() : (obj_base::weak_stub *)NULL;
        if (p2_)
            p2_->Ref();

        obj_base::weak_stub *p1 = p.exchange(p2_);
        if (p1)
            p1->Unref();

        return p2;
    }

private:
    std::atomic<obj_base::weak_stub*> p;
};

template<class T>
class naked_ptr
{
public:
    naked_ptr() : p(NULL), rp(NULL)
    {
    }

    naked_ptr(T *lp) : p(NULL), rp(NULL)
    {
        operator=(lp);
    }

    ~naked_ptr()
    {
        dispose();
    }

    T *operator=(T *lp)
    {
        dispose();
        p = lp;
        return lp;
    }

    operator T *()
    {
        return get_ptr();
    }

    T *operator->()
    {
        return get_ptr();
    }

    void dispose()
    {
        T *rp1 = rp.exchange(NULL);
        if (rp1)
        {
            rp1->dispose();
            rp1->Unref();
        }
    }

private:
    T* get_ptr()
    {
        T *p1 = p;
        T *rp1 = rp;

        if (!p1 || p1 == rp1)
            return p1;

        p1->Ref();
        rp1 = rp.exchange(p1);
        if (rp1)
            rp1->Unref();

        return p1;
    }

private:
    std::atomic<T*> p;
    std::atomic<T*> rp;
};

}

#endif /* OBJ_PTR_H_ */
