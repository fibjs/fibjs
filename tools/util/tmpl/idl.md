# <%-declare.type === 'module' ? '模块' : '对象'%> <%-declare.name%>
<%-declare.doc.descript%>

<%-declare.doc.detail.join('\n')%>

<%if(declare.type === 'interface'){%>

## 继承关系
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
if(method.type){%><%-method.type%> <%}
if(method.memType === 'event'){%>event <%}
%><%-declare.name == method.name ? ' new ' : declare.name + (method.memType !== 'operator' ? '.' + (method.memType === 'event' ? '' : method.symbol) : '')%><%-method.name%><%
if(method.memType == 'method' || method.memType == 'event'){
    var ps = '';

    if(method.params){
        method.params.forEach(function(p){
            if(ps)
                ps += ',\n                ';

            if(p.type)
                ps += p.type + ' ';
            ps += p.name;
            if(p.isarray)
                ps += "[]";
    
            if(p.default)
                ps += ' = ' + def_value(p.default, p);
        });
    }%>(<%-ps%>)<% if(method.async){%> <%-method.async%><%}}else if(method.default){%> = <%-def_value(method.default, method)%><%}%>;
```
<%if(method.params){%>
调用参数:<% method.doc.params.forEach(function(p){%>
* <%-p.name%>: <%-p.descript%><%});%>
<%}%><%if(method.doc.return){%>
返回结果:
* <%-method.doc.return.descript%><%}%>

<%-method.doc.detail.join('\n')%><%
}); // end methodsToProcess.forEach
%>

<%  }});
    }
    member_output('构造函数', function(m, n){
        return m.memType == 'method' && m.name == n;
    });

    member_output('操作符', function(m){
        return m.memType == 'operator' || m.symbol;
    });

    member_output('对象', function(m){
        return m.memType == 'object';
    });

    member_output('静态函数', function(m, n){
        return m.memType == 'method' && m.name !== n && m.static && !m.symbol;
    });

    member_output('静态属性', function(m){
        return m.memType == 'prop' && m.static && !m.symbol;
    });

    member_output('常量', function(m){
        return m.memType == 'const';
    });

    member_output('成员属性', function(m){
        return m.memType == 'prop' && !m.static && !m.symbol;
    });

    member_output('成员函数', function(m, n){
        return m.memType == 'method' && m.name !== n && !m.static && !m.symbol;
    });

    member_output('事件', function (m) {
        return m.memType == 'event';
    });

%>
