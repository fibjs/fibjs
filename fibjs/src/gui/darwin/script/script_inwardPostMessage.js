(function (w) {
    window.addEventListener('load', function () {
        webkit.messageHandlers.__inward.postMessage('inward:window.load');
    });
})(this);