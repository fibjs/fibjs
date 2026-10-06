# <%-declare.type === 'module' ? 'Module' : 'Object'%> <%-declare.name%>
<%-declare.doc.descript%>

<%-declare.doc.detail.join('\n')%>

<%if(declare.type === 'interface'){%>

## Inheritance
```dot
<%-dot%>
```

<%}%>

<%var last_member = '';

function def_value(v, o)
{
    if(v.value)
    {
        v = v.value;

        switch(v)
        {
        case 'v8::Undefined(isolate->m_isolate)':
            v = 'undefined';
            break;
        case 'v8::Object::New(isolate->m_isolate)':
            v = '{}';
            break;
        case 'v8::Array::New(isolate->m_isolate)':
            v = '[]';
            break;
        }
        return v;
    }

    if(Array.isArray(v.const))
        return v.const.join('.');

    return v.const;
}

function type_name(t)
{
    if(Array.isArray(t))
        return '(' + t.map(function(it){
            return (it.type ? it.type + ' ' : '') + it.name + (it.isarray ? '[]' : '');
        }).join(', ') + ')';

    return t;
}

function type_text(p)
{
    if(p.callback)
    {
        var ps = p.callback.params.map(function(cp){
            if(cp.type === '...' || !cp.type)
                return '...' + (cp.name && cp.name !== '...' ? cp.name : '');

            return type_text(cp) + ' ' + cp.name + (cp.isarray ? '[]' : '');
        }).join(', ');

        var s = 'Function(' + ps + ')';
        if(p.callback.ret)
            s += ' => ' + type_name(p.callback.ret);
        return s;
    }

    return p.type;
}

function member_output(title, test){
    var has = false;
    members.forEach(function(m){
     if(test(m, declare.name)){
         if(!has){
             has = true;%>## <%-title%>
        <%}else{%>--------------------------<%}
        if(last_member != m.name){%>
### <%-m.memType == 'operator'?'operator':''%><%-(m.memType === 'event' ? '' : m.symbol)+m.name%><%
last_member = m.name;
}%><%

// Handle method overloads if they exist
var methodsToProcess = m.overs || [m];
var isFirstOverload = true;

methodsToProcess.forEach(function(method) {
    if (!isFirstOverload) {%>

--------------------------<%
    }
    isFirstOverload = false;
%>
**<%-method.doc.descript%>**
```JavaScript
<%if(method.const){%><%-method.const%> <%}
if(method.static){%><%-method.static%> <%}
if(method.readonly){%><%-method.readonly%> <%}
if(method.callback){%><%-type_text(method)%> <%}else if(method.type){%><%-method.type%> <%}
if(method.memType === 'event'){%>event <%}
%><%-declare.name == method.name ? ' new ' : declare.name + (method.memType !== 'operator' ? '.' + (method.memType === 'event' ? '' : method.symbol) : '')%><%-method.name%><%
if(method.memType == 'method' || method.memType == 'event'){
    var ps = '';

    if(method.params){
        method.params.forEach(function(p){
            if(ps)
                ps += ',\n                ';

            if(p.type || p.callback)
                ps += type_text(p) + ' ';
            ps += p.name;
            if(p.isarray)
                ps += "[]";
    
            if(p.default)
                ps += ' = ' + def_value(p.default, p);
        });
    }%>(<%-ps%>)<% if(method.async){%> <%-method.async%><%}}else if(method.default){%> = <%-def_value(method.default, method)%><%}%>;
```
<%if(method.params){%>
Parameters:<% method.doc.params.forEach(function(p){%>
* <%-p.name%>: <%-p.descript%><%});%>
<%}%><%if(method.doc.return){%>
Returns:
* <%-method.doc.return.descript%><%}%>

<%-method.doc.detail.join('\n')%><%
}); // end methodsToProcess.forEach
%>

<%  }});
    }
    member_output('Constructors', function(m, n){
        return m.memType == 'method' && m.name == n;
    });

    member_output('Operators', function(m){
        return m.memType == 'operator' || m.symbol;
    });

    member_output('Objects', function(m){
        return m.memType == 'object';
    });

    member_output('Static Methods', function(m, n){
        return m.memType == 'method' && m.name !== n && m.static && !m.symbol;
    });

    member_output('Static Properties', function(m){
        return m.memType == 'prop' && m.static && !m.symbol;
    });

    member_output('Constants', function(m){
        return m.memType == 'const';
    });

    member_output('Properties', function(m){
        return m.memType == 'prop' && !m.static && !m.symbol;
    });

    member_output('Methods', function(m, n){
        return m.memType == 'method' && m.name !== n && !m.static && !m.symbol;
    });

    member_output('Events', function (m) {
        return m.memType == 'event';
    });

%>
