(function (w) {
    window.external = this;
    Object.defineProperty(window, 'postMessage', {
        writable: false,
        value: function (arg) { webkit.messageHandlers.invoke.postMessage(arg); }
    });
})(this);