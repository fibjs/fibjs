(function (w) {
	"use strict";
	
	var ArrayProto = Array.prototype;
	var arrfn = {
		every : ArrayProto.every || function (fun) {
			if (this === null)
				throw new TypeError();
			
			var t = Object(this),
			len = t.length >>> 0;
			
			if (typeof fun !== "function")
				throw new TypeError();
			
			var thisp = arguments[1];
			for (var i = 0; i < len; i++)
				if (!fun.call(thisp, t[i], i, t))
					return false;
			
			return true;
		},
		forEach : ArrayProto.forEach || function (fun, thisArg) {
			if (this === null)
				throw new TypeError();
			
			var t = Object(this),
			len = t.length >>> 0;
			
			if (typeof fun !== "function")
				throw new TypeError();
			
			var thisp = arguments[1];
			for (var i = 0; i < len; i++)
				fun.call(thisArg, t[i], i, t);
		},
		filter : ArrayProto.filter || function (fun) {
			if (this == null)
				throw new TypeError();
			
			var t = Object(this),
			len = t.length >>> 0;
			
			if (typeof fun != "function")
				throw new TypeError();
			
			var res = [],
			thisp = arguments[1];
			
			for (var i = 0; i < len; i++) {
				var val = t[i];
				if (fun.call(thisp, val, i, t))
					res.push(val);
			}
			
			return res;
		},
		map : Array.prototype.map || function (fun, thisArg) {
			var T,
			A;
			
			if (this == null)
				throw new TypeError("this is null or not defined");
			
			var O = Object(this);
			var len = O.length >>> 0;
			
			if (typeof fun != "function")
				throw new TypeError();
			
			if (thisArg)
				T = thisArg;
			
			A = new Array(len);
			
			for (var i = 0; i < len; i++) {
				var kValue,
				mappedValue;
				
				kValue = O[i];
				mappedValue = fun.call(T, kValue, i, O);
				A[i] = mappedValue;
			}
			return A;
		},
		remove : function (v) {
			var len = this.length;
			
			for (var i = 0; i < len; i++)
				if (this[i] === v) {
					this.splice(i, 1);
					return true;
				}
			
			return false;
		},
		reduce : function (fun) {
			if (this === void 0 || this === null)
				throw new TypeError();
			
			var t = Object(this),
			len = t.length >>> 0,
			k = 0,
			accumulator;
			
			if (typeof fun != 'function')
				throw new TypeError();
			if (len == 0 && arguments.length == 1)
				throw new TypeError();
			
			if (arguments.length >= 2)
				accumulator = arguments[1];
			else
				do {
					if (k in t) {
						accumulator = t[k++];
						break;
					}
					if (++k >= len)
						throw new TypeError();
				} while (true);
			while (k < len) {
				if (k in t)
					accumulator = fun.call(undefined, accumulator, t[k], k, t);
				k++;
			}
			return accumulator;
		},
		slice : ArrayProto.slice
	};
	
	for (var fn in arrfn)
		if (!(fn in ArrayProto))
			ArrayProto[fn] = arrfn[fn];
	
	var slice = ArrayProto.slice;
	
	if (!Function.prototype.bind) {
		Function.prototype.bind = function (oThis) {
			if (typeof this !== "function")
				throw new TypeError("Function.prototype.bind - what is trying to be bound is not callable");
			
			var aArgs = slice.call(arguments, 1),
			fToBind = this;
			
			return function () {
				return fToBind.apply(oThis, aArgs.concat(slice.call(arguments)));
			};
		};
	}
	
	if (!String.prototype.trim)
		String.prototype.trim = function () {
			return this.replace(/^\s+/, '').replace(/\s+$/, '');
		};
		
	if (!Object.keys) {
		Object.keys = (function() {
			var hasOwnProperty = Object.prototype.hasOwnProperty, hasDontEnumBug = !({
				toString : null
			}).propertyIsEnumerable('toString'), dontEnums = [
					'toString', 'toLocaleString', 'valueOf', 'hasOwnProperty',
					'isPrototypeOf', 'propertyIsEnumerable', 'constructor'
			], dontEnumsLength = dontEnums.length;

			return function(obj) {
				if (typeof obj !== 'object' && typeof obj !== 'function'
						|| obj === null)
					throw new TypeError('Object.keys called on non-object');

				var result = [];

				for ( var prop in obj) {
					if (hasOwnProperty.call(obj, prop))
						result.push(prop);
				}

				if (hasDontEnumBug) {
					for ( var i = 0; i < dontEnumsLength; i++) {
						if (hasOwnProperty.call(obj, dontEnums[i]))
							result.push(dontEnums[i]);
					}
				}
				return result;
			}
		})()
	};		

	var g = w.jModo = function (id) {
		var o;
		
		if (typeof id === 'string') {
			o = document.getElementById(id);
			if (!o)
				return o;
		} else
			o = id;
		
		if (!o)
			return o;
		
		if (o["#fn#"] !== fn["#fn#"])
			for (var m in fn) {
				var v1 = undefined,
				v2 = fn[m];
				
				try {
					v1 = o[m];
				} catch (e) {
				}

				if (!v1)
					try {
						o[m] = fn[m];
					} catch (e) {
					}
			}
		
		return o;
	}
	
	g.arrfn = arrfn;
	
	var fn = g.fn = {
		"#fn#" : 1
	};
	
	fn.extend = g.extend = function (o) {
		if ("#fn#" in this)
			this["#fn#"] = this["#fn#"] + 1;
		
		if (typeof o === 'object')
			for (var m in o)
				if (o.hasOwnProperty(m)) {
					var v1 = this[m],
					v2 = o[m];
					
					if (v1 !== v2)
						this[m] = o[m];
				} else
					throw new TypeError();
		
		return this;
	};
	
	function old_child_node(n, id) {
		var n1;
		
		for (var c = n.firstChild; c; c = c.nextSibling) {
			if (c.id == id)
				return c;
			n1 = old_child_node(c, id);
			if (n1)
				return n1;
		}
	}
	
	function fnProxy(f) {
		return function () {
			var len = this.length;
			for (var i = 0; i < len; i++)
				f.apply(g(this[i]), arguments);
		}
	}
	
	var clsfn = g.clsfn = {
		extend : g.extend,
		toString : function () {
			return "[jModo Object]";
		}
	};
	
	g.extend({
		array : function (o) {
			if (!o)
				return o;
			
			o.extend = g.extend;
			o.extend(g.arrfn);
			
			if (o["#fn#"] !== fn["#fn#"])
				for (var m in fn)
					if (!(m in o))
						o[m] = fnProxy(fn[m]);
			
			var len = o.length;
			for (var i = 0; i < len; i++)
				g(o[i]);
			
			return o;
		},
		Class : function (base, ext) {
			if (this instanceof g.Class)
				throw new Error('Operation is not allowed.');
			
			if (ext === undefined) {
				ext = base;
				base = undefined;
			} else if (!base)
				throw new Error('base is null.');
			else if (!("_super" in base && "_proto" in base))
				throw new Error('Operation is not allowed.');
			
			function Class() {
				if (!(this instanceof Class))
					throw new Error('Operation is not allowed.');
				
				var o = this,
				_inits = [];
				
				function construct(cls) {
					var _proto = cls._proto,
					_super = cls._super;
					
					if (_super !== undefined)
						construct(_super);
					else
						for (var m in clsfn)
							o[m] = clsfn[m];
					
					if (typeof(_proto) === 'object')
						o.extend(_proto);
					else if (typeof(_proto) === 'function')
						_proto.call(o);
					
					if (o._init) {
						_inits.push(o._init);
						delete o._init;
					}
				}
				
				construct(Class);
				o.constructor = Class;
				
				var as = arguments;
				_inits.forEach(function (fn) {
					fn.apply(o, as);
				});
				
				return o;
			}
			
			Class._super = base;
			Class._proto = ext;
			
			return Class;
		}
	});
	
	g.Object = g.Class({});
	
	g.fn.extend({
		child : function (id) {
			if (!document.querySelectorAll)
				return g(old_child_node(this, id));
			
			var n1;
			var old = this.getAttribute('id');
			
			this.setAttribute('id', 'jModo_current_node');
			
			n1 = document.querySelectorAll("[id='jModo_current_node'] #" + id);
			
			this.setAttribute('id', old);
			if (!old)
				this.removeAttribute("id");
			
			return g(n1[0]);
		},
		tag : function (n) {
			return g.array(this.getElementsByTagName(n));
		},
		append : function (v) {
			this.appendChild(v);
		},
		remove : function () {
			if (this.off())
				this.off();
			
			var p = this.parentNode;
			if (p)
				p.removeChild(this);
			
			if (this.clearAttributes) {
				this.clearAttributes();
			} else {
				for (var p in this)
					try {
						delete this[p];
					} catch (e) {}
			}
		},
		valid : function () {
			var o = this;
			while (o = o.parentNode) {
				if (o === document) {
					return true;
				}
			}
			return false;
		},
		html : function (ht) {
			this.innerHTML = ht;
		},
		txt : function (t) {
			this.innerHTML = g.HtmlEncode(t);
		}
	});
})(window);

