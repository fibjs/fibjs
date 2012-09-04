(function (g) {
	"use strict";
	
	var w = window,
	el = document.documentElement,
	slice = Array.prototype.slice,
	spc = new RegExp("\\s+");
	
	var events = {
		on : function (evType, fn) {
			var events = {},
			event_handles = {};
			
			function _trigger(o, evType, as) {
				var evs = events[evType],
				r = true;
				
				if (evs)
					evs.slice().forEach(function (v) {
						if (!v || !v.apply(o, as))
							r = false;
					});
				return r;
			}
			
			this.extend({
				on : function (evType, fn) {
					if (evType === undefined)
						return;
					
					if (g.is_object(evType)) {
						for (var c in evType)
							this.on(c, evType[c]);
						return;
					}
					
					var a = evType.split(spc);
					
					if (a.length > 1) {
						for (var i = 0; i < a.length; i++)
							this.on(a[i], fn);
						return;
					}
					
					if ("child" in this) {
						a = evType.split('.');
						if (a.length === 2) {
							var child = this.child(a[0]);
							if (child)
								return child.on(a[1], fn);
							else
								throw new Error("Element '" + a[0] + "' not found.");
						}
					}
					
					var bMouse = evType.substring(0, 5) === "mouse",
					bTouch = evType.substring(0, 5) === "touch",
					o = this;
					
					function handle_event() {
						var as = arguments;
						
						if (w.event) {
							var e = w.event,
							b = document.body;
							
							if (!e.target)
								e.target = g(e.srcElement);
							else
								g(e.target);
							
							if (bTouch) {
								if (e.touches.length)
									as = [{
											target : e.target,
											pageX : e.touches[0].pageX,
											pageY : e.touches[0].pageY,
											clientX : e.touches[0].clientX,
											clientY : e.touches[0].clientY,
											touches : e.touches
										}
									];
							} else if (bMouse) {
								if (!e.pageX) {
									e.pageX = e.clientX + b.scrollLeft + el.scrollLeft;
									e.pageY = e.clientY + b.scrollTop + el.scrollTop;
								}
								
								if (evType === 'mousewheel' && g.browser.opera && g.browser.ver === 9) // Opera 9.0 MouseScroll
									as = [{
											wheelDelta : -as[0].wheelDelta,
											target : e.target,
											pageX : e.pageX,
											pageY : e.pageY,
											clientX : e.clientX,
											clientY : e.clientY
										}
									];
								else if (evType === 'mousewheel' && Math.abs(as[0].wheelDelta) > 1000) // Chrome 2.0 MouseScroll
									as = [{
											wheelDelta : as[0].wheelDelta / 100,
											target : e.target,
											pageX : e.pageX,
											pageY : e.pageY,
											clientX : e.clientX,
											clientY : e.clientY
										}
									];
								else
									as = [e];
							} else
								as = [e];
						} else if (evType === "DOMMouseScroll") {
							var e = as[0];
							
							as = [{
									wheelDelta : -e.detail * 40,
									target : e.target,
									pageX : e.pageX,
									pageY : e.pageY,
									clientX : e.clientX,
									clientY : e.clientY
								}
							];
						}
						
						var r = _trigger(o, evType, as);
						
						if (!r)
							if (w.event)
								w.event.returnValue = false;
							else
								arguments[0].preventDefault();
						
						return r;
					}
					
					if (evType === 'mousewheel' && g.browser.firefox)
						evType = "DOMMouseScroll";
					
					if (evType in events)
						events[evType].push(fn);
					else {
						events[evType] = [fn];
						event_handles[evType] = handle_event;
						
						if (o.addEventListener)
							o.addEventListener(evType, handle_event, false);
						else if (o.attachEvent)
							o.attachEvent("on" + evType, handle_event);
					}
				},
				off : function (evType, fn) {
					var o = this;
					
					if (evType === undefined) {
						for (var en in events)
							o.off(en);
					} else {
						var a = evType.split(spc);
						
						if (a.length > 1) {
							for (var i = 0; i < a.length; i++)
								this.off(a[i], fn);
							return;
						}
						
						if ("child" in this) {
							var a = evType.split('.');
							if (a.length === 2) {
								var child = this.child(a[0]);
								if (child)
									return child.off(a[1], fn);
								else
									throw new Error("Element '" + a[0] + "' not found.");
							}
						}
						
						if (evType === 'mousewheel' && g.browser.firefox)
							evType = "DOMMouseScroll";
						
						var evs = events[evType];
						
						if (!evs)
							return;
						
						if (fn)
							evs.remove(fn);
						
						if (!fn || evs.length === 0) {
							if (o.removeEventListener)
								o.removeEventListener(evType, event_handles[evType], false);
							else if (o.detachEvent)
								o.detachEvent("on" + evType, event_handles[evType]);
							
							delete event_handles[evType];
							delete events[evType];
						}
					}
				},
				trigger : function (evType) {
					if (evType === undefined)
						return;
					
					var a = evType.split(spc);
					
					if (a.length > 1) {
						for (var i = 0; i < a.length; i++)
							this.trigger(a[i]);
						return;
					}
					
					if ("child" in this) {
						var a = evType.split('.');
						if (a.length === 2) {
							var child = this.child(a[0]);
							if (child)
								return child.trigger(a[1], slice.call(arguments, 1));
							else
								throw new Error("Element '" + a[0] + "' not found.");
						}
					}
					
					return _trigger(this, evType, slice.call(arguments, 1));
				}
			});
			
			this.on(evType, fn);
		},
		once : function (evType, fn) {
			var o = this;
			
			function onceProxy() {
				o.off(evType, onceProxy);
				fn.apply(this, arguments);
			}
			
			o.on(evType, onceProxy);
		},
		off : function () {},
		trigger : function () {},
		proxy : function () {
			var len = arguments.length,
			o = this,
			n;
			
			for (var i = 0; i < len; i++) {
				n = arguments[i];
				if (n in o && g.is_function(o[n]))
					o[n] = o[n].bind(this);
			}
		}
	};
	
	g.fn.extend(events);
	g.clsfn.extend(events);
	
	var epoll = g.epoll = function () {
		if (!(this instanceof epoll))
			throw new Error('Operation is not allowed.');
		
		var es = {},
		aes = {},
		aeid = 1,
		rs = {},
		rp = null;
		
		function e() {
			var as = arguments,
			k,
			k1,
			m,
			h,
			r;
			
			function p() {
				if (k && k in es) {
					if (h)
						r = h.apply(null, arguments);
					else
						r = slice.call(arguments, 0);
					
					rs[k] = {
						event : m,
						data : r
					};
					
					delete es[k];
				} else if (k1 && k1 in aes) {
					if (h)
						h.apply(null, arguments);
					
					delete aes[k1];
				}
				
				if (g.isEmpty(es) && g.isEmpty(aes) && rp) {
					var rp1 = rp;
					rp = null;
					rp1(rs);
				}
			}
			
			switch (as.length) {
			case 0:
				k1 = aeid++;
			case 1:
				if (typeof as[0] === "function") {
					k1 = aeid++;
					h = as[0];
				} else if (typeof as[0] === "string") {
					k = as[0];
				}
			case 2:
				if (typeof as[0] === "string") {
					if (typeof as[1] === "string") {
						k = as[0];
						m = as[1];
					} else if (typeof as[1] === "function") {
						k = as[0];
						h = as[1];
					}
				}
			case 2:
				if (typeof as[0] === "string" && typeof as[1] === "string" && typeof as[2] === "function") {
					k = as[0];
					m = as[1];
					h = as[2];
				}
			}
			
			if (k1) {
				aes[k1] = true;
				return p;
			}
			
			if (k) {
				es[k] = true;
				return p;
			}
			
			throw new Error("Invalid Argument");
		}
		
		e.ready = function (r) {
			if (g.isEmpty(es) && g.isEmpty(aes))
				r(rs);
			else
				rp = r;
		}
		
		return e;
	};
})(jModo);
