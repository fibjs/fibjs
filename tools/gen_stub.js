#!/bin/js

var txt = [];

var fs = require('fs');
var path = require('path');

for (var i = 0; i < 10; i++) {
    gen_stub(i, false, false);
    gen_stub(i, true, false);

    gen_stub(i + 1, false, true);
    gen_stub(i + 1, true, true);
}

fs.writeFile(path.join(__dirname, '../fibjs/include/object_async.inl'), txt.join('\n'));

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
            s += (bCCall ? ', Isolate* isolate = NULL' : '') + ') {\\';
            txt.push(s);
        } else {
            txt.push((bInst ? '	' : '	static ') + 'result_t ' + ac + '_##m(' + (bCCall ? 'Isolate* isolate = NULL' : '') + ') { \\');
        }

        txt.push('	class _t : public ' + AsyncCall + ' \\\n	{ \\\n	public: \\');
        txt.push('		_t(void ** a' + (bCCall ? ', Isolate* isolate' : ', Isolate* isolate') + ') : ' +
            AsyncCall + '(a' + (bCCall ? ', isolate' : ', isolate') + ') {} \\');

        txt.push('		virtual void invoke() \\\n		{ \\\n			setAsync(); \\');
        txt.push('			result_t hr = ' + (bInst ? '((cls*)args[' + (argn) + '])->' : 'cls::') + 'm( \\');

        s = '				';
        a = [];
        for (i = 0; i < argn; i++)
            a.push('*(T' + i + '*) args[' + i + ']');
        a.push('this');
        s += a.join(', ');
        s += '); \\'
        txt.push(s);

        txt.push('			if(hr != CALL_E_PENDDING)post(hr); \\');
        txt.push('		} \\');

        txt.push('	}; \\');

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

            txt.push('	_t ac(args' + (bCCall ? ', isolate' : bInst ? ', holder()' : ', Isolate::current()') + '); \\');
        } else
            txt.push('	_t ac(NULL' + (bCCall ? ', isolate' : bInst ? ', holder()' : ', Isolate::current()') + '); \\');

        s = '	return ac.check_result(m(';
        if (argn > 0) {
            a = [];
            for (i = 0; i < argn; i++)
                a.push('v' + i);
            a.push('&ac');
            s += a.join(', ');
        } else
            s += '&ac';

        txt.push(s + ')); \\\n	}');
    }

    function gen_callback() {
        var s, i, a;

        var argn1 = argn;

        if (bRet)
            argn1--;

        if (argn1 > 0) {
            txt.push((bInst ? '	' : '	static ') + 'result_t acb_##m( \\');

            s = '		';
            a = [];
            for (i = 0; i < argn1; i++)
                a.push('T' + i + ' v' + i);

            s += a.join(', ');
            s += ', v8::Local<v8::Object> cb, const v8::FunctionCallbackInfo<v8::Value>& args) {\\';
            txt.push(s);
        } else
            txt.push((bInst ? '	' : '	static ') + 'result_t acb_##m(v8::Local<v8::Object> cb, const v8::FunctionCallbackInfo<v8::Value>& args) { \\');

        txt.push('	class _t: public AsyncCallBack { \\\n	public: \\');
        s = '		_t(' + (bInst ? 'cls* pThis, ' : '');
        for (i = 0; i < argn1; i++)
            s += 'T' + i + '& v' + i + ', ';
        s += 'v8::Local<v8::Object> cb) : \\';
        txt.push(s);

        s = '			AsyncCallBack(cb' + (bInst ? ', pThis' : '') + ')';
        for (i = 0; i < argn1; i++)
            s += ', m_v' + i + '(v' + i + ')';
        s += ' \\';
        txt.push(s);

        txt.push('		{} \\\n		virtual void invoke() \\\n		{ \\\n			setAsync(); \\');
        if (bInst)
            s = '			result_t hr = ((cls*)(object_base*)m_pThis)->m(';
        else
            s = '			result_t hr = cls::m(';

        for (i = 0; i < argn1; i++)
            s += 'm_v' + i + '.c_value(), ';
        if (bRet)
            s += 'retVal, ';
        s += 'this); \\';
        txt.push(s);

        txt.push('			if (hr != CALL_E_PENDDING)post(hr); \\\n' + '		} \\');

        if (bRet) {
            txt.push('    	virtual void to_args(std::vector<v8::Local<v8::Value>>& args) \\');
            txt.push('    	{ fillRetVal(args, retVal); } \\');

            txt.push('	public: \\\n		T' + argn1 + ' retVal; \\');
        }

        txt.push('	private: \\');

        for (i = 0; i < argn1; i++)
            txt.push('		_at<T' + i + '> m_v' + i + '; \\');

        txt.push('	}; \\');

        s = '	_t* ac = new _t(' + (bInst ? 'this, ' : '');
        for (i = 0; i < argn1; i++)
            s += 'v' + i + ', ';
        s += 'cb); \\';
        txt.push(s);

        s = '	return ac->check_result(m(';

        a = [];
        for (i = 0; i < argn1; i++)
            a.push('v' + i);

        if (bRet)
            a.push('ac->retVal');

        a.push('ac');
        s += a.join(', ');

        txt.push(s + '), args); \\\n	}');
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