(function (g) {
	var console,
	d = document;
	
	g.extend({
		time : function () {
			return (new Date()).getTime();
		},
		is_object : function (a) {
			return a && typeof a == "object";
		},
		is_array : function (a) {
			return Object.prototype.toString.call(a) === '[object Array]';
		},
		is_function : function (a) {
			return a && typeof a === "function";
		},
		is_RegExp : function (obj) {
			return !!(obj && obj.test && obj.exec && (obj.ignoreCase || obj.ignoreCase === false));
		},
		is_element : function (obj) {
			return !!(obj && obj.nodeType == 1);
		},
		isEmpty : function (o) {
			for (var prop in o)
				if (o.hasOwnProperty(prop))
					return false;
			return true;
		},
		is_window : function (obj) {
			return obj && typeof obj === "object" && "setInterval" in obj;
		},
		trim : function (s) {
			return s.replace(new RegExp('(^\\s*)|(\\s*$)', 'g'), "");
		},
		log : function (s) {
			if (!console) {
				console = g.create('div');
				
				console.style.cssText = "position:fixed!important;position:absolute;overflow:auto;text-align:left;top:10px;bottom: 10px;left:auto;right:10px;padding:5px;border:1px solid #000;background:#fff;width:300px;z-index:20000;word-wrap: break-word;word-break: break-all;";
				var tryShow = setInterval(function () {
						try{
							document.body.appendChild(console);
							console.scrollTop = console.scrollHeight;
							clearInterval(tryShow);
						}catch(e){}
					}, 10);
			}
			
			var as = [];
			
			for (var i = 0; i < arguments.length; i++) {
				var s = arguments[i];
				
				switch (typeof s) {
				case "undefined":
					as.push("[undefined]");
					break;
				case "object":
					try {
						as.push(g.HtmlEncode(JSON.stringify(s)));
					} catch (e) {
						as.push(Object.prototype.toString.call(s));
					}
					break;
				case "string":
					as.push(g.HtmlEncode(s));
					break;
				default:
					as.push(s.toString());
					break;
				}
			}
			
			var n = g.create('div');
			n.innerHTML = as.join(",");
			console.append(n);
			console.scrollTop = console.scrollHeight;
		},
		logUrl : function(){},
		logEvent : function(){},
		head : document.head || document.getElementsByTagName('head')[0] || document.documentElement
	});

	function isUndefinedOrNull(value) {
		return value === null || value === undefined;
	}

	function isArguments(object) {
		return Object.prototype.toString.call(object) == '[object Arguments]';
	}

	function objEquiv(a, b) {
		if (isUndefinedOrNull(a) || isUndefinedOrNull(b))
			return false;

		if (a.prototype !== b.prototype)
			return false;

		if (isArguments(a)) {
			if (!isArguments(b))
				return false;

			a = pSlice.call(a);
			b = pSlice.call(b);
			return _deepEqual(a, b);
		}
		try {
			var ka = Object.keys(a), kb = Object.keys(b), key, i;
		} catch (e) {
			return false;
		}

		if (ka.length != kb.length)
			return false;

		ka.sort();
		kb.sort();

		for (i = ka.length - 1; i >= 0; i--)
			if (ka[i] != kb[i])
				return false;

		for (i = ka.length - 1; i >= 0; i--) {
			key = ka[i];
			if (!_deepEqual(a[key], b[key]))
				return false;
		}
		return true;
	}
	
	function _deepEqual(actual, expected) {
		if (actual === expected)
			return true;
		else if (actual instanceof Date && expected instanceof Date)
			return actual.getTime() === expected.getTime();
		else if (actual instanceof RegExp && expected instanceof RegExp)
			return actual.source === expected.source
					&& actual.global === expected.global
					&& actual.multiline === expected.multiline
					&& actual.lastIndex === expected.lastIndex
					&& actual.ignoreCase === expected.ignoreCase;
		else if (typeof actual != 'object' && typeof expected != 'object')
			return actual == expected;
		else
			return objEquiv(actual, expected);
	}
	
	g.deepEqual = _deepEqual;
	
	(function () {
		var scripts = document.getElementsByTagName('script');
		var src = scripts[scripts.length - 1].src;
		g.baseUrl = src.substring(0, src.lastIndexOf('/') + 1);
		if (src.lastIndexOf('?') > 0)
			g.baseUrlVer = src.substring(src.lastIndexOf('?'));
		else
			g.baseUrlVer = '';
	})();
	
	g(window);
})(jModo);

