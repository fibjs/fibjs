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

class obj_base
{
public:
	obj_base() :
			refs_(0)
	{
	}

	virtual ~obj_base()
	{
	}

public:
	void Ref()
	{
		if (exlib::atom_inc(&refs_) == 1)
			firstRef();
	}

	void Unref()
	{
		if (exlib::atom_dec(&refs_) == 0)
			finalRelease();
	}

	virtual void firstRef()
	{
	}

	virtual void finalRelease()
	{
		delete this;
	}

	void dispose()
	{
		if (refs_ == 0)
			delete this;
	}

private:
	int refs_;
};

template<class T>
class obj_ptr
{
public:
	obj_ptr() :
			p(NULL)
	{
	}

	obj_ptr(T* lp) :
			p(NULL)
	{
		operator=(lp);
	}

	obj_ptr(const obj_ptr<T>& lp) :
			p(NULL)
	{
		operator=(lp);
	}

	~obj_ptr()
	{
		Release();
	}

	T* operator=(T* lp)
	{
		if (lp != NULL)
			lp->Ref();

		return Attach(lp);
	}

	T* operator=(const obj_ptr<T>& lp)
	{
		return operator=(lp.p);
	}

	operator T*() const
	{
		return p;
	}

	T& operator*() const
	{
		return *p;
	}

	T** operator&()
	{
		return &p;
	}

	bool operator!() const
	{
		return (p == NULL);
	}

	bool operator==(T* pT) const
	{
		return p == pT;
	}

	T* operator->()
	{
		return p;
	}

	void Release()
	{
		T* pTemp = Detach();
		if (pTemp)
			pTemp->Unref();
	}

	T* Attach(T* p2)
	{
		T* p1 = p;
		p = p2;

		if (p1)
			p1->Unref();

		return p2;
	}

	T* Detach()
	{
		T* p1 = p;

		p = NULL;
		return p1;
	}

	T* p;
};

}

#endif /* OBJ_PTR_H_ */
