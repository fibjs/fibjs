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

    window.external = external;
})(this);