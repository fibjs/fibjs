define("jModo.rpc", ["jModo", "jModo.transport", "jModo.json"], function (g) {
	"use strict";
	
	var slice = Array.prototype.slice;
	
	g.rpc = function () {
		var r,
		t = 10000,
		id,
		n = arguments.length,
		url,
		jss = [],
		ht,
		frame,
		frmChecker,
		i;
		
		function on_msg(m) {
			try {
				clearTimeout(id);
				
				if (frame) {
					if (frmChecker)
						clearInterval(frmChecker);
					else
						frame.parentNode.removeChild(frame);
				}
			} catch (e) {}
			
			frame = null;
			ht = null;
			
			if(r)
				r(m);
		}
		
		function on_err(e) {
			if(!r)
				throw new Error(e);
			on_msg({
				error : {
					code : -1,
					message : e
				}
			});
		}
		
		url = arguments[0];
		if (n >= 3) {
			if (n >= 4 && typeof arguments[n - 2] === "function") {
				t = arguments[n - 1];
				r = arguments[n - 2];
				n -= 2;
			} else if (typeof arguments[n - 1] === "function") {
				r = arguments[n - 1];
				n--;
			}
		}
		
		id = setTimeout(function () {
				on_err("Timeout.");
			}, t);
		
		jss.push('{"method":"');
		jss.push(arguments[1].toString());

		if(n > 2)
		{
			jss.push('","params":');
			jss.push(JSON.stringify(slice.call(arguments, 2, n)));
			jss.push("}");
		}else jss.push('"}');
		
		var ps = jss.join("");
		
		function on_text_msg(t) {
			on_msg(JSON.parse(t));
		}

		var fdoc,
		state = 0,
		loadState = 0;

		function onFrameLoad() {
			if (state === 2) {
				var r;
				try {
					r = JSON.parse(frame.contentWindow.name);
				} catch (e) {
					on_err("Server error.");
				}
				
				if (r)
					on_msg(r);
			} else if (state === 1) {
				try {
					state = 2;
					loadState = 0;
					
					frame.contentWindow.location = "about:blank";
				} catch (e) {
					on_err("Server error.");
				}
			} else if (state === 0) {
				state = 1;
				loadState = 0;
				var frm = fdoc.getElementById("frm");
				frm.jsonrpc.value = ps;
				frm.submit();
			}
		}
		
		if (g.xdrQuery(url, ps, on_text_msg, on_err))
			return;

		if (g.flashQuery(url, ps, on_text_msg, on_err))
			return;
			
		if (g.enableIFRAME) {
			if (g.browser.msie) {
				ht = new ActiveXObject("htmlfile");
				
				ht.open();
				ht.write('<html><iframe id="ifrm"></iframe></html>');
				ht.close();
				
				frame = ht.getElementById("ifrm");
				
				frmChecker = setInterval(function () {
						if (loadState === 0)
							try {
								frame = ht.getElementById("ifrm");
								if (frame.readyState === 'complete') {
									loadState = 1;
									onFrameLoad();
								}
							} catch (e) {
								on_err("Server error.");
							}
					}, 1);
			} else {
				frame = g.create('iframe');
				frame.style.cssText = "position:absolute;top:0px;left:-1000px;width:0px;height:0px;";
				document.body.appendChild(frame);
			}

			g(frame).on("load", onFrameLoad);

			fdoc = frame.contentWindow.document;
			fdoc.open();
			fdoc.write('<body><form id="frm" method="post" action="' + url + '" accept-charset="utf-8"><input name=jsonrpc type=hidden></form></body>');
			fdoc.close();
			return;
		}
		
		on_err("Server error.");
	};
});
