#!/bin/js

var txt = [];

var fs = require('fs');

for (var i = 0; i < 10; i++) {
	gen_stub(i, false, false);
	gen_stub(i, true, false);

	if (i > 0) {
		gen_stub(i, false, true);
		gen_stub(i, true, true);
	}
}

// print(txt.join('\n'));
fs.writeFile('object_async.inl', txt.join('\n'));

function gen_stub(argn, bInst, bRet, bCCall) {
	var s, i, a, fa, an = bInst ? 1 : 0;

	function gen_code(bCCall) {
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

		txt.push('	ac.async(); \\\n	return ac.wait();} ' + (bCCall ? '\n' : '\\'));
	}

	if (argn > 0) {
		s = '#define ASYNC_' + (bInst ? 'MEMBER' : 'STATIC') + (bRet ? "VALUE" : "") + argn + '(cls, m, ';
		a = [];
		for (i = 0; i < argn; i++)
			a.push('T' + i);

		s += a.join(', ');
		s += ') \\';
		txt.push(s);
	} else {
		txt.push('#define ASYNC_' + (bInst ? 'MEMBER' : 'STATIC' + (bRet ? "VALUE" : "")) + argn + '(cls, m) \\');
	}

	gen_code(false);
	gen_code(true);
}