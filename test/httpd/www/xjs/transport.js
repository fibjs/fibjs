define("jModo.transport", ["jModo"], function(g) {
	"use strict";

	var w = g(window),
		d = g(document),
		flashBridge = false,
		fss = [],
		fssi = 0,
		rndID = (new Date()).getTime(),
		fs,
		i;

	w["fsCallback" + rndID] = function(id, cmd, value) {
		if (id < 0) {
			if (cmd === 'oninit') {
				var tmpfss = fss.slice(0);
				flashBridge = true;
				setTimeout(function() {
					tmpfss.forEach(function(o) {
						o.connect();
					});
				}, 10);
			}
		} else {
			var o;

			if (o = fss[id]) {
				if (cmd === 'onopen')
					o.reping();

				if (o && cmd in o)
					if (cmd === 'onmessage')
						o.onmessage({
							data: decodeURIComponent(value)
						});
					else
						o[cmd]();

				if (cmd === 'onclose' || cmd === 'onerror')
					delete fss[id];
			}
		}
	};

	function getFlashID() {
		for (fssi++; fss[fssi]; fssi++) {
			if (fssi > 32000)
				fssi = 1;
		}
		return fssi;
	}

	g.websocket = g.Class(function() {
		var MyID = 0,
			o = this,
			ws = null,
			l_ping,
			l_pingval = 10000,
			MyUrl;

		function ping() {
			o.send("");
		}

		function nil() {}

		this.extend({
			_init: function(url) {
				MyUrl = url;
				o.reconnect();
			},
			setKeepaliveInterval: function(t) {
				l_pingval = t;
				o.reping();
			},
			reping: function() {
				clearTimeout(l_ping);
				l_ping = setTimeout(ping, l_pingval);
			},
			close: function() {
				if (ws) {
					ws.close();
					ws = null;
				} else if (MyID) {
					fs.close(MyID);
					MyID = 0;
				}
				clearTimeout(l_ping);
			},
			send: function(data) {
				o.reping();
				if (ws) {
					ws.send(data);
				} else {
					fs.send(MyID, data);
				}
			},
			onopen: nil,
			onmessage: nil,
			onclose: nil,
			onerror: nil,
			reconnect: function() {
				o.close();

				if ('MozWebSocket' in w)
					ws = new MozWebSocket(MyUrl);
				else if ("WebSocket" in w)
					ws = new WebSocket(MyUrl);

				if (ws) {
					ws.onopen = function() {
						o.reping();
						o.onopen();
					};
					ws.onmessage = function(e) {
						o.onmessage(e);
					};
					ws.onclose = function() {
						o.onclose();
					};
					ws.onerror = function() {
						o.onerror();
					};
				} else {
					MyID = getFlashID();
					fss[MyID] = o;
					if (flashBridge)
						fs.connect(MyID, MyUrl);
					else
						o.connect = function() {
							fs.connect(MyID, MyUrl);
							delete o.connect;
						};
				}
			}
		});
	});

	var FlashHttp = g.Class(function() {
		var MyID,
			o = this,
			MyUrl,
			MyData;

		this.extend({
			_init: function(url, data) {
				MyUrl = url;
				MyData = data;

				MyID = getFlashID();
				fss[MyID] = o;

				if (flashBridge)
					this.connect();
			},
			connect: function() {
				setTimeout(function() {
					fs.http(MyID, MyUrl, MyData);
				}, 10);
			},
			close: function() {
				fs.close(MyID);
				delete fss[MyID];
			}
		});
	});

	function _initFlash() {
		var fsc = g.create('div');
		var smv;

		fsc.style.cssText = "position:absolute;top:0px;left:-1000px;width:10px;height:10px;";
		d.body.appendChild(fsc);

		if (g.browser.msie) {
			smv = '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab" id=f_socket><param name="movie" value="' + g.baseUrl + 'fsocket.swf' + g.baseUrlVer + '"><param name="allowScriptAccess" value="always"><param name="FlashVars" value="callback=fsCallback' + rndID + '"></object>';
			var p1 = g.create('span');
			fsc.appendChild(p1);
			var s = d.title;
			p1.outerHTML = smv;
			d.title = s;
		} else {
			smv = '<embed type="application/x-shockwave-flash" src="' + g.baseUrl + 'fsocket.swf' + g.baseUrlVer + '" allowScriptAccess="always" FlashVars="callback=fsCallback' + rndID + '"></embed>';
			fsc.innerHTML = smv;
		}
		fs = fsc.childNodes[0];
	}

	if (d.body)
		_initFlash();
	else
		w.once("load", _initFlash);

	var msXmlhttp = ["Msxml2.XMLHTTP.5.0", "Msxml2.XMLHTTP.4.0", "Msxml2.XMLHTTP.3.0", "Msxml2.XMLHTTP", "Microsoft.XMLHTTP"];

	g.extend({
		enableWebSocket: true,
		enableXMLHttpRequest: true,
		enableFlashHttp: true,
		enableIFRAME: true,
		flashQuery: function(u, p, r, er) {
			var frq;

			if (g.enableFlashHttp) {
				frq = new FlashHttp(u, p);
				frq.extend({
					onerror: function() {
						setTimeout(function() {
							er("Server error.");
						}, 10);
					},
					onmessage: function(e) {
						frq = null;
						setTimeout(function() {
							r(e.data);
						}, 10);
					}
				});
				return true;
			}

			return false;
		},
		xdrQuery: function(u, p, r, er) {
			var xdr;

			function onXhrReady() {
				if (xdr.readyState === 4) {
					if (xdr.status != 200) {
						if (xdr.responseText)
							er(xdr.responseText);
						else
							er("Server response error.");
					} else
						r(xdr.responseText);
				}
			}

			if (!g.enableXMLHttpRequest)
				return false;

			if ('XMLHttpRequest' in w) {
				xdr = new XMLHttpRequest();
				if (!g.sameDomain(u) && !("withCredentials" in xdr))
					return false;
			} else if (g.sameDomain(u)) {
				for (i = 0; i < msXmlhttp.length; i++) {
					try {
						xdr = new ActiveXObject(msXmlhttp[i]);
					} catch (e) {}
				}
			} else
				return false;

			xdr.onreadystatechange = onXhrReady;

			try {
				if (p != null) {
					xdr.open("POST", u);
					if ("setRequestHeader" in xdr)
						xdr.setRequestHeader("Content-Type", "application/json");
				} else
					xdr.open("GET", u);

				if ("withCredentials" in xdr)
					xdr.withCredentials = true;
				xdr.send(p);
			} catch (e) {
				er("Server request error.");
			}

			return true;
		},
		Query: function() {
			var n = arguments.length,
				u,
				p,
				r,
				er,
				xdr;

			p = null;
			er = function(m) {
				throw new Error(m)
			};
			u = arguments[0];
			if (n >= 2 && n <= 4 && typeof arguments[n - 1] === "function") {
				if (typeof arguments[n - 2] === "function") {
					r = arguments[n - 2];
					er = arguments[n - 1];
					if (n == 4)
						p = arguments[n - 3];
				} else if (n < 4) {
					r = arguments[n - 1];
					if (n == 3)
						p = arguments[n - 2];
				} else
					throw new Error("Invalid Argument");
			} else
				throw new Error("Invalid Argument");

			if (g.xdrQuery(u, p, r, er))
				return true;

			return g.flashQuery(u, p, r, er);
		}
	});
});