(function (w) {
    var external = window.external = {};

    external.onmessage = external.onmessage || function () { };
    external.onclose = external.onclose || function () { };

    Object.defineProperty(external, 'postMessage', {
        writable: false,
        value: function (arg) {
            window.webkit.messageHandlers.invoke.postMessage(arg);
        }
    });

    Object.defineProperty(external, 'log', {
        writable: false,
        value: function (logLevel, formattedLog) {
            window.webkit.messageHandlers.__externalLog.postMessage(JSON.stringify({
                level: logLevel,
                fmt: formattedLog
            }));
        }
    });

    Object.defineProperty(window, 'external', {
        writable: false,
        configurable: false,
        value: external
    });

    window.onerror = function (err_msg) {
        var err = new Error(err_msg)
        var msg = err_msg + '\n    at ' + err.stack
        console.error(msg)
    }

    var origClose = window.close.bind(window);


    Object.defineProperty(window, 'close', {
        writable: false,
        configurable: false,
        // window.close() wouldn't trigger `WindowShouldClose` handler, we should patch it 
        value: function () {
            origClose.apply(window, arguments);
            window.webkit.messageHandlers.__inward.postMessage('inward:window.doClose');
        }
    });
})(this);