(function (w) {
    if (!w.JSON)
        w.JSON = {};

    function f(n) {
        return n < 10 ? "0" + n : n;
    }
    if (typeof Date.prototype.toJSON !== "function") {
        Date.prototype.toJSON = function (key) {
            return isFinite(this.valueOf()) ? this.getUTCFullYear() + "-" + f(this.getUTCMonth() + 1) + "-" + f(this.getUTCDate()) + "T" + f(this.getUTCHours()) + ":" + f(this.getUTCMinutes()) + ":" + f(this.getUTCSeconds()) + "Z" : null;
        };
        String.prototype.toJSON = Number.prototype.toJSON = Boolean.prototype.toJSON = function (key) {
            return this.valueOf();
        };
    }
    var cx = new RegExp('[\\u0000\\u00ad\\u0600-\\u0604\\u070f\\u17b4\\u17b5\\u200c-\\u200f\\u2028-\\u202f\\u2060-\\u206f\\ufeff\\ufff0-\\uffff]', 'g'),
        escapable = new RegExp('[\\\\\\"\\x00-\\x1f\\x7f-\\x9f\\u00ad\\u0600-\\u0604\\u070f\\u17b4\\u17b5\\u200c-\\u200f\\u2028-\\u202f\\u2060-\\u206f\\ufeff\\ufff0-\\uffff]', 'g'),
        re1 = new RegExp("^[\\],:{}\\s]*$"),
        re2 = new RegExp("\\\\(?:[\"\\\\\\/bfnrt]|u[0-9a-fA-F]{4})", "g"),
        re3 = new RegExp("\"[^\"\\\\\\n\\r]*\"|true|false|null|-?\\d+(?:\\.\\d*)?(?:[eE][+\\-]?\\d+)?", "g"),
        re4 = new RegExp("(?:^|:|,)(?:\\s*\\[)+", "g"),
        gap,
        indent,
        meta = {
            "\b": "\\b",
            "\t": "\\t",
            "\n": "\\n",
            "\f": "\\f",
            "\r": "\\r",
            '"': '\\"',
            "\\": "\\\\"
        },
        rep;

    function quote(string) {
        escapable.lastIndex = 0;
        return escapable.test(string) ? '"' + string.replace(escapable, function (a) {
            var c = meta[a];
            return typeof c === "string" ? c : "\\u" + ("0000" + a.charCodeAt(0).toString(16)).slice(-4);
        }) + '"' : '"' + string + '"';
    }

    function str(key, holder) {
        var i,
            k,
            v,
            length,
            mind = gap,
            partial,
            value = holder[key];
        if (value && typeof value === "object" && typeof value.toJSON === "function") {
            value = value.toJSON(key);
        }
        if (typeof rep === "function") {
            value = rep.call(holder, key, value);
        }
        switch (typeof value) {
            case "string":
                return quote(value);
            case "number":
                return isFinite(value) ? String(value) : "null";
            case "boolean":
            case "null":
                return String(value);
            case "object":
                if (!value) {
                    return "null";
                }
                gap += indent;
                partial = [];
                if (Object.prototype.toString.apply(value) === "[object Array]") {
                    length = value.length;
                    for (i = 0; i < length; i += 1) {
                        partial[i] = str(i, value) || "null";
                    }
                    v = partial.length === 0 ? "[]" : gap ? "[\n" + gap + partial.join(",\n" + gap) + "\n" + mind + "]" : "[" + partial.join(",") + "]";
                    gap = mind;
                    return v;
                }
                if (rep && typeof rep === "object") {
                    length = rep.length;
                    for (i = 0; i < length; i += 1) {
                        k = rep[i];
                        if (typeof k === "string") {
                            v = str(k, value);
                            if (v) {
                                partial.push(quote(k) + (gap ? ": " : ":") + v);
                            }
                        }
                    }
                } else {
                    for (k in value) {
                        if (Object.hasOwnProperty.call(value, k)) {
                            v = str(k, value);
                            if (v) {
                                partial.push(quote(k) + (gap ? ": " : ":") + v);
                            }
                        }
                    }
                }
                v = partial.length === 0 ? "{}" : gap ? "{\n" + gap + partial.join(",\n" + gap) + "\n" + mind + "}" : "{" + partial.join(",") + "}";
                gap = mind;
                return v;
        }
    }
    if (typeof JSON.stringify !== "function") {
        JSON.stringify = function (value, replacer, space) {
            var i;
            gap = "";
            indent = "";
            if (typeof space === "number") {
                for (i = 0; i < space; i += 1) {
                    indent += " ";
                }
            } else {
                if (typeof space === "string") {
                    indent = space;
                }
            }
            rep = replacer;
            if (replacer && typeof replacer !== "function" && (typeof replacer !== "object" || typeof replacer.length !== "number")) {
                throw new Error("JSON.stringify");
            }
            return str("", {
                "": value
            });
        };
    }
    if (typeof JSON.parse !== "function") {
        JSON.parse = function (text, reviver) {
            var j;

            function walk(holder, key) {
                var k,
                    v,
                    value = holder[key];
                if (value && typeof value === "object") {
                    for (k in value) {
                        if (Object.hasOwnProperty.call(value, k)) {
                            v = walk(value, k);
                            if (v !== undefined) {
                                value[k] = v;
                            } else {
                                delete value[k];
                                value[k] = null;
                            }
                        }
                    }
                }
                return reviver.call(holder, key, value);
            }
            text = String(text);
            cx.lastIndex = 0;
            if (cx.test(text)) {
                text = text.replace(cx, function (a) {
                    return "\\u" + ("0000" + a.charCodeAt(0).toString(16)).slice(-4);
                });
            }
            if (re1.test(text.replace(re2, "@").replace(re3, "]").replace(re4, ""))) {
                j = eval("(" + text + ")");
                return typeof reviver === "function" ? walk({
                    "": j
                }, "") : j;
            }
            throw new SyntaxError("JSON.parse");
        };
    }
})(this);

