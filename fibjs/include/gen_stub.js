/**
 * 
 */

var txt = [];

var fs = require('fs');

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
	a.push('ac');
	s += a.join(', ');
	s += '); \\'
	txt.push(s);

	txt.push('			if(ac->hr != CALL_E_PENDDING)ac->post(); } }; \\');

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

		txt.push('	AsyncCall ac(q, args, _t::_stub); \\');
	}else
		txt.push('	AsyncCall ac(q, NULL, _t::_stub); \\');

	txt.push('	return ac.hr;}\n');
}
