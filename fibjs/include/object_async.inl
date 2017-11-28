#define ASYNC_STATIC0_AC(cls, m) \
	static result_t ac_##m() { \
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	_t ac(NULL); \
	return ac.check_result(m(&ac)); \
	}
#define ASYNC_STATIC0_CC(cls, m) \
	static result_t cc_##m() { \
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	_t ac(NULL); \
	return ac.check_result(m(&ac)); \
	}
#define ASYNC_STATIC0_ACB(cls, m) \
	static void acb_##m(v8::Local<v8::Function> cb) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
	}; \
	_t* ac = new _t(cb); \
	ac->check_result(m(ac)); \
	}
#define ASYNC_STATIC0(cls, m) \
	ASYNC_STATIC0_AC(cls, m) \
	ASYNC_STATIC0_CC(cls, m) \
	ASYNC_STATIC0_ACB(cls, m)

#define ASYNC_MEMBER0_AC(cls, m) \
	result_t ac_##m() { \
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[0])->m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {this}; \
	_t ac(args); \
	return ac.check_result(m(&ac)); \
	}
#define ASYNC_MEMBER0_CC(cls, m) \
	result_t cc_##m() { \
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[0])->m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {this}; \
	_t ac(args); \
	return ac.check_result(m(&ac)); \
	}
#define ASYNC_MEMBER0_ACB(cls, m) \
	void acb_##m(v8::Local<v8::Function> cb) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
	}; \
	_t* ac = new _t(this, cb); \
	ac->check_result(m(ac)); \
	}
#define ASYNC_MEMBER0(cls, m) \
	ASYNC_MEMBER0_AC(cls, m) \
	ASYNC_MEMBER0_CC(cls, m) \
	ASYNC_MEMBER0_ACB(cls, m)

#define ASYNC_STATICVALUE1_AC(cls, m, T0) \
	static result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_STATICVALUE1_CC(cls, m, T0) \
	static result_t cc_##m( \
		T0& v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_STATICVALUE1_ACB(cls, m, T0) \
	static void acb_##m(v8::Local<v8::Function> cb) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T0 retVal; \
	private: \
	}; \
	_t* ac = new _t(cb); \
	ac->check_result(m(ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE1(cls, m, T0) \
	ASYNC_STATICVALUE1_AC(cls, m, T0) \
	ASYNC_STATICVALUE1_CC(cls, m, T0) \
	ASYNC_STATICVALUE1_ACB(cls, m, T0)

#define ASYNC_MEMBERVALUE1_AC(cls, m, T0) \
	result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_MEMBERVALUE1_CC(cls, m, T0) \
	result_t cc_##m( \
		T0& v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_MEMBERVALUE1_ACB(cls, m, T0) \
	void acb_##m(v8::Local<v8::Function> cb) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T0 retVal; \
	private: \
	}; \
	_t* ac = new _t(this, cb); \
	ac->check_result(m(ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE1(cls, m, T0) \
	ASYNC_MEMBERVALUE1_AC(cls, m, T0) \
	ASYNC_MEMBERVALUE1_CC(cls, m, T0) \
	ASYNC_MEMBERVALUE1_ACB(cls, m, T0)

#define ASYNC_STATIC1_AC(cls, m, T0) \
	static result_t ac_##m( \
		T0 v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_STATIC1_CC(cls, m, T0) \
	static result_t cc_##m( \
		T0 v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_STATIC1_ACB(cls, m, T0) \
	static void acb_##m( \
		T0 v0, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
	}; \
	_t* ac = new _t(v0, cb); \
	ac->check_result(m(v0, ac)); \
	}
#define ASYNC_STATIC1(cls, m, T0) \
	ASYNC_STATIC1_AC(cls, m, T0) \
	ASYNC_STATIC1_CC(cls, m, T0) \
	ASYNC_STATIC1_ACB(cls, m, T0)

#define ASYNC_MEMBER1_AC(cls, m, T0) \
	result_t ac_##m( \
		T0 v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_MEMBER1_CC(cls, m, T0) \
	result_t cc_##m( \
		T0 v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, &ac)); \
	}
#define ASYNC_MEMBER1_ACB(cls, m, T0) \
	void acb_##m( \
		T0 v0, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
	}; \
	_t* ac = new _t(this, v0, cb); \
	ac->check_result(m(v0, ac)); \
	}
