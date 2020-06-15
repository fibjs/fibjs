#ifdef __APPLE__

namespace fibjs {
const char* g_console_js = "!function(t){function o(t){return t<10?\"0\"+t:t}t.JSON||(t.JSON={}),\"function\"!=t"
"ypeof Date.prototype.toJSON&&(Date.prototype.toJSON=function(t){return isFinite("
"this.valueOf())?this.getUTCFullYear()+\"-\"+o(this.getUTCMonth()+1)+\"-\"+o(this.get"
"UTCDate())+\"T\"+o(this.getUTCHours())+\":\"+o(this.getUTCMinutes())+\":\"+o(this.getU"
"TCSeconds())+\"Z\":null},String.prototype.toJSON=Number.prototype.toJSON=Boolean.p"
"rototype.toJSON=function(t){return this.valueOf()})}(this),function(t){function "
"u(t){return null!==t&&\"object\"==typeof t&&\"function\"!=typeof t.toJSON&&function "
"t(o,e,n,r){{if(\"object\"!=typeof o||null===o)return;if(o instanceof i)return void"
" o.count++;if(\"function\"==typeof o.toJSON&&!o.toJSON.forceDecirc)return;if(r&&~n"
".indexOf(o))return void(r[e]=new i(o,e,r))}n.push(o);for(var e in o)t(o[e],e,n,o"
");n.pop()}(t,\"\",[],null),JSON.stringify(t)}function i(t,o,e){this.val=t,this.k=o"
",this.parent=e,this.count=1}function o(o){return function(){var t=[o].concat(Arr"
"ay.prototype.slice.call(arguments,0));return function(t,o){void 0===o&&(o=\"\"),o="
"o.toString();var i=Array.prototype.slice.call(arguments,2);i.length&&(o=o.replac"
"e(/(%?)(%([jds]))/g,function(t,o,e,n){var r=i.shift();switch(n){case\"s\":r=\"objec"
"t\"==typeof r&&\"function\"!=typeof r.toJSON?u(r):\"\"+r;break;case\"d\":r=Number(r);br"
"eak;case\"j\":r=u(r)}return o?(i.unshift(r),t):r}));for(var e=0;e<i.length;e++){va"
"r n=i[e];\"object\"==typeof n&&\"function\"!=typeof n.toJSON?o+=\" \"+u(n):o+=\" \"+n}o="
"o.replace(/%{2,2}/g,\"%\"),external.log(t,\"\"+o)}.apply(this,t)}}i.prototype.toJSON"
"=function(){return 0==--this.count&&(this.parent[this.k]=this.val),\"[Circular]\"}"
",t.console={debug:o(7),log:o(6),notice:o(5),warn:o(4),error:o(3),crit:o(2),alert"
":o(1),fatal:o(0)}}(this);";
}

#endif