(function (g) {
	var d = document,
	re = new RegExp("(\r\n)|[&\"'<>\r\n]", "g");
	
	g.extend({
		create : function (t, n, v) {
			var o = g(document.createElement(t));
			if (arguments.length > 1)
				o.attr(n, v);
			return o;
		},
		HtmlEncode : function (s) {
			s = s || "";
			return s.replace(re, function (s) {
				switch (s) {
				case "&":
					return "&amp;";
				case '"':
					return '&quot;';
				case "'":
					return '&apos;';
				case "<":
					return "&lt;";
				case ">":
					return "&gt;";
				default:
					return "<br>";
				}
			});
		}
	});
	
	g.fn.extend({
		create : function (t, n, v) {
			var e = g.create(t, n, v);
			this.append(e);
			return e;
		},
		createBefore : function(t, n, v){
			var e = g.create(t, n, v);
            var p = this.parentNode;
            p.insertBefore(e, this);
            return e;
        },
		attr : function (n, v) {
			if (v !== undefined)
				if(n === "class" && this.nodeType === 1)
					this.className = v;
				else
					this.setAttribute(n, v);
			else if (g.is_object(n))
				for (var c in n)
					this.setAttribute(c, n[c]);
			else if (typeof n === "string")
				if(n === "class" && this.nodeType === 1)
					return this.className;
				else
					return this.getAttribute(n);
			
			return this;
		},
		val : function (v) {
			if (v !== undefined)
				this.value = v;
			else
				return this.value;
		},
		removeAttr : function (n) {
			this.removeAttribute(n);
			return this;
		},
		opacity : function (a) {
			switch (g.browser.engine) {
			case "trident":
				this.style["filter"] = "alpha(opacity=" + (a * 100) + ")";
			case "gecko":
				this.style["-khtml-opacity"] = a;
			case "webkit":
				this.style["-moz-opacity"] = a;
			case "presto":
				this.style["opacity"] = a;
			}
			return this;
		}
	});
})(jModo);

