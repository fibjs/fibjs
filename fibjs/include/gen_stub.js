#!/bin/js

var txt = [];

var fs = require('fs');

for (var i = 0; i < 10; i++) {
	gen_stub(i, false, false);
	gen_stub(i, true, false);

	gen_stub(i + 1, false, true);
	gen_stub(i + 1, true, true);
}

fs.writeFile('object_async.inl', txt.join('\n'));

function gen_stub(argn, bInst, bRet) {
	var group = [];

	function gen_define(type) {
		var s, i, a;

		if (argn > 0) {
			s = 'ASYNC_' + (bInst ? 'MEMBER' : 'STATIC') + (bRet ? "VALUE" : "") + argn + type + '(cls, m, ';
			a = [];
			for (i = 0; i < argn; i++)
				a.push('T' + i);

			s += a.join(', ');
			s += ')';
		} else
			s = 'ASYNC_' + (bInst ? 'MEMBER' : 'STATIC' + (bRet ? "VALUE" : "")) + argn + type + '(cls, m)';

		txt.push('#define ' + s + ' \\');
		group.push('	' + s)
	}

	function gen_code(bCCall) {
		var s, i, a;
		var ac = bCCall ? "cc" : "ac";
		var AsyncCall = bCCall ? "CAsyncCall" : "AsyncCall";

		if (argn > 0) {
			txt.push((bInst ? '	' : '	static ') + 'result_t ' + ac + '_##m( \\');

			s = '		';
			a = [];
			for (i = 0; i < argn; i++)
				a.push('T' + i + ' v' + i);

			if (bRet)
				a[i - 1] = 'T' + (i - 1) + '& v' + (i - 1);

			s += a.join(', ');
			s += ') {\\';
			txt.push(s);
		} else {
			txt.push((bInst ? '	' : '	static ') + 'result_t ' + ac + '_##m() { \\');
		}

		txt.push('	class _t : public ' + AsyncCall + ' { public: \\');
		txt.push('		_t(void ** a) : ' + AsyncCall + '(a) {} \\');
		txt.push('		virtual void invoke() { \\');
		txt.push('			result_t hr = ' + (bInst ? '((cls*)args[' + (argn) + '])->' : 'cls::') + 'm( \\');

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

		txt.push(s + '); \\\n' + '	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC && hr != CALL_E_GUICALL)return hr; \\');

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

		txt.push('	if(hr != CALL_E_NOSYNC){ac.async(hr); return ac.wait();} \\\n	else return ac.async_wait(); \\\n	}');
	}

	function gen_callback() {
		var s, i, a;

		var argn1 = argn;

		if (bRet)
			argn1--;

		if (argn1 > 0) {
			txt.push((bInst ? '	' : '	static ') + 'void acb_##m( \\');

			s = '		';
			a = [];
			for (i = 0; i < argn1; i++)
				a.push('T' + i + ' v' + i);

			s += a.join(', ');
			s += ', v8::Local<v8::Function> cb) {\\';
			txt.push(s);
		} else
			txt.push((bInst ? '	' : '	static ') + 'void acb_##m(v8::Local<v8::Function> cb) { \\');

		txt.push('	class _t: public AsyncCallBack { \\\n	public: \\');
		s = '		_t(' + (bInst ? 'cls* pThis, ' : '');
		for (i = 0; i < argn1; i++)
			s += 'T' + i + '& v' + i + ', ';
		s += 'v8::Local<v8::Function> cb) : \\';
		txt.push(s);

		s = '			AsyncCallBack(cb)' + (bInst ? ', m_pThis(pThis)' : '');
		for (i = 0; i < argn1; i++)
			s += ', m_v' + i + '(v' + i + ')';
		s += ' \\';
		txt.push(s);

		txt.push('		{} \\\n		virtual void invoke() \\\n		{ \\');
		if (bInst)
			s = '			result_t hr = m_pThis->m(';
		else
			s = '			result_t hr = cls::m(';

		for (i = 0; i < argn1; i++)
			s += 'm_v' + i + '.value(), ';
		if (bRet)
			s += 'retVal, ';
		s += 'this); \\';
		txt.push(s);

		txt.push('			if (hr != CALL_E_PENDDING)post(hr); \\\n' + '		} \\');

		if (bRet) {
			txt.push('    virtual v8::Local<v8::Value> getValue() \\');
			txt.push('    {   return GetReturnValue(isolate()->m_isolate, retVal); } \\');

			txt.push('	public: \\\n		T' + argn1 + ' retVal; \\');
		}

		txt.push('	private: \\');
		if (bInst)
			txt.push('		obj_ptr<cls> m_pThis; \\');

		for (i = 0; i < argn1; i++)
			txt.push('		_at<T' + i + '> m_v' + i + '; \\');

		txt.push('	}; \\');

		s = '	_t* ac = new _t(' + (bInst ? 'this, ' : '');
		for (i = 0; i < argn1; i++)
			s += 'v' + i + ', ';
		s += 'cb); \\';
		txt.push(s);

		s = '	result_t hr = m(';

		a = [];
		for (i = 0; i < argn1; i++)
			a.push('v' + i);

		if (bRet)
			a.push('ac->retVal');

		a.push('NULL');
		s += a.join(', ');

		txt.push(s + '); \\');

		txt.push('	if(hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC && hr != CALL_E_GUICALL)ac->post(hr); \\');
		txt.push('	else ac->async(hr); \\');

		txt.push('	}');
	}

	gen_define('_AC');
	gen_code(false);
	gen_define('_CC');
	gen_code(true);
	gen_define('_ACB');
	gen_callback();
	var def_group = group.join(' \\\n');
	gen_define('');
	txt.push(def_group);
	txt.push('');
}