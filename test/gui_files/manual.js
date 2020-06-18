require('gui').open(process.env.FIBJS_GUI_URL || 'http://fibjs.org', {
    'script_after': `(function () { var script = document.createElement('script'); script.src="//cdn.jsdelivr.net/npm/eruda"; document.body.appendChild(script); script.onload = function () { eruda.init() } })();`
})