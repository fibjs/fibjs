/**
 * 
 */

var txt = [];

var fs = require('fs');

for(var i = 0; i < 10; i ++)
{
	gen_stub(i, false);
	gen_stub(i, true);
	gen_callback(i, false);
	gen_callback(i, true);
}

// print(txt.join('\n'));
fs.writeFile('object_async.inl', txt.join('\n'));

function gen_stub(argn, bInst)
{
	var s, i, a, fa, an = bInst ? 1 : 0;

	txt.push('#define ASYNC_' + (bInst ? 'MEMBER' : 'STATIC') +argn+'(cls, m) \\');
	
	if(argn > 0)
	{
		s = 'template<';
		a = [];
		for(i = 0; i < argn; i ++)
			a.push('typename T' + i);
		s += a.join(', '); 
		s += '> \\';
		txt.push(s);

		txt.push((bInst ? '	' : '	static ')+'result_t ac_##m(AsyncQueue& q, \\');
		
		s = '		';
		a = [];
		for(i = 0; i < argn; i ++)
			a.push('T' + i + '& v' + i);
		s += a.join(', ');
		s += ') {\\';
		txt.push(s);
	}else
		txt.push((bInst ? '	' : '	static ')+'result_t ac_##m(AsyncQueue& q) { \\');

	txt.push('	class _t { public: \\');
	txt.push('		static void _stub(AsyncCall* ac) { \\');
	txt.push('			result_t hr = ' + (bInst ? '((cls*)ac->args['+(argn)+'])->' : 'cls::') + 'm( \\');

	s = '				';
	a = [];
	for(i = 0; i < argn; i ++)
		a.push('*(T'+i+'*) ac->args['+i+']');
	a.push('ac');
	s += a.join(', ');
	s += '); \\'
	txt.push(s);

	txt.push('			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \\');

	if(argn > 0 || bInst)
	{
		s = '	void* args[] = {';
		if(argn > 0)
		{
			a = [];
			for(i = 0; i < argn; i ++)
				a.push('&v' + i);
			s += a.join(', ');
			if(bInst) s+= ', ';
		}
		s += bInst ? 'this}; \\' : '}; \\';
		txt.push(s);

		txt.push('	AsyncCall ac(args, _t::_stub); \\');
	}else
		txt.push('	AsyncCall ac(NULL, _t::_stub); \\');

	txt.push('	q.put(&ac); \\\n	return ac.wait();}\n');

}

function gen_callback(argn, bRet)
{
	if(bRet)
		txt.push('#define ASYNC_VALUEBACK' +argn+'(cls, m, rt) \\');
	else
		txt.push('#define ASYNC_CALLBACK' +argn+'(cls, m) \\');

	if(argn > 0)
	{
		s = '	template<';
		a = [];
		for(i = 0; i < argn; i ++)
			a.push('typename T' + i);
		s += a.join(', '); 
		s += '> \\';
		txt.push(s);

		txt.push('	void acb_##m(AsyncQueue& q, \\');
		
		s = '		';
		a = [];
		for(i = 0; i < argn; i ++)
			a.push('T' + i + '& v' + i);
		s += a.join(', ');
		s += ') {\\';
		txt.push(s);
	}else
		txt.push('	void acb_##m(AsyncQueue& q) { \\');

	txt.push('	class _t: public AsyncCallBack { \\\n	public: \\');
	s = '		_t(cls* pThis';
	for(i = 0; i < argn; i ++)
		s += ', T' + i + '& v' + i;
	s += ') : \\';
	txt.push(s);

	s = '			AsyncCallBack(pThis, NULL, _stub)';
	for(i = 0; i < argn; i ++)
		s += ', m_v' + i + '(v' + i + ')';
	s += ' \\';
	txt.push(s);

	txt.push('		{} \\\n		static void _stub(AsyncCall* ac) \\\n		{	_t* t = (_t*) ac; \\');
	s = '			result_t hr = ((cls*)(object_base*)t->m_pThis)->m(';
	for(i = 0; i < argn; i ++)
		s += 'm_v(t->m_v' + i + '), ';
	if(bRet)
		s += 't->retVal, ';
	s += 't); \\';
	txt.push(s);

	txt.push('			if (hr != CALL_E_PENDDING)t->post(hr); \\\n' +
			'		} \\\n' +
			'		virtual bool clear_value() \\\n' +
			'		{	bool b = true; \\');

	for(i = 0; i < argn; i ++)
		txt.push('			b = b && c_v(m_v' + i + '); \\');
	if(bRet)
		txt.push('			b = b && c_v(retVal); \\');
	
	txt.push('			b = b && c_v(m_pThis); \\\n' +
			'			return b; \\\n' +
			'		} \\\n' +
			'		virtual void callback() \\');
	
	if(bRet)
		txt.push('		{ _trigger(#m, retVal); }\\');
	else
		txt.push('		{ _trigger(#m); }\\');

	txt.push('	private: \\');
	if(bRet)
		txt.push('		rt retVal; \\');
	
	for(i = 0; i < argn; i ++)
		txt.push('		T' + i +' m_v' + i + '; \\');
	
	txt.push('	}; \\');

	s = '	q.put(new _t(this';
	for(i = 0; i < argn; i ++)
		s += ', v' + i;
	s += ')); \\';
	txt.push(s);
	
	txt.push('	}\n');
}
