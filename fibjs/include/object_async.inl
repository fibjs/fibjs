#define ASYNC_STATIC0(cls, m) \
	static result_t ac_##m() { \
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	_t ac(NULL); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m() { \
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	_t ac(NULL); \
	ac.async(hr == CALL_E_LONGSYNC); \
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
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m() { \
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[0])->m( \
				this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC1(cls, m, T0) \
	static result_t ac_##m( \
		T0 v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER1(cls, m, T0) \
	result_t ac_##m( \
		T0 v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE1(cls, m, T0) \
	static result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0& v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE1(cls, m, T0) \
	result_t ac_##m( \
		T0& v0) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0& v0) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[1])->m( \
				*(T0*) args[0], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC2(cls, m, T0, T1) \
	static result_t ac_##m( \
		T0 v0, T1 v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER2(cls, m, T0, T1) \
	result_t ac_##m( \
		T0 v0, T1 v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE2(cls, m, T0, T1) \
	static result_t ac_##m( \
		T0 v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1& v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE2(cls, m, T0, T1) \
	result_t ac_##m( \
		T0 v0, T1& v1) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1& v1) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[2])->m( \
				*(T0*) args[0], *(T1*) args[1], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC3(cls, m, T0, T1, T2) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER3(cls, m, T0, T1, T2) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE3(cls, m, T0, T1, T2) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE3(cls, m, T0, T1, T2) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2& v2) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[3])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC4(cls, m, T0, T1, T2, T3) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER4(cls, m, T0, T1, T2, T3) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE4(cls, m, T0, T1, T2, T3) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE4(cls, m, T0, T1, T2, T3) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3& v3) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[4])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC5(cls, m, T0, T1, T2, T3, T4) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER5(cls, m, T0, T1, T2, T3, T4) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE5(cls, m, T0, T1, T2, T3, T4) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE5(cls, m, T0, T1, T2, T3, T4) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4& v4) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[5])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC6(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER6(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE6(cls, m, T0, T1, T2, T3, T4, T5) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE6(cls, m, T0, T1, T2, T3, T4, T5) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5& v5) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[6])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE7(cls, m, T0, T1, T2, T3, T4, T5, T6) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6& v6) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[7])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE8(cls, m, T0, T1, T2, T3, T4, T5, T6, T7) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7& v7) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[8])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATIC9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBER9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8 v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_STATICVALUE9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	static result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	static result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = cls::m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 

#define ASYNC_MEMBERVALUE9(cls, m, T0, T1, T2, T3, T4, T5, T6, T7, T8) \
	result_t ac_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public AsyncCall { public: \
		_t(void ** a) : AsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} \
	result_t cc_##m( \
		T0 v0, T1 v1, T2 v2, T3 v3, T4 v4, T5 v5, T6 v6, T7 v7, T8& v8) {\
	class _t : public CAsyncCall { public: \
		_t(void ** a) : CAsyncCall(a) {} \
		virtual void invoke() { \
			result_t hr = ((cls*)args[9])->m( \
				*(T0*) args[0], *(T1*) args[1], *(T2*) args[2], *(T3*) args[3], *(T4*) args[4], *(T5*) args[5], *(T6*) args[6], *(T7*) args[7], *(T8*) args[8], this); \
			if(hr != CALL_E_PENDDING)post(hr); } }; \
	result_t hr = m(v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL); \
	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC)return hr; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	_t ac(args); \
	ac.async(hr == CALL_E_LONGSYNC); \
	return ac.wait();} 