#define ASYNC_MEMBER1(cls, m, T0) \
	ASYNC_MEMBER1_AC(cls, m, T0) \
	ASYNC_MEMBER1_CC(cls, m, T0) \
	ASYNC_MEMBER1_ACB(cls, m, T0)

#define ASYNC_STATICVALUE2_AC(cls, m, T0, T1) \
	static result_t ac_##m( \
		T0 v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_STATICVALUE2_CC(cls, m, T0, T1) \
	static result_t cc_##m( \
		T0 v0, T1& v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_STATICVALUE2_ACB(cls, m, T0, T1) \
	static void acb_##m( \
		T0 v0, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T1 retVal; \
	private: \
		_at<T0> m_v0; \
	}; \
	_t* ac = new _t(v0, cb); \
	ac->check_result(m(v0, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE2(cls, m, T0, T1) \
	ASYNC_STATICVALUE2_AC(cls, m, T0, T1) \
	ASYNC_STATICVALUE2_CC(cls, m, T0, T1) \
	ASYNC_STATICVALUE2_ACB(cls, m, T0, T1)

#define ASYNC_MEMBERVALUE2_AC(cls, m, T0, T1) \
	result_t ac_##m( \
		T0 v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_MEMBERVALUE2_CC(cls, m, T0, T1) \
	result_t cc_##m( \
		T0 v0, T1& v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_MEMBERVALUE2_ACB(cls, m, T0, T1) \
	void acb_##m( \
		T0 v0, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T1 retVal; \
	private: \
		_at<T0> m_v0; \
	}; \
	_t* ac = new _t(this, v0, cb); \
	ac->check_result(m(v0, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE2(cls, m, T0, T1) \
	ASYNC_MEMBERVALUE2_AC(cls, m, T0, T1) \
	ASYNC_MEMBERVALUE2_CC(cls, m, T0, T1) \
	ASYNC_MEMBERVALUE2_ACB(cls, m, T0, T1)

#define ASYNC_STATIC2_AC(cls, m, T0, T1) \
	static result_t ac_##m( \
		T0 v0, T1 v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_STATIC2_CC(cls, m, T0, T1) \
	static result_t cc_##m( \
		T0 v0, T1 v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_STATIC2_ACB(cls, m, T0, T1) \
	static void acb_##m( \
		T0 v0, T1 v1, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
	}; \
	_t* ac = new _t(v0, v1, cb); \
	ac->check_result(m(v0, v1, ac)); \
	}
#define ASYNC_STATIC2(cls, m, T0, T1) \
	ASYNC_STATIC2_AC(cls, m, T0, T1) \
	ASYNC_STATIC2_CC(cls, m, T0, T1) \
	ASYNC_STATIC2_ACB(cls, m, T0, T1)

#define ASYNC_MEMBER2_AC(cls, m, T0, T1) \
	result_t ac_##m( \
		T0 v0, T1 v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_MEMBER2_CC(cls, m, T0, T1) \
	result_t cc_##m( \
		T0 v0, T1 v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, &ac)); \
	}
#define ASYNC_MEMBER2_ACB(cls, m, T0, T1) \
	void acb_##m( \
		T0 v0, T1 v1, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
	}; \
	_t* ac = new _t(this, v0, v1, cb); \
	ac->check_result(m(v0, v1, ac)); \
	}
#define ASYNC_MEMBER2(cls, m, T0, T1) \
	ASYNC_MEMBER2_AC(cls, m, T0, T1) \
	ASYNC_MEMBER2_CC(cls, m, T0, T1) \
	ASYNC_MEMBER2_ACB(cls, m, T0, T1)