(function (w) {
    function stringify(obj) {
        if (obj !== null && typeof obj === 'object' && typeof obj.toJSON !== 'function') {
            decirc(obj, '', [], null)
        }
        return JSON.stringify(obj)
    }

    function Circle(val, k, parent) {
        this.val = val
        this.k = k
        this.parent = parent
        this.count = 1
    }
    Circle.prototype.toJSON = function toJSON() {
        if (--this.count === 0) {
            this.parent[this.k] = this.val
        }
        return '[Circular]'
    }

    function decirc(val, k, stack, parent) {
        var keys, len, i
        if (typeof val !== 'object' || val === null) {
            return
        } else if (val instanceof Circle) {
            val.count++;
            return
        } else if (typeof val.toJSON === 'function' && !val.toJSON.forceDecirc) {
            return
        } else if (parent) {
            if (~stack.indexOf(val)) {
                parent[k] = new Circle(val, k, parent)
                return
            }
        }
        stack.push(val)
        for (var k in val)
            decirc(val[k], k, stack, val)
        stack.pop()
    }

    function _log(level, fmt) {
        if (fmt === undefined)
            fmt = "";
        fmt = fmt.toString();
        var re = /(%?)(%([jds]))/g,
            args = Array.prototype.slice.call(arguments, 2);
        if (args.length) {
            fmt = fmt.replace(re, function (match, escaped, ptn, flag) {
                var arg = args.shift();
                switch (flag) {
                    case 's':
                        if (typeof arg === 'object' && typeof arg.toJSON !== 'function')
                            arg = stringify(arg);
                        else
                            arg = '' + arg;
                        break;
                    case 'd':
                        arg = Number(arg);
                        break;
                    case 'j':
                        arg = stringify(arg);
                        break;
                }
                if (!escaped) {
                    return arg;
                }
                args.unshift(arg);
                return match;
            })
        }
        for (var i = 0; i < args.length; i++) {
            var arg = args[i];
            if (typeof arg === 'object' && typeof arg.toJSON !== 'function')
                fmt += ' ' + stringify(arg);
            else
                fmt += ' ' + arg;
        }
        fmt = fmt.replace(/%{2,2}/g, '%');
        external.log(level, '' + fmt);
    }

    function bind_log(level) {
        return function () {
            var args = [level].concat(Array.prototype.slice.call(arguments, 0));
            return _log.apply(this, args);
        };
    };
    w.console = {
        debug: bind_log(7),
        log: bind_log(6),
        notice: bind_log(5),
        warn: bind_log(4),
        error: bind_log(3),
        crit: bind_log(2),
        alert: bind_log(1),
        fatal: bind_log(0)
    }
})(this);