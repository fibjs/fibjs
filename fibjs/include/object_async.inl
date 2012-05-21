#define ASYNC_STATIC0(cls, m) \
	static result_t ac_##m(AsyncQueue& q) { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	AsyncCall ac(NULL, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER0(cls, m) \
	result_t ac_##m(AsyncQueue& q) { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[0])->m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK0(cls, m) \
	void acb_##m(AsyncQueue& q) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis) : \
			AsyncCallBack(pThis, NULL, _stub) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
	}; \
	q.put(new _t(this)); \
	}

#define ASYNC_VALUEBACK0(cls, m, rt) \
	void acb_##m(AsyncQueue& q) { \
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis) : \
			AsyncCallBack(pThis, NULL, _stub) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
	}; \
	q.put(new _t(this)); \
	}

#define ASYNC_STATIC1(cls, m) \
template<typename T0> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER1(cls, m) \
template<typename T0> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[1])->m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK1(cls, m) \
	template<typename T0> \
	void acb_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
	}; \
	q.put(new _t(this, v0)); \
	}

#define ASYNC_VALUEBACK1(cls, m, rt) \
	template<typename T0> \
	void acb_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
	}; \
	q.put(new _t(this, v0)); \
	}

#define ASYNC_STATIC2(cls, m) \
template<typename T0, typename T1> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER2(cls, m) \
template<typename T0, typename T1> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[2])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK2(cls, m) \
	template<typename T0, typename T1> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
	}; \
	q.put(new _t(this, v0, v1)); \
	}

#define ASYNC_VALUEBACK2(cls, m, rt) \
	template<typename T0, typename T1> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
	}; \
	q.put(new _t(this, v0, v1)); \
	}

#define ASYNC_STATIC3(cls, m) \
template<typename T0, typename T1, typename T2> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER3(cls, m) \
template<typename T0, typename T1, typename T2> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[3])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK3(cls, m) \
	template<typename T0, typename T1, typename T2> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
	}; \
	q.put(new _t(this, v0, v1, v2)); \
	}

#define ASYNC_VALUEBACK3(cls, m, rt) \
	template<typename T0, typename T1, typename T2> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
	}; \
	q.put(new _t(this, v0, v1, v2)); \
	}

#define ASYNC_STATIC4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[4])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK4(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3)); \
	}

#define ASYNC_VALUEBACK4(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(retVal); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3)); \
	}

#define ASYNC_STATIC5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[5])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK5(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
		{	bool b = true; \
			b = b && c_v(m_v0); \
			b = b && c_v(m_v1); \
			b = b && c_v(m_v2); \
			b = b && c_v(m_v3); \
			b = b && c_v(m_v4); \
			b = b && c_v(m_pThis); \
			return b; \
		} \
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4)); \
	}

#define ASYNC_VALUEBACK5(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4)); \
	}

#define ASYNC_STATIC6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[6])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK6(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5)); \
	}

#define ASYNC_VALUEBACK6(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5)); \
	}

#define ASYNC_STATIC7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[7])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK7(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6)); \
	}

#define ASYNC_VALUEBACK7(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		rt retVal; \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6)); \
	}

#define ASYNC_STATIC8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[8])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK8(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
	private: \
		T0 m_v0; \
		T1 m_v1; \
		T2 m_v2; \
		T3 m_v3; \
		T4 m_v4; \
		T5 m_v5; \
		T6 m_v6; \
		T7 m_v7; \
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7)); \
	}

#define ASYNC_VALUEBACK8(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
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
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7)); \
	}

#define ASYNC_STATIC9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_MEMBER9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[9])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	AsyncCall ac(args, _t::_stub); \
	q.put(&ac); \
	return ac.wait();}

#define ASYNC_CALLBACK9(cls, m) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), m_v(t->m_v8), t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{\
			m_pThis->_trigger(#m, NULL, 0); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
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
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8)); \
	}

#define ASYNC_VALUEBACK9(cls, m, rt) \
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	void acb_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t: public AsyncCallBack { \
	public: \
		_t(cls* pThis, T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) : \
			AsyncCallBack(pThis, NULL, _stub), m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) \
		{	pThis->Ref();} \
		static void _stub(AsyncCall* ac) \
		{	_t* t = (_t*) ac; \
			result_t hr = ((cls*)t->m_pThis)->m(m_v(t->m_v0), m_v(t->m_v1), m_v(t->m_v2), m_v(t->m_v3), m_v(t->m_v4), m_v(t->m_v5), m_v(t->m_v6), m_v(t->m_v7), m_v(t->m_v8), t->retVal, t); \
			if (hr != CALL_E_PENDDING)t->post(hr); \
		} \
		bool clear_value() \
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
		virtual void post(int v) \
		{	if(m_pThis->hasTrigger() || !clear_value()) \
				AsyncCallBack::post(v); \
			else delete this; \
		} \
		virtual void callback() \
		{	v8::Handle<v8::Value> v = ReturnValue(retVal); \
			m_pThis->_trigger(#m, &v, 1); \
			if(m_pThis)m_pThis->Unref(); \
			delete this; \
		} \
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
	}; \
	q.put(new _t(this, v0, v1, v2, v3, v4, v5, v6, v7, v8)); \
	}
