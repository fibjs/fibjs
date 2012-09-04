(function (g) {
	"use strict";
	
	var running,
	pendding = {},
	modules = g.modules = {},
	config,
	a_mod,
	p_mods = {},
	evs = new g.Object(),
	errMod = {
		fn : undefined,
		pendding : 0
	};
	
	g(g.head);
	config = modules.config = {};
	modules.jModo = g;
	
	function loadScript(url, callback) {
		var s = g.create('script'),
		tm;
		
		function onload() {
			if (!s.readyState || s.readyState === 'loaded' || s.readyState === 'complete') {
				var v = s.mod || a_mod;
				s.mod = a_mod = undefined;
				
				if (tm) {
					clearInterval(tm);
					tm = undefined;
				}
				
				s.onreadystatechange = null;
				s.remove();
				s = null;
				callback(v);
			}
		}
		
		function onerror() {
			if (s) {
				a_mod = errMod;
				onload();
			}
		}
		
		if (g.browser.opera)
			tm = setInterval(function () {
					if (s.readyState === 'loaded') {
						clearInterval(tm);
						tm = undefined;
						setTimeout(onerror, 100);
					}
				}, 1);
		
		if (s.readyState)
			s.onreadystatechange = onload;
		else {
			s.onload = onload;
			s.onerror = onerror;
		}
		
		s.type = 'text/javascript';
		s.async = true;
		s.src = url;
		
		var s1 = running;
		running = s;
		g.head.append(s)
		running = s1;
	}
	
	function fixurl(id) {
		if (id.indexOf('/') !== -1)
			return id;
		
		var a = id.split('.');
		if (a[0]in config)
			return config[a[0]] + '/' + a.slice(1).join('/') + ".js";
		
		return id;
	}
	
	var module = g.Class({
			_init : function (id, deps, fn) {
				this.extend({
					id : id,
					deps : deps,
					mods : [],
					url : undefined,
					fn : fn,
					pendding : deps ? deps.length : 0
				});
				
				if (id)
					p_mods[id] = 1;
				else if (g.browser.msie) {
					if (!running || running.readyState !== 'interactive') {
						var scripts = g(g.head).tag('script');
						running = undefined;
						
						for (var i = 0; i < scripts.length; i++) {
							var s = scripts[i];
							if (s.readyState === 'interactive') {
								running = s;
								break;
							}
						}
					}
					
					if (running)
						running.mod = this;
				} else
					a_mod = this;
				
				this.loadDeps();
			},
			ready : function () {
				var factory = this.fn,
				url = this.url,
				id = this.id;
				
				if (typeof factory === "function")
					this.fn = factory = factory.apply(null, this.mods);
				
				if (id) {
					modules[id] = factory;
					delete p_mods[id];
					evs.trigger(id, factory);
				} else if (url && pendding[url]) {
					delete pendding[url];
					evs.trigger(url, factory);
				}
			},
			depReady : function (i, v) {
				this.mods[i] = v;
				if ((--this.pendding) === 0)
					this.ready();
			},
			require : function (i) {
				var dep = this.deps[i];
				
				if (dep in modules)
					this.depReady(i, modules[dep]);
				else if (dep in p_mods)
					evs.once(dep, this.depReady.bind(this, i));
				else {
					var url = fixurl(dep);
					
					evs.once(dep, this.depReady.bind(this, i));
					
					if (!(url in pendding)) {
						pendding[url] = 1;
						loadScript(url, function (v) {
							if (v) {
								if (v.pendding === 0) {
									delete pendding[url];
									evs.trigger(url, v.fn);
								} else
									v.url = url;
							} else {
								delete pendding[url];
								evs.trigger(url, undefined);
							}
						});
					}
				}
			},
			loadDeps : function () {
				var len = this.pendding;
				
				if (len === 0)
					this.ready();
				else
					for (var i = 0; i < len; i++)
						this.require(i);
			}
		});
	
	function define(id, deps, factory) {
		var argsLen = arguments.length;
		
		if (argsLen === 0 || argsLen > 3)
			throw new Error('Operation is not allowed.');
		else if (argsLen === 1) {
			factory = id;
			id = undefined;
		} else if (argsLen === 2) {
			factory = deps;
			
			if (g.is_array(id)) {
				deps = id;
				id = undefined;
			} else
				deps = undefined;
		}
		
		new module(id, deps, factory);
	}
	
	g.load = function (u, fn) {
		new module(undefined, g.is_array(u) ? u : [u], fn);
	};
	
	window.define = define;
})(jModo);