(function (g, w, n) {
	var versionSearchString;
	var dataBrowser = [{
			string : n.userAgent,
			subString : new RegExp("Chrome", 'i'),
			identity : "Chrome"
		}, {
			string : n.userAgent,
			subString : new RegExp("OmniWeb", 'i'),
			versionSearch : "OmniWeb/",
			identity : "OmniWeb"
		}, {
			string : n.userAgent,
			subString : new RegExp("iPad|iPhone|iPod", 'i'),
			identity : "Safari",
			versionSearch : "OS"
		}, {
			string : n.userAgent,
			subString : new RegExp("AppleWebKit", 'i'),
			identity : "AppleWebKit",
			versionSearch : "Version"
		}, {
			string : n.vendor,
			subString : new RegExp("iCab", 'i'),
			identity : "iCab"
		}, {
			string : n.vendor,
			subString : new RegExp("KDE", 'i'),
			identity : "Konqueror"
		}, {
			string : n.vendor,
			subString : new RegExp("Camino", 'i'),
			identity : "Camino"
		}, {
			string : n.userAgent,
			subString : new RegExp("Netscape", 'i'),
			identity : "Netscape"
		}, {
			string : n.userAgent,
			subString : new RegExp("Gecko", 'i'),
			identity : "Mozilla",
			versionSearch : "rv"
		}, {
			string : n.userAgent,
			subString : new RegExp("Mozilla", 'i'),
			identity : "Netscape",
			versionSearch : "Mozilla"
		}
	];
	var dataOS = [{
			string : n.userAgent,
			subString : new RegExp("Windows CE", 'i'),
			identity : "WinCE"
		}, {
			string : n.platform,
			subString : new RegExp("Win", 'i'),
			identity : "Windows"
		}, {
			string : n.platform,
			subString : new RegExp("Mac", 'i'),
			identity : "Mac"
		}, {
			string : n.userAgent,
			subString : new RegExp("iPhone", 'i'),
			identity : "iPhone"
		}, {
			string : n.userAgent,
			subString : new RegExp("iPad", 'i'),
			identity : "iPad"
		}, {
			string : n.userAgent,
			subString : new RegExp("iPod", 'i'),
			identity : "iPod"
		}, {
			string : n.userAgent,
			subString : new RegExp("Android", 'i'),
			identity : "Android"
		}, {
			string : n.userAgent,
			subString : new RegExp("BlackBerry", 'i'),
			identity : "BlackBerry"
		}, {
			string : n.userAgent,
			subString : new RegExp("hpwOS|webOS", 'i'),
			identity : "webOS"
		}, {
			string : n.platform,
			subString : new RegExp("Linux", 'i'),
			identity : "Linux"
		}
	];
	
	function searchString(data) {
		var i;
		for (i = 0; i < data.length; i++) {
			var dataString = data[i].string;
			versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.match(data[i].subString)) {
					return data[i].identity;
				}
			}
		}
	}
	
	function searchVersion(dataString) {
		var index = dataString.indexOf(versionSearchString);
		if (index === -1) {
			return;
		}
		return parseFloat(dataString.substring(index + versionSearchString.length + 1));
	}
	
	var dataString = n.userAgent;
	if ("ActiveXObject" in w) {
		g.browser = {
			engine : "trident",
			agent : "msie",
			ver : w.XMLHttpRequest ? (w.XDomainRequest ? (document.querySelector ? 9 : 8) : 7) : 6
		};
	} else if ("opera" in w) {
		var index;
		g.browser = {
			engine : "presto",
			agent : "opera",
			ver : ((index = dataString.indexOf("Version")) !== -1) ?
			parseFloat(dataString.substring(index + 8)) :
			parseFloat(dataString.substring(dataString.indexOf("Opera") + 6))
		};
	} else if ("globalStorage" in w || "mozIndexedDB" in w) {
		g.browser = {
			engine : "gecko",
			agent : "firefox",
			ver : parseFloat(dataString.substring(dataString.indexOf("Firefox") + 8))
		};
	} else {
		g.browser = {
			agent : searchString(dataBrowser),
			ver : (searchVersion(n.userAgent) || searchVersion(n.appVersion) || "an unknown version")
		};
		if ("getComputedStyle" in w)
			g.browser.engine = "webkit";
	}
	
	g.browser.os = (searchString(dataOS) || "an unknown OS");
	g.browser[g.browser.agent] = true;
	g.browser[g.browser.os] = true;
	g.browser[g.browser.engine] = true;
	g.browser[g.browser.agent + g.browser.ver] = true;
	g.browser.desktop = !!{
		Windows : true,
		Mac : true,
		Linux : true
	}
	[g.browser.os];
	
	if(!g.browser.desktop)
		g.browser.phone = (document.documentElement.clientWidth < 600 || document.documentElement.clientHeight < 600);