#define ASYNC_STATICVALUE3_AC(cls, m, T0, T1, T2) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_STATICVALUE3_CC(cls, m, T0, T1, T2) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_STATICVALUE3_ACB(cls, m, T0, T1, T2) \
	static void acb_##m( \
		T0 v0, T1 v1, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T2 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
	}; \
	_t* ac = new _t(v0, v1, cb); \
	ac->check_result(m(v0, v1, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE3(cls, m, T0, T1, T2) \
	ASYNC_STATICVALUE3_AC(cls, m, T0, T1, T2) \
	ASYNC_STATICVALUE3_CC(cls, m, T0, T1, T2) \
	ASYNC_STATICVALUE3_ACB(cls, m, T0, T1, T2)

#define ASYNC_MEMBERVALUE3_AC(cls, m, T0, T1, T2) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_MEMBERVALUE3_CC(cls, m, T0, T1, T2) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_MEMBERVALUE3_ACB(cls, m, T0, T1, T2) \
	void acb_##m( \
		T0 v0, T1 v1, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T2 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
	}; \
	_t* ac = new _t(this, v0, v1, cb); \
	ac->check_result(m(v0, v1, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE3(cls, m, T0, T1, T2) \
	ASYNC_MEMBERVALUE3_AC(cls, m, T0, T1, T2) \
	ASYNC_MEMBERVALUE3_CC(cls, m, T0, T1, T2) \
	ASYNC_MEMBERVALUE3_ACB(cls, m, T0, T1, T2)

#define ASYNC_STATIC3_AC(cls, m, T0, T1, T2) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_STATIC3_CC(cls, m, T0, T1, T2) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_STATIC3_ACB(cls, m, T0, T1, T2) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
	}; \
	_t* ac = new _t(v0, v1, v2, cb); \
	ac->check_result(m(v0, v1, v2, ac)); \
	}
#define ASYNC_STATIC3(cls, m, T0, T1, T2) \
	ASYNC_STATIC3_AC(cls, m, T0, T1, T2) \
	ASYNC_STATIC3_CC(cls, m, T0, T1, T2) \
	ASYNC_STATIC3_ACB(cls, m, T0, T1, T2)

#define ASYNC_MEMBER3_AC(cls, m, T0, T1, T2) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_MEMBER3_CC(cls, m, T0, T1, T2) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, &ac)); \
	}
#define ASYNC_MEMBER3_ACB(cls, m, T0, T1, T2) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, cb); \
	ac->check_result(m(v0, v1, v2, ac)); \
	}
#define ASYNC_MEMBER3(cls, m, T0, T1, T2) \
	ASYNC_MEMBER3_AC(cls, m, T0, T1, T2) \
	ASYNC_MEMBER3_CC(cls, m, T0, T1, T2) \
	ASYNC_MEMBER3_ACB(cls, m, T0, T1, T2)

