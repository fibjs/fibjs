#ifdef __APPLE__

namespace fibjs {
const wchar_t* g_console_js = L"!function(t){function c(t){return null!==t&&\"object\"==typeof t&&\"function\"!=type"
"of t.toJSON&&function t(n,o,r,e){{if(\"object\"!=typeof n||null===n)return;if(n in"
"stanceof i)return void n.count++;if(\"function\"==typeof n.toJSON&&!n.toJSON.force"
"Decirc)return;if(e&&~r.indexOf(n))return void(e[o]=new i(n,o,e))}r.push(n);for(v"
"ar o in n)t(n[o],o,r,n);r.pop()}(t,\"\",[],null),JSON.stringify(t)}function i(t,n,"
"o){this.val=t,this.k=n,this.parent=o,this.count=1}function n(n){return function("
"){var t=[n].concat(Array.prototype.slice.call(arguments,0));return function(t,n)"
"{void 0===n&&(n=\"\"),n=n.toString();var i=Array.prototype.slice.call(arguments,2)"
";i.length&&(n=n.replace(/(%?)(%([jds]))/g,function(t,n,o,r){var e=i.shift();swit"
"ch(r){case\"s\":e=\"object\"==typeof e&&\"function\"!=typeof e.toJSON?c(e):\"\"+e;break;"
"case\"d\":e=Number(e);break;case\"j\":e=c(e)}return n?(i.unshift(e),t):e}));for(var "
"o=0;o<i.length;o++){var r=i[o];\"object\"==typeof r&&\"function\"!=typeof r.toJSON?n"
"+=\" \"+c(r):n+=\" \"+r}n=n.replace(/%{2,2}/g,\"%\"),external.log(t,\"\"+n)}.apply(this,"
"t)}}i.prototype.toJSON=function(){return 0==--this.count&&(this.parent[this.k]=t"
"his.val),\"[Circular]\"},t.console={debug:n(7),log:n(6),notice:n(5),warn:n(4),erro"
"r:n(3),crit:n(2),alert:n(1),fatal:n(0)}}(this);";
const wchar_t* script_regExternal = L"!function(){window.external=this,Object.defineProperty(window,\"postMessage\",{wri"
"table:!1,value:function(e){webkit.messageHandlers.invoke.postMessage(e)}})}();";
const wchar_t* script_inwardPostMessage = L"window.addEventListener(\"load\",function(){webkit.messageHandlers.__inward.postMe"
"ssage(\"inward:window.load\")});";
}

#endif