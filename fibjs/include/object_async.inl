#define ASYNC_STATIC0(cls, m) \
	static result_t ac_##m() { \
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	_t ac(NULL); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER0(cls, m) \
	result_t ac_##m() { \
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[0])->m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK0(cls, m) \
	void acb_##m(const char* ev = #m) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, ev)); \
	}

#define ASYNC_VALUEBACK0(cls, m, rt) \
	void acb_##m(const char* ev = #m) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, ev)); \
	}

#define ASYNC_STATIC1(cls, m) \
template<typename T0> \
	static result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER1(cls, m) \
template<typename T0> \
	result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK1(cls, m) \
	template<typename T0> \
	void acb_##m( \
		T0& v0, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, ev)); \
	}

#define ASYNC_VALUEBACK1(cls, m, rt) \
	template<typename T0> \
	void acb_##m( \
		T0& v0, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, ev)); \
	}

#define ASYNC_STATIC2(cls, m) \
template<typename T0, typename T1> \
	static result_t ac_##m( \
		T0& v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER2(cls, m) \
template<typename T0, typename T1> \
	result_t ac_##m( \
		T0& v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK2(cls, m) \
	template<typename T0, typename T1> \
	void acb_##m( \
		T0& v0, T1& v1, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, ev)); \
	}

#define ASYNC_VALUEBACK2(cls, m, rt) \
	template<typename T0, typename T1> \
	void acb_##m( \
		T0& v0, T1& v1, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, ev)); \
	}

#define ASYNC_STATIC3(cls, m) \
template<typename T0, typename T1, typename T2> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER3(cls, m) \
template<typename T0, typename T1, typename T2> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK3(cls, m) \
	template<typename T0, typename T1, typename T2> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, ev)); \
	}

#define ASYNC_VALUEBACK3(cls, m, rt) \
	template<typename T0, typename T1, typename T2> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, ev)); \
	}

#define ASYNC_STATIC4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK4(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, ev)); \
	}

#define ASYNC_VALUEBACK4(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, ev)); \
	}

#define ASYNC_STATIC5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK5(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, ev)); \
	}

#define ASYNC_VALUEBACK5(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, ev)); \
	}

#define ASYNC_STATIC6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK6(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, ev)); \
	}

#define ASYNC_VALUEBACK6(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, ev)); \
	}

#define ASYNC_STATIC7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK7(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, ev)); \
	}

#define ASYNC_VALUEBACK7(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, ev)); \
	}

#define ASYNC_STATIC8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK8(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), m_v(m_v7), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		T7 m_v7; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, ev)); \
	}

#define ASYNC_VALUEBACK8(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), m_v(m_v7), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		T7 m_v7; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, ev)); \
	}

#define ASYNC_STATIC9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	static result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK9(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), m_v(m_v7), m_v(m_v8), this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev); }\
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		T7 m_v7; \
		T8 m_v8; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8, ev)); \
	}

#define ASYNC_VALUEBACK9(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev) : \
			AsyncCallBack(pThis, NULL), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_ev(ev) \
		{} \
		virtual void invoke() \
		{ \
			result_t hr = ((cls*)(object_base*)m_pThis)->m(m_v(m_v0), m_v(m_v1), m_v(m_v2), m_v(m_v3), m_v(m_v4), m_v(m_v5), m_v(m_v6), m_v(m_v7), m_v(m_v8), retVal, this); \
			if (hr != CALL_E_PENDDING)post(hr); \
		} \
		virtual void js_callback() \
		{ _trigger(m_ev, retVal); }\
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		T7 m_v7; \
		T8 m_v8; \
		const char* m_ev; \
	}; \
	s_acPool.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8, ev)); \
	}
