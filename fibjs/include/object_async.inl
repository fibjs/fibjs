#define ASYNC_STATIC0(cls, m) \
	static result_t ac_##m() { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	AsyncCall ac(NULL, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER0(cls, m) \
	result_t ac_##m() { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[0])->m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK0(cls, m) \
	void acb_##m(const char* ev = #m) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER1(cls, m) \
template<typename T0> \
	result_t ac_##m( \
		T0& v0) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[1])->m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK1(cls, m) \
	template<typename T0> \
	void acb_##m( \
		T0& v0, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER2(cls, m) \
template<typename T0, typename T1> \
	result_t ac_##m( \
		T0& v0, T1& v1) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[2])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK2(cls, m) \
	template<typename T0, typename T1> \
	void acb_##m( \
		T0& v0, T1& v1, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER3(cls, m) \
template<typename T0, typename T1, typename T2> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[3])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK3(cls, m) \
	template<typename T0, typename T1, typename T2> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[4])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK4(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[5])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK5(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[6])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK6(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[7])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK7(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[8])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK8(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(m_v7); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(m_v7); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	result_t ac_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[9])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	AsyncCall ac(args, _t::_stub); \
	s_acPool.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK9(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	void acb_##m( \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev = #m) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8, const char* ev) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), m_v(t->m_v8), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(m_v7); \
			b = b && c_v(m_v8); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_ev(ev) \
		{} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), m_v(t->m_v8), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		virtual bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_v5); \
			b = b && c_v(m_v6); \
			b = b && c_v(m_v7); \
			b = b && c_v(m_v8); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void callback() \
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
