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
"+=\" \"+c(r):n+=\" \"+r}n=n.replace(/%{2,2}/g,\"%\"),window.external.log(t,\"\"+n)}.appl"
"y(this,t)}}i.prototype.toJSON=function(){return 0==--this.count&&(this.parent[th"
"is.k]=this.val),\"[Circular]\"},t.console={debug:n(7),log:n(6),info:n(6),notice:n("
"5),warn:n(4),error:n(3),crit:n(2),alert:n(1),fatal:n(0)}}(this);";
const wchar_t* scriptStart_regExternal = L"!function(){var e=window.external={};e.onmessage=e.onmessage||function(){},e.onc"
"lose=e.onclose||function(){},Object.defineProperty(e,\"postMessage\",{writable:!1,"
"value:function(e){window.webkit.messageHandlers.invoke.postMessage(e)}}),Object."
"defineProperty(e,\"log\",{writable:!1,value:function(e,n){window.webkit.messageHan"
"dlers.__externalLog.postMessage(JSON.stringify({level:e,fmt:n}))}}),Object.defin"
"eProperty(window,\"external\",{writable:!1,configurable:!1,value:e}),window.onerro"
"r=function(e){var n=e+\"\\n    at \"+new Error(e).stack;console.error(n)};var n=win"
"dow.close.bind(window);Object.defineProperty(window,\"close\",{writable:!1,configu"
"rable:!1,value:function(){n.apply(window,arguments),window.webkit.messageHandler"
"s.__inward.postMessage(\"inward:window.doClose\")}})}();";
const wchar_t* scriptEnd_inwardPostMessage = L"window.addEventListener(\"load\",function(){window.webkit.messageHandlers.__inward"
".postMessage(\"inward:window.onLoad\")});";
const wchar_t* scriptEnd_afterInternal = L"webkit.messageHandlers.__inward.postMessage(\"inward:internal-script-loaded\");";
}

#endif