#define ASYNC_STATICVALUE4_AC(cls, m, T0, T1, T2, T3) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_STATICVALUE4_CC(cls, m, T0, T1, T2, T3) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_STATICVALUE4_ACB(cls, m, T0, T1, T2, T3) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T3 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
	}; \
	_t* ac = new _t(v0, v1, v2, cb); \
	ac->check_result(m(v0, v1, v2, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE4(cls, m, T0, T1, T2, T3) \
	ASYNC_STATICVALUE4_AC(cls, m, T0, T1, T2, T3) \
	ASYNC_STATICVALUE4_CC(cls, m, T0, T1, T2, T3) \
	ASYNC_STATICVALUE4_ACB(cls, m, T0, T1, T2, T3)

#define ASYNC_MEMBERVALUE4_AC(cls, m, T0, T1, T2, T3) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_MEMBERVALUE4_CC(cls, m, T0, T1, T2, T3) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_MEMBERVALUE4_ACB(cls, m, T0, T1, T2, T3) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T3 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, cb); \
	ac->check_result(m(v0, v1, v2, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE4(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBERVALUE4_AC(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBERVALUE4_CC(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBERVALUE4_ACB(cls, m, T0, T1, T2, T3)

#define ASYNC_STATIC4_AC(cls, m, T0, T1, T2, T3) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_STATIC4_CC(cls, m, T0, T1, T2, T3) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_STATIC4_ACB(cls, m, T0, T1, T2, T3) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, cb); \
	ac->check_result(m(v0, v1, v2, v3, ac)); \
	}
#define ASYNC_STATIC4(cls, m, T0, T1, T2, T3) \
	ASYNC_STATIC4_AC(cls, m, T0, T1, T2, T3) \
	ASYNC_STATIC4_CC(cls, m, T0, T1, T2, T3) \
	ASYNC_STATIC4_ACB(cls, m, T0, T1, T2, T3)

#define ASYNC_MEMBER4_AC(cls, m, T0, T1, T2, T3) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_MEMBER4_CC(cls, m, T0, T1, T2, T3) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, &ac)); \
	}
#define ASYNC_MEMBER4_ACB(cls, m, T0, T1, T2, T3) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, cb); \
	ac->check_result(m(v0, v1, v2, v3, ac)); \
	}
#define ASYNC_MEMBER4(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBER4_AC(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBER4_CC(cls, m, T0, T1, T2, T3) \
	ASYNC_MEMBER4_ACB(cls, m, T0, T1, T2, T3)

#define ASYNC_STATICVALUE5_AC(cls, m, T0, T1, T2, T3, T4) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_STATICVALUE5_CC(cls, m, T0, T1, T2, T3, T4) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_STATICVALUE5_ACB(cls, m, T0, T1, T2, T3, T4) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T4 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, cb); \
	ac->check_result(m(v0, v1, v2, v3, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE5(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATICVALUE5_AC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATICVALUE5_CC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATICVALUE5_ACB(cls, m, T0, T1, T2, T3, T4)

#define ASYNC_MEMBERVALUE5_AC(cls, m, T0, T1, T2, T3, T4) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_MEMBERVALUE5_CC(cls, m, T0, T1, T2, T3, T4) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_MEMBERVALUE5_ACB(cls, m, T0, T1, T2, T3, T4) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T4 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, cb); \
	ac->check_result(m(v0, v1, v2, v3, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE5(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBERVALUE5_AC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBERVALUE5_CC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBERVALUE5_ACB(cls, m, T0, T1, T2, T3, T4)

#define ASYNC_STATIC5_AC(cls, m, T0, T1, T2, T3, T4) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_STATIC5_CC(cls, m, T0, T1, T2, T3, T4) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_STATIC5_ACB(cls, m, T0, T1, T2, T3, T4) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, ac)); \
	}
#define ASYNC_STATIC5(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATIC5_AC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATIC5_CC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_STATIC5_ACB(cls, m, T0, T1, T2, T3, T4)

#define ASYNC_MEMBER5_AC(cls, m, T0, T1, T2, T3, T4) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_MEMBER5_CC(cls, m, T0, T1, T2, T3, T4) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, &ac)); \
	}
#define ASYNC_MEMBER5_ACB(cls, m, T0, T1, T2, T3, T4) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, ac)); \
	}
#define ASYNC_MEMBER5(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBER5_AC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBER5_CC(cls, m, T0, T1, T2, T3, T4) \
	ASYNC_MEMBER5_ACB(cls, m, T0, T1, T2, T3, T4)

#define ASYNC_STATICVALUE6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_STATICVALUE6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_STATICVALUE6_ACB(cls, m, T0, T1, T2, T3, T4, T5) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T5 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE6(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATICVALUE6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATICVALUE6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATICVALUE6_ACB(cls, m, T0, T1, T2, T3, T4, T5)

#define ASYNC_MEMBERVALUE6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_MEMBERVALUE6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_MEMBERVALUE6_ACB(cls, m, T0, T1, T2, T3, T4, T5) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T5 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE6(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBERVALUE6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBERVALUE6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBERVALUE6_ACB(cls, m, T0, T1, T2, T3, T4, T5)

#define ASYNC_STATIC6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_STATIC6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_STATIC6_ACB(cls, m, T0, T1, T2, T3, T4, T5) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, ac)); \
	}
#define ASYNC_STATIC6(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATIC6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATIC6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_STATIC6_ACB(cls, m, T0, T1, T2, T3, T4, T5)

#define ASYNC_MEMBER6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_MEMBER6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, &ac)); \
	}
#define ASYNC_MEMBER6_ACB(cls, m, T0, T1, T2, T3, T4, T5) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, ac)); \
	}
#define ASYNC_MEMBER6(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBER6_AC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBER6_CC(cls, m, T0, T1, T2, T3, T4, T5) \
	ASYNC_MEMBER6_ACB(cls, m, T0, T1, T2, T3, T4, T5)

#define ASYNC_STATICVALUE7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_STATICVALUE7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_STATICVALUE7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T6 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATICVALUE7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATICVALUE7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATICVALUE7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6)

#define ASYNC_MEMBERVALUE7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_MEMBERVALUE7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_MEMBERVALUE7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T6 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBERVALUE7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBERVALUE7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBERVALUE7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6)

#define ASYNC_STATIC7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_STATIC7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_STATIC7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, ac)); \
	}
#define ASYNC_STATIC7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATIC7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATIC7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_STATIC7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6)

#define ASYNC_MEMBER7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_MEMBER7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, &ac)); \
	}
#define ASYNC_MEMBER7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, ac)); \
	}
#define ASYNC_MEMBER7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBER7_AC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBER7_CC(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	ASYNC_MEMBER7_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6)

#define ASYNC_STATICVALUE8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_STATICVALUE8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_STATICVALUE8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T7 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATICVALUE8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATICVALUE8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATICVALUE8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7)

#define ASYNC_MEMBERVALUE8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_MEMBERVALUE8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_MEMBERVALUE8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T7 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBERVALUE8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBERVALUE8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBERVALUE8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7)

#define ASYNC_STATIC8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_STATIC8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_STATIC8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, v7, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, ac)); \
	}