//	g.log(screen.width, screen.height, document.documentElement.clientWidth, document.documentElement.clientHeight);
	
	g.browser.ios = !!{
		iPhone : true,
		iPad : true,
		iPod : true
	}[g.browser.os];

	g.browser.webApp = !g.browser.desktop && g.browser["Safari"] && n.userAgent.indexOf('Safari') === -1;

	var vendor = g.browser.webkit ? 'webkit' : g.browser.gecko ? 'Moz' : g.browser.presto ? 'O' : (g.browser.trident && g.browser.ver > 9) ? 'ms' : '';
	g.browser.hasTransform =  (vendor + 'Transform') in document.documentElement.style;
	g.browser.has3d = g.browser.webkit && 'WebKitCSSMatrix' in window && 'm11' in new WebKitCSSMatrix();
	g.browser.retina = window.devicePixelRatio > 1 ? true : false;
	
	//g.browser.desktop = false;
	//g.browser.phone = !g.browser.desktop && screen.width < 960 && screen.height < 960;

	var pu_re = new RegExp("^(?:(?![^:@]+:[^:@\\/]*@)([^:\\/?#.]+):)?(?:\\/\\/)?((?:(([^:@]*)(?::([^:@]*))?)?@)?([^:\\/?#]*)(?::(\\d*))?)(((\\/(?:[^?#](?![^?#\\/]*\\.[^?#\\/.]+(?:[?#]|$)))*\\/?)?([^?#\\/]*))(?:\\?([^#]*))?(?:#(.*))?)");
	var pu_qre = new RegExp('(?:^|&)([^&=]*)=?([^&]*)', 'g');
	var pu_key = ["source", "protocol", "authority", "userInfo", "user", "password", "host", "port", "relative", "path", "directory", "file", "query", "anchor"];
	
	g.extend({
		cookie : function (name, value, expires, path, domain, secure) {
			if (arguments.length === 1) {
				var start = document.cookie.indexOf(name + "=");
				var len = start + name.length + 1;
				if ((!start) && (name != document.cookie.substring(0, name.length))) {
					return "";
				}
				if (start === -1) {
					return "";
				}
				var end = document.cookie.indexOf(';', len);
				if (end === -1) {
					end = document.cookie.length;
				}
				return unescape(document.cookie.substring(len, end));
			} else {
				var today = new Date();
				today.setTime(today.getTime());
				if (expires) {
					expires = expires * 1000 * 60 * 60 * 24;
				}
				var expires_date = new Date(today.getTime() + (expires));
				document.cookie = name + '=' + escape(value) + ((expires) ? ';expires=' + expires_date.toGMTString() : '') + ((path) ? ';path=' + path : '') + ((domain) ? ';domain=' + domain : '') + ((secure) ? ';secure' : '');
			}
		},
		deleteCookie : function (name, path, domain) {
			if (g.getCookie(name)) {
				document.cookie = name + '=' + ((path) ? ';path=' + path : '') + ((domain) ? ';domain=' + domain : '') + ';expires=Thu, 01-Jan-1970 00:00:01 GMT';
			}
		},
		parseUri : function (str) {
			var m = pu_re.exec(str),
			uri = {},
			i = pu_key.length;
			while (i--) {
				uri[pu_key[i]] = m[i] || "";
			}
			
			uri.queryKey = {};
			uri.query.replace(pu_qre, function ($0, $1, $2) {
				if ($1) {
					uri.queryKey[$1] = $2;
				}
			});
			
			uri.toString = function () {
				return this.source;
			};
			
			return uri;
		},
		dirname : function (path) {
			var s = path.match(/.*(?=\/.*$)/);
			return (s ? s[0] : '.') + '/';
		},
		realpath : function (path) {
			path = path.replace(/([^:\/])\/+/g, '$1\/');
			
			if (path.indexOf('.') === -1) {
				return path;
			}
			
			var old = path.split('/');
			var ret = [],
			part,
			i = 0,
			len = old.length;
			
			for (; i < len; i++) {
				part = old[i];
				if (part === '..') {
					if (ret.length === 0) {
						util.error('Invalid path:', path);
					}
					ret.pop();
				} else if (part !== '.') {
					ret.push(part);
				}
			}
			
			return ret.join('/');
		},
		sameDomain : function (u) {
			var svrUrl = g.parseUri(u);
			return !svrUrl["host"] || ((location.hostname === svrUrl["host"]) && ((location.port || 80) === (svrUrl["port"] || 80)));
		}
	});
})(jModo, window, navigator);
