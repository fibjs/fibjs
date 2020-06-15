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