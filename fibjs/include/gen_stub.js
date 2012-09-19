/**
 * 
 */

var txt = [];

var fs = require('fs');

for ( var i = 0; i < 10; i++) {
	gen_stub(i, false, false);
	gen_stub(i, true, false);
	if(i > 0)
	{
		gen_stub(i, false, true);
		gen_stub(i, true, true);
	}
	gen_callback(i, false);
	gen_callback(i, true);
}

// print(txt.join('\n'));
fs.writeFile('object_async.inl', txt.join('\n'));

function gen_stub(argn, bInst, bRet) {
	var s, i, a, fa, an = bInst ? 1 : 0;

	if (argn > 0) {
		s = '#define ASYNC_' + (bInst ? 'MEMBER' : 'STATIC') + (bRet ? "VALUE" : "") + argn
				+ '(cls, m, ';
		a = [];
		for (i = 0; i < argn; i++)
			a.push('T' + i);
		
		s += a.join(', ');
		s += ') \\';
		txt.push(s);

		txt.push((bInst ? '	' : '	static ') + 'result_t ac_##m( \\');

		s = '		';
		a = [];
		for (i = 0; i < argn; i++)
			a.push('T' + i + ' v' + i);

		if(bRet)
			a[i - 1] = 'T' + (i - 1) + '& v' + (i - 1);

		s += a.join(', ');
		s += ') {\\';
		txt.push(s);
	} else {
		txt.push('#define ASYNC_' + (bInst ? 'MEMBER' : 'STATIC' + (bRet ? "VALUE" : "")) + argn
				+ '(cls, m) \\');
		txt.push((bInst ? '	' : '	static ') + 'result_t ac_##m() { \\');
	}

	txt.push('	class _t : public AsyncCall { public: \\');
	txt.push('		_t(void ** a) : AsyncCall(a) {} \\');
	txt.push('		virtual void invoke() { \\');
	txt.push('			result_t hr = '
			+ (bInst ? '((cls*)args[' + (argn) + '])->' : 'cls::') + 'm( \\');

	s = '				';
	a = [];
	for (i = 0; i < argn; i++)
		a.push('*(T' + i + '*) args[' + i + ']');
	a.push('this');
	s += a.join(', ');
	s += '); \\'
	txt.push(s);

	txt.push('			if(hr != CALL_E_PENDDING)post(hr); } }; \\');

	s = '	result_t hr = m(';
	if (argn > 0) {
		a = [];
		for (i = 0; i < argn; i++)
			a.push('v' + i);
		a.push('NULL');
		s += a.join(', ');
	} else
		s += 'NULL';

	txt.push(s + '); \\\n' + '	if(hr != CALL_E_NOSYNC)return hr; \\');

	if (argn > 0 || bInst) {
		s = '	void* args[] = {';
		if (argn > 0) {
			a = [];
			for (i = 0; i < argn; i++)
				a.push('&v' + i);
			s += a.join(', ');
			if (bInst)
				s += ', ';
		}
		s += bInst ? 'this}; \\' : '}; \\';
		txt.push(s);

		txt.push('	_t ac(args); \\');
	} else
		txt.push('	_t ac(NULL); \\');

	txt.push('	s_acPool.put(&ac); \\\n	return ac.wait();}\n');

}

function gen_callback(argn, bRet) {
	if (bRet)
		txt.push('#define ASYNC_VALUEBACK' + argn + '(cls, m, rt) \\');
	else
		txt.push('#define ASYNC_CALLBACK' + argn + '(cls, m) \\');

	if (argn > 0) {
		s = '	template<';
		a = [];
		for (i = 0; i < argn; i++)
			a.push('typename T' + i);
		s += a.join(', ');
		s += '> \\';
		txt.push(s);

		txt.push('	void acb_##m( \\');

		s = '		';
		a = [];
		for (i = 0; i < argn; i++)
			a.push('T' + i + '& v' + i);
		s += a.join(', ');
		s += ', const char* ev = #m) {\\';
		txt.push(s);
	} else
		txt.push('	void acb_##m(const char* ev = #m) { \\');

	txt.push('	class _t: public AsyncCallBack { \\\n	public: \\');
	s = '		_t(cls* pThis';
	for (i = 0; i < argn; i++)
		s += ', T' + i + '& v' + i;
	s += ', const char* ev) : \\';
	txt.push(s);

	s = '			AsyncCallBack(pThis, NULL)';
	for (i = 0; i < argn; i++)
		s += ', m_v' + i + '(v' + i + ')';
	s += ', m_ev(ev) \\';
	txt.push(s);

	txt.push('		{} \\\n		virtual void invoke() \\\n		{ \\');
	s = '			result_t hr = ((cls*)(object_base*)m_pThis)->m(';
	for (i = 0; i < argn; i++)
		s += 'm_v(m_v' + i + '), ';
	if (bRet)
		s += 'retVal, ';
	s += 'this); \\';
	txt.push(s);

	txt.push('			if (hr != CALL_E_PENDDING)post(hr); \\\n' + '		} \\\n'
			+ '		virtual void js_callback() \\');

	if (bRet)
		txt.push('		{ _trigger(m_ev, retVal); }\\');
	else
		txt.push('		{ _trigger(m_ev); }\\');

	txt.push('	private: \\');
	if (bRet)
		txt.push('		rt retVal; \\');

	for (i = 0; i < argn; i++)
		txt.push('		T' + i + ' m_v' + i + '; \\');

	txt.push('		const char* m_ev; \\\n	}; \\');

	s = '	s_acPool.put(new _t(this';
	for (i = 0; i < argn; i++)
		s += ', v' + i;
	s += ', ev)); \\';
	txt.push(s);

	txt.push('	}\n');
}
