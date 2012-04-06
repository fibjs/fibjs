/**
 * 
 */

var txt = [];

for(var i = 0; i < 10; i ++)
{
	gen_stub(i, false);
	gen_stub(i, true);
}

//print(txt.join('\n'));
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
	txt.push('			ac->hr = ' + (bInst ? '((cls*)ac->args['+(argn)+'])->' : 'cls::') + 'm( \\');

	s = '				';
	a = [];
	for(i = 0; i < argn; i ++)
		a.push('*(T'+i+'*) ac->args['+i+']');
	s += a.join(', ');
	s += '); \\'
	txt.push(s);

	txt.push('			ac->post(); } }; \\');

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

	txt.push('	q.put(&ac); \\\n	v8::Unlocker unlocker(isolate); \\');
	if(bInst)txt.push('	m_lock.lock(); \\\n	ac.weak.wait(); \\\n	m_lock.unlock(); \\');
	else txt.push('	ac.weak.wait(); \\');
	txt.push('	return ac.hr;}\n');
}