#define ASYNC_STATIC8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATIC8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATIC8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_STATIC8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7)

#define ASYNC_MEMBER8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_MEMBER8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, &ac)); \
	}
#define ASYNC_MEMBER8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, ac)); \
	}
#define ASYNC_MEMBER8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBER8_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBER8_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	ASYNC_MEMBER8_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7)

#define ASYNC_STATICVALUE9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_STATICVALUE9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_STATICVALUE9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T8 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, v7, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATICVALUE9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATICVALUE9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATICVALUE9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8)

#define ASYNC_MEMBERVALUE9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_MEMBERVALUE9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_MEMBERVALUE9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T8 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBERVALUE9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBERVALUE9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBERVALUE9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8)

#define ASYNC_STATIC9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_STATIC9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_STATIC9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), m_v8.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
		_at<T8> m_v8; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, v7, v8, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, ac)); \
	}
#define ASYNC_STATIC9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATIC9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATIC9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_STATIC9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8)

#define ASYNC_MEMBER9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_MEMBER9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, &ac)); \
	}
#define ASYNC_MEMBER9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), m_v8.value(), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
		_at<T8> m_v8; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, ac)); \
	}
#define ASYNC_MEMBER9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBER9_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBER9_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	ASYNC_MEMBER9_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8)

#define ASYNC_STATICVALUE10_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, T9& v9) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], *(T9*) args[9], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, &ac)); \
	}
#define ASYNC_STATICVALUE10_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, T9& v9) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], *(T9*) args[9], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, &ac)); \
	}
#define ASYNC_STATICVALUE10_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	static void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = cls::m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), m_v8.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T9 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
		_at<T8> m_v8; \
	}; \
	_t* ac = new _t(v0, v1, v2, v3, v4, v5, v6, v7, v8, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, ac->retVal, ac)); \
	}
#define ASYNC_STATICVALUE10(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_STATICVALUE10_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_STATICVALUE10_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_STATICVALUE10_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)

#define ASYNC_MEMBERVALUE10_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, T9& v9) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[10])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], *(T9*) args[9], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, &ac)); \
	}
#define ASYNC_MEMBERVALUE10_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, T9& v9) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			setAsync(); \
			result_t hr = ((cls*)args[10])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], *(T9*) args[9], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, this}; \
	_t ac(args); \
	return ac.check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, &ac)); \
	}
#define ASYNC_MEMBERVALUE10_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	void acb_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8, v8::Local<v8::Function> cb) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, v8::Local<v8::Function> cb) : \
			AsyncCallBack(cb, pThis), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{} \
		virtual void invoke() \
		{ \
			setAsync(); \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v0.value(), m_v1.value(), m_v2.value(), m_v3.value(), m_v4.value(), m_v5.value(), m_v6.value(), m_v7.value(), m_v8.value(), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
    	virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args) \
    	{ fillRetVal(args, retVal); } \
	public: \
		T9 retVal; \
	private: \
		_at<T0> m_v0; \
		_at<T1> m_v1; \
		_at<T2> m_v2; \
		_at<T3> m_v3; \
		_at<T4> m_v4; \
		_at<T5> m_v5; \
		_at<T6> m_v6; \
		_at<T7> m_v7; \
		_at<T8> m_v8; \
	}; \
	_t* ac = new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8, cb); \
	ac->check_result(m(v0, v1, v2, v3, v4, v5, v6, v7, v8, ac->retVal, ac)); \
	}
#define ASYNC_MEMBERVALUE10(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_MEMBERVALUE10_AC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_MEMBERVALUE10_CC(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	ASYNC_MEMBERVALUE10_ACB(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)
