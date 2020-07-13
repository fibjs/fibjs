(function (w) {
    window.addEventListener('load', function () {
        window.webkit.messageHandlers.__inward.postMessage('inward:window.onLoad');
    });
})(this);