#!/bin/js

var txt = [];

var fs = require('fs');
var path = require('path');

for (var i = 0; i < 10; i++) {
    gen_stub(i, false, false);
    gen_stub(i, true, false);

    gen_stub(i + 1, false, true);
    gen_stub(i + 1, true, true);

    // `V` family: the same void bridges, renamed to acv_/ccv_/acbv_. A
    // typeless overload sitting in a group next to a VALUE overload of the same
    // arity would otherwise emit an identical C++ name (`ac_text(exlib::string)`
    // vs `ac_text(exlib::string&)`), and every call with an lvalue would be
    // ambiguous. Only mixed groups use this family (see gen_code.js).
    gen_stub(i, false, false, true);
    gen_stub(i, true, false, true);

    // `PICK` family: the typed counterparts of a mixed group, which reach the
    // target through a cast that selects the overload by signature. Same reason,
    // other side of the same ambiguity.
    gen_stub(i + 1, false, true, false, true);
    gen_stub(i + 1, true, true, false, true);
}

fs.writeFile(path.join(__dirname, '../fibjs/include/object_async.inl'), txt.join('\n'));

function gen_stub(argn, bInst, bRet, bVoidTag, bPick) {
    var group = [];
    var tag = bVoidTag ? 'V' : (bPick ? 'PICK' : '');
    var pfx = bVoidTag ? 'v' : (bPick ? 'p' : '');

    // A typeless overload inside a mixed group shares its parameter list with the
    // typed overload's retVal parameter (`text(exlib::string, AsyncHandle)` next
    // to `text(exlib::string& retVal, AsyncHandle)`), so every pass-through lvalue
    // would be ambiguous.
    //
    // The V family copies the argument into a prvalue instead (the by-value
    // binding it already does, minus the reference candidate). The PICK family
    // goes the other way and selects the typed overload by signature, through a
    // pointer-to-member/function-pointer cast.
    var pick = null;

    if (bPick) {
        var sig = [];
        for (var n = 0; n < argn; n++)
            sig.push('T' + n + ((bRet && n == argn - 1) ? '&' : ''));
        sig.push('AsyncHandle');
        pick = 'static_cast<result_t ' + (bInst ? '(cls::*)(' : '(*)(') + sig.join(', ') + ')>(&cls::m)';
    }

    // the callee expression that reaches the target method. `obj` is a fully
    // parenthesised object expression, or NULL for a static target
    function callee(obj) {
        if (!bPick)
            return obj ? obj + '->m' : (obj === null ? 'm' : 'cls::m');

        return obj ? '((*' + obj + ').*' + pick + ')' : '((*' + pick + '))';
    }

    function callee_obj() {
        return bInst ? '((cls*)args[' + argn + '])' : null;
    }

    function arg(i) {
        return bVoidTag ? '(T' + i + ') v' + i : 'v' + i;
    }

    function deref(i) {
        return bVoidTag ? '(T' + i + ') *(T' + i + '*) args[' + i + ']' : '*(T' + i + '*) args[' + i + ']';
    }

    function cval(i) {
        return bVoidTag ? '(T' + i + ') m_v' + i + '.c_value()' : 'm_v' + i + '.c_value()';
    }

    function gen_define(type) {
        var s, i, a;

        if (argn > 0) {
            s = 'ASYNC_' + (bInst ? 'MEMBER' : 'STATIC') + (bRet ? "VALUE" : "") + argn + tag + type + '(cls, m, ';
            a = [];
            for (i = 0; i < argn; i++)
                a.push('T' + i);

            s += a.join(', ');
            s += ')';
        } else
            s = 'ASYNC_' + (bInst ? 'MEMBER' : 'STATIC' + (bRet ? "VALUE" : "")) + argn + tag + type + '(cls, m)';

        txt.push('#define ' + s + ' \\');
        group.push('	' + s)
    }

    function gen_code(bCCall) {
        var s, i, a;
        var ac = (bCCall ? "cc" : "ac") + pfx;
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
        txt.push('			result_t hr = ' + (bPick ? callee(callee_obj()) : (bInst ? callee_obj() + '->m' : 'cls::m')) + '( \\');

        s = '				';
        a = [];
        for (i = 0; i < argn; i++)
            a.push(deref(i));
        a.push('this');
        s += a.join(', ');
        s += '); \\'
        txt.push(s);

        txt.push('			if(!hr.is_pending())post(hr); \\');
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

        if (bCCall) {
            txt.push('	return ac.check_result(result_t::nosync()); \\\n	}');
        } else {
            s = '\treturn ac.check_result(' + (bPick ? callee(bInst ? 'this' : null) : 'm') + '(';
            if (argn > 0) {
                a = [];
                for (i = 0; i < argn; i++)
                    a.push(arg(i));
                a.push('&ac');
                s += a.join(', ');
            } else
                s += '&ac';

            txt.push(s + ')); \\\n	}');
        }
    }

    function gen_callback() {
        var s, i, a;

        var argn1 = argn;

        if (bRet)
            argn1--;

        if (argn1 > 0) {
            txt.push((bInst ? '	' : '	static ') + 'result_t acb' + pfx + '_##m( \\');

            s = '		';
            a = [];
            for (i = 0; i < argn1; i++)
                a.push('T' + i + ' v' + i);

            s += a.join(', ');
            s += ', v8::Local<v8::Object> cb, const v8::FunctionCallbackInfo<v8::Value>& args) {\\';
            txt.push(s);
        } else
            txt.push((bInst ? '	' : '	static ') + 'result_t acb' + pfx + '_##m(v8::Local<v8::Object> cb, const v8::FunctionCallbackInfo<v8::Value>& args) { \\');

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
            s = '\t\t\tresult_t hr = ' + (bPick ? callee('((cls*)(object_base*)m_pThis)') : '((cls*)(object_base*)m_pThis)->m') + '(';
        else
            s = '\t\t\tresult_t hr = ' + (bPick ? callee(null) : 'cls::m') + '(';

        for (i = 0; i < argn1; i++)
            s += cval(i) + ', ';
        if (bRet)
            s += 'retVal, ';
        s += 'this); \\';
        txt.push(s);

        txt.push('			if (!hr.is_pending())post(hr); \\\n' + '		} \\');

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

        s = '\treturn ac->check_result(' + (bPick ? callee(bInst ? 'this' : null) : 'm') + '(';

        a = [];
        for (i = 0; i < argn1; i++)
            a.push(arg(i));

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